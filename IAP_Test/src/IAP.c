/****************************************************************************
 *   $Id:: iaptest.c 4054 2010-07-30 00:02:55Z usb01267                  $
 *   Project: NXP LPC13Uxx IAP example
 *
 *   Description:
 *     This file contains IAP test modules, main entry, to test IAP APIs.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <iap.h>
#include "LPC8xx.h"
#include "FlashPrg.h"
#include "syscon.h"
#include "swm.h"
#include "utilities.h"
#include "iap.h"
#include "chip_setup.h"

void setup_debug_uart(void);

#define WHICH_SECTOR END_SECTOR // 63
#define WHICH_PAGE   7
#define ADDR ((WHICH_SECTOR * SECTOR_SIZE) + (WHICH_PAGE * PAGE_SIZE))
#define NUM_BYTES    64

char the_data[NUM_BYTES];
uint32_t uid[4];
static struct sIAP IAP;

uint32_t ReadIdentification(void)
{
  IAP.cmd = IAP_READ_PART_ID;
  IAP_Call (&IAP.cmd, &IAP.stat);
  printf("Part ID is 0x%X\n\n",IAP.res[0] );
  if (IAP.stat) return (0);

  return (IAP.res[0]);
}

uint32_t ReadBootcodeVersion(void)
{
  IAP.cmd = IAP_READ_BOOT_VER;
  IAP_Call (&IAP.cmd, &IAP.stat);
  printf("Boot Version is 0x%X\n\n",IAP.res[0] );
  if (IAP.stat) return (0);

  return (IAP.res[0]);
}

int ReadUID(uint32_t * uid)
{
  if (uid == 0)
    return 1;

  IAP.cmd = IAP_READ_UID;
  IAP_Call (&IAP.cmd, &IAP.stat);
  if (IAP.stat) return (1);

  uid[0] = IAP.res[0];
  uid[1] = IAP.res[1];
  uid[2] = IAP.res[2];
  uid[3] = IAP.res[3];
  printf("UID is 0x%X %X %X %X\n\n", uid[3],uid[2],uid[1],uid[0]);
  printf("\rRead UID successful\n\n\r");
  return (0);
}


int main(void) {
  uint32_t k;
  uint32_t ret_code;
  volatile uint32_t i = 0;
    volatile uint32_t command =0;
    // Configure the debug uart (see Serial.c)
    setup_debug_uart();

    SystemCoreClockUpdate();

    while(1)
    {

	/* remap to show all of Flash */
    LPC_SYSCON->SYSMEMREMAP = 2;

    IAP.cmd = 0;
    IAP.stat = 0;

	// Initialize the data array
	for (k=0; k<NUM_BYTES; k++)
    the_data[k] = 0x55;


    printf("\rChoose the IAP command you want to execute:\n\r1. ReadIdentification\n\r2. ReadBootcodeVersion\n\r3. ReadUID\n\r4. ErasePage\n\r5. Program flash and Mem compare\n\r\n\r");
    scanf("%d",&command);

  // Disable all interrupts during IAP calls
  __disable_irq();


   switch(command){
				  case 1:
				  {
					  /* Read Identification */
					  i = ReadIdentification();
					   if ((i & 0x00008042) != 0x00008042)
					   {
					 	printf ("\rERROR! ReadIdentification failed.\n\r");
					     while (1);
					   }
					   else
					 	  printf ("\rReadIdentification successful\n\n\r");
					   break;

				  }
				  case 2:
				  {   /* Read Bootcode Version */
					  i = ReadBootcodeVersion();
					    if ((i & 0x00000D01) != 0x00000D01)
					    {
					  	printf ("\rERROR! ReadBootcodeVersion failed\n\r");
					      while (1);
					    }
					    else
					  	  printf ("\rReadBootcodeVersion successful\n\n\r");
					    break;
				  }
				  case 3:
				  {
					  if(ReadUID(uid))
					    {
					  	  printf("ERROR! Read UID failed!\n");
					  	  while(1);
					    }
					  break;
				  }
				  case 4:
				  {

				  // IAP Prepare Sectors for Write command can be skipped since we are using
				  // the drivers in FlashPrg.c which do the Prepare.

				  // IAP Erase Page command (pass the start address of the last page in the last sector; sector 63, page 15)
				  // The ending address, or any other address in the page will work also. The driver handles the math.
				  if ((ret_code = ErasePage(ADDR))) {
					printf ("ErasePage failed. Return code was 0x%X\n\r", ret_code);
					while(1);
				  }
				  else
					  printf("\rErasePage succeeded. Return code was 0x%X\n\r", ret_code);
				  break;
				  }

				  case 5:
				  {
				  // IAP Copy RAM to Flash command
					// Print the prompt
				  printf("Enter something to be programmed to Flash memory:\n\r");

				  // Load the data array with the user's input
				  GetTerminalString(pDBGU, the_data);
				  if ((ret_code = Program(ADDR, NUM_BYTES, (uint8_t *)the_data))) {
					printf ("\rCopy RAM to Flash failed. Return code was 0x%X\n\r", ret_code);
					while(1);
				  }
				  else
					 printf ("\rCopy RAM to Flash succeeded. Return code was 0x%X\n\r", ret_code);

				  // IAP Compare command
				  if ((ret_code = Compare(ADDR, (uint32_t)the_data, NUM_BYTES))) {
					printf ("\rCompare failed. Return code was 0x%X\n\r", ret_code);
					while(1);
				  }
				  else
					 printf ("\rCompare memory succeeded. Return code was 0x%X\n\r", ret_code);
				printf("\rProgramming succeeded.\n\n\r");
				break;
				  }
				  default:
				  {  printf("\rINVALID command\n\r");
					  exit(0);
   	   	   	   	   }
  // Reenable interrupts
  __enable_irq();

   }

  } // end of while(1)

}  // end of main
