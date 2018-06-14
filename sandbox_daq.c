#include <CAENDigitizer.h>
#include <CAENComm.h>
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
  /* printf("\n"); */
  /* printf("******************************************************\n"); */
  /* printf("                 hello! \n"); */
  /* printf("******************************************************\n"); */

  CAEN_DGTZ_ErrorCode ret;
  CAENComm_ErrorCode ret2;
  int handle;
  int i, j;
  CAEN_DGTZ_BoardInfo_t BoardInfo;
  
  // ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB, 1, 0, 0x00A00000, &handle);
  ret2 = CAENComm_OpenDevice(CAENComm_USB, 1, 0, 0x00A00000, &handle);

  ret2 = CAENComm_Write16(handle, 0x00C, 0x1);   //reset

  ret2 = CAENComm_Write16(handle, 0x002, 0xC000); //9C00 for ext trig

  uint16_t trig;
  ret2 = CAENComm_Read16(handle, 0x002, &trig); //9C00 for ext trig
  fprintf(stderr,"trigger register is %04x\n",trig);
  
  ret2 = CAENComm_Write16(handle, 0x004, 0xE440); //1024/8 =128 samples

  uint16_t sample;
  ret2 = CAENComm_Read16(handle, 0x004, &sample); //9C00 for ext trig
  fprintf(stderr,"sample register is %04x\n",sample);

  uint16_t rate;
  ret2 = CAENComm_Write16(handle, 0x006, 0x1385);//2.5 KHz sampling rate (0x1385 =4997 and then +3 by definition)
   ret2 = CAENComm_Read16(handle, 0x006, &rate);
   fprintf(stderr,"rate is %i\n", rate);
   
  ret2 = CAENComm_Write16(handle, 0x008, 0xFF00); //disable Interrupts

  ret2 = CAENComm_Write16(handle, 0x010, 0xFF00); //Default gain of 1


  
  uint16_t dataRDY;
  unsigned short buff[0x400]={0};
  CAENComm_ErrorCode error[0x400]={CAENComm_Success};
  uint32_t address=0x80;

 do {
	CAENComm_Read16(handle, 0x002, &dataRDY);
        /* fprintf(stderr,"reg02: %x\n", dataRDY); */
	
      }
      while(!(dataRDY >> 7 & 1));
 
 /* fprintf(stderr,"Calibration Complete\n");  */
  
  for(i=0;i<100;i++) 
    {
      ret2 = CAENComm_Write16(handle, 0x000E, 0x1);
      /* fprintf(stderr,"SW Trigger number %i issued\n", i); */
      do {
	CAENComm_Read16(handle, 0x002, &dataRDY);
	// fprintf(stderr,"reg02: %x\n", dataRDY);
      }
      while(!(dataRDY >> 6 & 1));

      /* fprintf(stderr,"I'm reading...\n"); */

     
      /* for(i=0; i< 0x400; i++) */
      /* 	{CAENComm_MultiRead16(handle, 0x80+2i,  */
      /* 	} */

      /* CAENComm_MultiRead16(handle, &address, 0x400, buff, error); */
      // fprintf(stderr,"before Read statement\n");
      for(j=0; j<0x400;j++) {
	CAENComm_Read16(handle, 0x80+2*j, buff+j);
	usleep(1000);
 	fprintf(stderr,"%04hx ", buff[j]);
 	if (!((j+1)%16))fprintf(stderr,"\n");
      }
      /* fwrite(buff, 2, 0x400, stdout); */
      //fwrite(error, 2, 0x400, stdout);
      fprintf(stderr,"\after write statement\n");
      bzero(buff,0x800);
    }



  

  
  //fprintf(stderr,"after readout loop\n");
  //  CAEN_DGTZ_Reset (handle);

  // ret2 = CAENComm_Write16(handle, 0x002, 0xF0D0);

  
 QuitProgram:
  // if (ErrCode)
  // {
      /* fprintf(stderr,"\nquitting program\n"); */
  // }
  
  CAEN_DGTZ_SWStopAcquisition(handle);
  CAEN_DGTZ_CloseDigitizer(handle);


  
  return 0;
}
