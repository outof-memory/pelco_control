#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>        /* 文件控制定义 */
#include <termios.h>      /* PPSIX终端控制定义 */
#include <cstring>
#include <string>
#include <iostream>
#include <errno.h>
#include <map>
using namespace std;

inline int CheckSum(unsigned char *command)
{
  int sum = 0;
  for (int i = 1; i < 6; i ++)
  {
    sum += command[i];
  }
  if (sum > 0xFF)
    command[6] = sum % 0x100;
  else
    command[6] = sum;
}

inline int cmd(unsigned char *command, char * cmdName){
  if (std::strcmp(cmdName, "clean") == 0 || std::strcmp(cmdName, "CLEAN") == 0){
    command[2] = 0x00; // cmd1
    command[3] = 0x00; // cmd2
  }else if (std::strcmp(cmdName, "right") == 0 || std::strcmp(cmdName, "RIGHT") == 0){
    command[2] = 0x00; // cmd1
    command[3] = 0x02; // cmd2
  }else if (std::strcmp(cmdName, "left") == 0 || std::strcmp(cmdName, "LEFT") == 0){
    command[2] = 0x00; // cmd1
    command[3] = 0x04; // cmd2
  }else if (std::strcmp(cmdName, "up") == 0 || std::strcmp(cmdName, "UP") == 0){
    command[2] = 0x00; // cmd1
    command[3] = 0x08; // cmd2
  }else if (std::strcmp(cmdName, "down") == 0 || std::strcmp(cmdName, "DOWN") == 0){
    command[2] = 0x00; // cmd1
    command[3] = 0x10; // cmd2
  }else if (std::strcmp(cmdName, "setp") == 0 || std::strcmp(cmdName, "SETP") == 0){
    command[2] = 0x00; // cmd1
    command[3] = 0x4b; // cmd2
  }else if (std::strcmp(cmdName, "sett") == 0 || std::strcmp(cmdName, "SETt") == 0){
    command[2] = 0x00; // cmd1
    command[3] = 0x4d; // cmd2
  }else if (std::strcmp(cmdName, "queryp") == 0 || std::strcmp(cmdName, "QUERYP") == 0){
    command[2] = 0x00; // cmd1
    command[3] = 0x51; // cmd2
  }else if (std::strcmp(cmdName, "queryt") == 0 || std::strcmp(cmdName, "QUERYT") == 0){
    command[2] = 0x00; // cmd1
    command[3] = 0x53; // cmd2
  }else{
    cout << "cmd name wrong!" << endl;
  }
}

int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void makeCommand(unsigned char* command, char * cmdName, int data1, int data2){
  command[0] = 0xFF; // sync
  command[1] = 0x01; // addr
  cmd(command, cmdName);
  command[4] = data1 & 0xff; // data1
  command[5] = data2 & 0xff; // data2
  CheckSum(command);
}

int recvData(int fd, char* cmdName){
  unsigned char buf[1024];
  bzero(buf, 1024);
  int rdlen;

  fd_set set;
  struct timeval timeout;
  int rv;
  FD_ZERO(&set); /* clear the set */
  FD_SET(fd, &set); /* add our file descriptor to the set */

  timeout.tv_sec = 1;
  timeout.tv_usec = 1000;
  rv = select(fd+ 1, &set, NULL, NULL, &timeout);
  if(rv == -1) {
    return -1;
  }
  else if(rv == 0){
    printf("command %s resonse, no motion \n", cmdName); /* a timeout occured */
    return 0;
  }
  else{
    rdlen = read( fd, buf, 1024); /* there was data to read */
    if (buf[0] == 0x01){
      printf("command %s resonse \n", cmdName);
    }else{
      printf("%s: %d \n", cmdName, buf[4]*256+buf[5]);
    }
    return 1;
  }

}

int main(int argc, char *argv[])
{
  char cmdName[10];
  int data(0), data1(0), data2(0);
  if (argc > 1){
    sprintf(cmdName, "%s", argv[1]);
  }else{
    sprintf(cmdName, "CLEAN");
  }
  if (argc > 2){
    data = atoi(argv[2]);
    data1 = data / 256;
    data2 = data % 256;
  }

  char *dev = "/dev/ttyTHS0";
  int fd = open(dev, O_RDWR | O_NOCTTY | O_SYNC);
  if (fd > 0)
    set_interface_attribs(fd, B9600);
  else
  {
    printf("Can't Open Serial Port '%s'\n ", dev);
    return -1;
  }

  unsigned char command[7];
  printf("command %s: %02x %02x %02x %02x %02x %02x %02x \n", cmdName,
         command[0], command[1], command[2], command[3], command[4], command[5], command[6]
         );
  makeCommand(command, cmdName, data1, data2);
  write(fd, command, sizeof(command));
  int ret = recvData(fd, cmdName);
  if (ret == -1){
    cout << "wrong" << endl; 
  }else if (ret == 1){
    cout << "reponse correctly" << endl; 
  }else if (ret == 0){
    cout << "reponse correctly, but no motion" << endl; 
  }

  close(fd);

  return 0;
}
