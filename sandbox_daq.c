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

  ret2 = CAENComm_Write16(handle, 0x00C, 0x1);

  ret2 = CAENComm_Write16(handle, 0x002, 0x8800); //9C00 for ext trig

  ret2 = CAENComm_Write16(handle, 0x004, 0xE440); //1024/8 =128 samples

  ret2 = CAENComm_Write16(handle, 0x006, 0x1385); //2.5 KHz sampling rate (0x1385 =4997 and then +3 by definition)

  ret2 = CAENComm_Write16(handle, 0x008, 0xFF00); //disable Interrupts

  ret2 = CAENComm_Write16(handle, 0x010, 0xFF00); //Default gain of 1

  uint16_t dataRDY;
  unsigned short buff[0x400];
  CAENComm_ErrorCode error[0x400];
  uint32_t address=0x80;
  for(i=0; i<100; i++) 
    {
      ret2 = CAENComm_Write16(handle, 0x00E, 0x1);
      printf("SW Trigger number %i issued\n", i);
      do CAENComm_Read16(handle, 0x002, &dataRDY);
      while(!(dataRDY >> 6 & 1));
      printf("I'm reading...\n");

     
      /* for(i=0; i< 0x400; i++) */
      /* 	{CAENComm_MultiRead16(handle, 0x80+2i,  */
      /* 	} */

      CAENComm_MultiRead16(handle, &address, 0x400, buff, error);
      /* for(i=0; i<0x400 */
      printf("before write statement\n");
      fwrite(buff, 2, 0x400, stdout);
      printf("after write statement\n");
       usleep(10000);
    }



  

  
  printf("after readout loop\n");
  //  CAEN_DGTZ_Reset (handle);

  // ret2 = CAENComm_Write16(handle, 0x002, 0x00D0);

  
 QuitProgram:
  // if (ErrCode)
  // {
      printf("\nquitting program\n");
  // }
  
  CAEN_DGTZ_SWStopAcquisition(handle);
  CAEN_DGTZ_CloseDigitizer(handle);


  
  return 0;
}
