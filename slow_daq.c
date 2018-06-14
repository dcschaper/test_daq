#include <CAENDigitizer.h>
#include <CAENComm.h>
//#include <CAENVMEoslib.h>
//#include <CAENVMEtypes.h>
//#include <CAENVMElib.h>
//#include "keyb.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>



CAEN_DGTZ_BoardInfo_t BoardInfo;
//typedef enum {
// ERR_NONE= 0,
// ERR_DGZ_OPEN,
// ERR_BOARD_INFO_READ,

  //ErrCode =
  // ERR_DGZ_OPEN =
  //ERR_BOARD_INFO_READ = 

int main(int argc, char *argv[])
{
  printf("\n");
  printf("******************************************************\n");
  printf("                 hello! \n");
  printf("******************************************************\n");

  system("fortune | cowsay");
  int ret=0;
  int handle=0;
  // CAENVME_Init(V1718, 0, 0, &handle);
  
  
  return 0;
}
