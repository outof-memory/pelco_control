/* pelco serial control demo */
#include "pelcoSerial.h"
int main(int argc, char *argv[]){
  char cmdName[10] = "CLEAN";
  int data(0);
  if (argc > 1){
    bzero(cmdName, 10);
    sprintf(cmdName, "%s", argv[1]);
  }
  if (argc > 2){
    data = atoi(argv[2]);
  }

  char dev[20] = "/dev/ttyTHS0";
  pelco handle(dev);
  handle.running(cmdName, data);
}
