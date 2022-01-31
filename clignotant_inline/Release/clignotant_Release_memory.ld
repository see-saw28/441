/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2019
 * Generated linker script file for LPC804
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.23
 * MCUXpresso IDE v10.2.1 [Build 795] [2018-07-25] on 14 oct. 2019 23:16:51
 */

MEMORY
{
  /* Define each memory region */
  MFlash32 (rx) : ORIGIN = 0x0, LENGTH = 0x7f80 /* 32640 bytes (alias Flash) */  
  RamLoc4 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x1000 /* 4K bytes (alias RAM) */  
}

  /* Define a symbol for the top of each memory region */
  __base_MFlash32 = 0x0  ; /* MFlash32 */  
  __base_Flash = 0x0 ; /* Flash */  
  __top_MFlash32 = 0x0 + 0x7f80 ; /* 32640 bytes */  
  __top_Flash = 0x0 + 0x7f80 ; /* 32640 bytes */  
  __base_RamLoc4 = 0x10000000  ; /* RamLoc4 */  
  __base_RAM = 0x10000000 ; /* RAM */  
  __top_RamLoc4 = 0x10000000 + 0x1000 ; /* 4K bytes */  
  __top_RAM = 0x10000000 + 0x1000 ; /* 4K bytes */  
