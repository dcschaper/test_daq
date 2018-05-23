#include <CAENDigitizer.h>
#include <CAENComm.h>
//#include "keyb.h"
#include <stdio.h>
#include <stdlib.h>




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

  CAEN_DGTZ_ErrorCode ret;
  CAENComm_ErrorCode ret2;
  int handle;
  int i;
  CAEN_DGTZ_BoardInfo_t BoardInfo;
  
  // ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB, 1, 0, 0x00A00000, &handle);
  ret2 = CAENComm_OpenDevice(CAENComm_USB, 1, 0, 0x00A00000, &handle);

  if(ret)
    {
      printf("Error code is %d, handle is %i \n", ret, handle);
      // ErrCode = ERR_DGZ_OPEN;
      goto QuitProgram;
    }
  printf("hi");
  //  CAEN_DGTZ_Reset (handle);

  ret2 = CAENComm_Write16(handle, 0x002, 0xF0D0);
  
  CAEN_DGTZ_IOLevel_t info;
  uint32_t iolvl;
  ret = CAEN_DGTZ_SetIOLevel(handle, CAEN_DGTZ_IOLevel_TTL); //if I change to TTL, printed numbers change accordingly. I must be missing some sort of base conversion...why are they printing with 10 digits instead of 8? Why are they all printing with 2 leading 4s???
 ret = CAEN_DGTZ_GetIOLevel(handle, &info);
 ret = CAEN_DGTZ_ReadRegister(handle, 0x811C, &iolvl);
 printf("\nSuccessfully connected! IO Level is %i or %08x", info, iolvl);

  usleep(3000000);

  ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
  printf("\nConnected to CAEN Digitizer, Model %s", BoardInfo.ModelName);

  uint32_t length;
  ret = CAEN_DGTZ_SetRecordLength(handle, 4096);
  ret = CAEN_DGTZ_GetRecordLength(handle, &length);
  printf("\nRecord Length set to %i", length);

  

 QuitProgram:
  // if (ErrCode)
  // {
      printf("\nquitting program\n");
  // }
  
  CAEN_DGTZ_SWStopAcquisition(handle);
  CAEN_DGTZ_CloseDigitizer(handle);


  
  return 0;
}
