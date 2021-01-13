#ifndef PELCO_SERIAL_H
#define PELCO_SERIAL_H
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
using namespace std;

class pelco{
public:
  int fd;
  pelco(char* dev);
  ~pelco();
  int running(char*, int data);
  int recvData(int fd, char* cmdName);

private:
  int set_interface_attribs(int fd, int speed);
  void makeCommand(unsigned char* command, char * cmdName, int data1, int data2);
  inline int CheckSum(unsigned char *command);
  inline int cmd(unsigned char *command, char * cmdName);
};
#endif //
