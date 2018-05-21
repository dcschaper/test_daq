#include <CAENDigitizer.h>
//#include "keyb.h"
#include "keyb.c"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAXNUMBRD 1 //Number of connected boards
//******************
// Global Variables
//******************

int run_number;
long old_run_time;
long new_run_time;
FILE *obfile;
uint16_t waveform[15000];
//maybe DEVT_BANK?



int RegisterSetBits(int handle, uint16_t addr, int start_bit, int end_bit, int val)
{
	uint32_t mask=0, reg;
	int ret;
	int i;

	ret = CAEN_DGTZ_ReadRegister(handle, addr, &reg);   
	for(i=start_bit; i<=end_bit; i++)
		mask |= 1<<i;
	reg = reg & ~mask | ((val<<start_bit) & mask);
	ret |= CAEN_DGTZ_WriteRegister(handle, addr, reg);   
	return ret;
}


void new_binfile()
{
   FILE *inFile;
   inFile = fopen("./run_number","r");
   int rnum = 0;
   fscanf(inFile,"%d",&rnum);
   fclose(inFile);
   rnum += 1;
   FILE *outFile;
   outFile = fopen("./run_number","w");
   fprintf(outFile,"%d",rnum);
   fclose(outFile);
   char buf[128];
   sprintf(buf,"./data/run%05d.bin",rnum); //use 5 digits with leading zeroes
   run_number = rnum;
   // old_run_time = get_time(); //should our daq be based on the run time or the total number of triggers received? I think triggers
   obfile = fopen(buf,"w");
}

void close_binfile()
{
  fclose(obfile);
}

int checkCommand(){
  if(!kbhit()) return 0;
  switch (getch()){
  case 's': return 9; break;
  case 'k': return 1; break;
  case 'q': return 2; break;
  }
  return 0;
}

//**********************************************************
//MAIN
//**********************************************************


int main(int argc, char *argv[])
{
  printf("\n");
  printf("******************************************************\n");
  printf("                 hello! \n");
  printf("******************************************************\n");


  int c=0;
  CAEN_DGTZ_ErrorCode ret;
  int handle;
  CAEN_DGTZ_BoardInfo_t BoardInfo;
  CAEN_DGTZ_IOLevel_t level;
  uint32_t reclength;
  uint32_t mask = 0b00001111; //LSB (rightmost bit) is channel zero, followed by channel 1, 2..7, read right to left.
  CAEN_DGTZ_TriggerMode_t mode;
  uint32_t posttrig = 80;
  uint32_t percent;
  uint32_t DCoffset=0x7FFF;
  uint32_t dcoffset;
  char *buffer = NULL;
  uint32_t size;
  uint32_t bsize;
  uint32_t numEvents;

  
  ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB, 0, 0, 0x32100000, &handle);
  if(ret)
    {
      fprintf(stderr,"Error code is %d, handle is %i \n", ret, handle);
      // ErrCode = ERR_DGZ_OPEN;
      goto QuitProgram;
    }

  ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
  printf("\nConnected to CAEN Digitizer Model %s, recognized as board %d\n", BoardInfo.ModelName, handle);
  printf("\tROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
  printf("\tAMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);

  ret = CAEN_DGTZ_Reset(handle);

  ret = CAEN_DGTZ_SetIOLevel(handle, CAEN_DGTZ_IOLevel_TTL);
  ret = CAEN_DGTZ_GetIOLevel(handle, &level);
  printf("Front Panel I/O Level is %i (1 for TTL, 0 for NIM)\n", level);

  ret = CAEN_DGTZ_SetRecordLength(handle, 4096);
  ret = CAEN_DGTZ_GetRecordLength(handle, &reclength);
  printf("Record Length is set to %08x\n", reclength);

  ret = CAEN_DGTZ_SetChannelEnableMask(handle, mask);
  ret = CAEN_DGTZ_GetChannelEnableMask(handle, &mask);
  printf("Channel Enable Mask set to %i, %x in hex\n", mask, mask);

  //ret = CAEN_DGTZ_SetExtTriggerInputMode(handle, CAEN_DGTZ_TRGMODE_ACQ_ONLY);
  // ret = CAEN_DGTZ_GetExtTriggerInputMode(handle, &mode);
  // printf("Trigger mode is %i\n", mode);

   ret = CAEN_DGTZ_SetSWTriggerMode(handle, CAEN_DGTZ_TRGMODE_ACQ_ONLY);
  ret = CAEN_DGTZ_GetSWTriggerMode(handle, &mode);
  printf("Trigger mode is %i\n", mode);

  
  ret = CAEN_DGTZ_SetPostTriggerSize(handle, posttrig);
  ret = CAEN_DGTZ_GetPostTriggerSize(handle, &percent);
  printf("Post-trigger record length set to %i percent\n", percent);

  ret = CAEN_DGTZ_SetChannelDCOffset(handle, -1, DCoffset);
  ret = CAEN_DGTZ_GetChannelDCOffset(handle, 0, &dcoffset);
  printf("DC offset is set to %i for all channels\n", dcoffset);

  ret = CAEN_DGTZ_SetChannelPulsePolarity(handle, -1, CAEN_DGTZ_PulsePolarityPositive); //can also set to negative pulse polarity

  
  
    
  printf("hi!\n");
  
  printf("Successfully connected!\n");

  usleep(1000000);



  
  int err; //note that function definition at the top returns an int instead of CAEN_DGTZ_ErrorCode
  uint32_t data;
  err = RegisterSetBits(handle, 0x8044, 0, 7, 1);
  ret = CAEN_DGTZ_ReadRegister(handle, 0x8044, &data);
  printf("Decimation factor is set to %08x\n", data);

  printf("Press 's' to start acquisition, 'q' to quit\n");

  
 ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &buffer, &size);
 if(ret) {
   printf("cannot allocate memory!");
 }
 else{
   printf("Memory allocated. Buffer size is %08x\n", size);
 }
 
  while(1)
     {
       c = checkCommand();
       if(c==9) break;
       //if(c==2) return 0;
       Sleep(100);
     }

 
 //*****************************************************
 //READOUT
 //*****************************************************
 
  
  ret = CAEN_DGTZ_SWStartAcquisition(handle);
  printf("Starting SW Acq");
  int i;
  while(1)
    {
      // ret = CAEN_DGTZ_SendSWtrigger(handle);

   
   ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &bsize); //read the buffer from the digitizer

   //fprintf(stderr, "buffer size is %d\n", bsize);
   
   ret = CAEN_DGTZ_GetNumEvents(handle, buffer, bsize, &numEvents);

   if(numEvents>0)
     {
   fprintf(stderr,"Board retrieved %d event(s)\n", numEvents);
     }
     fprintf(stderr, buffer, bsize);


   //ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &bsize); //read the buffer from the digitizer
    
   c = checkCommand();
   if (c==1) goto Continue;
   if (c==2) goto Continue;
    }

 Continue:
  printf("\nBoard retrieved %d events\n", numEvents);  
  
  
  //Need to fwrite events??
  
 QuitProgram:
  // if (ErrCode)
  // {
      printf("quitting program\n");
  // }
      ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer);     
  CAEN_DGTZ_SWStopAcquisition(handle);
  CAEN_DGTZ_CloseDigitizer(handle);

  return 0;
}
