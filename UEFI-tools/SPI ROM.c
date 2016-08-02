#include  <industrystandard/Acpi50.h>
#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>
#include  <Library/IoLib.h>
#include  <Library/BaseMemoryLib.h>
#include  <Base.h>


#define OEM_PCIEX_BASE_ADDRESS 				OEM_GetPciBaseAddr()  
#define EFI_ACPI_SERIAL_BUS_RESOURCE_TYPE_SPI 0x2
#define PCIEX_BASE_ADDRESS 0
#define SPI_WREN        0x06     //Set Write Enable Latch
#define SPI_WRSR        0x01     //Write Status Register
#define SPI_OPMENU      0x98     //Opcode Menu Configuration Register
#define SPI_OPTYPE      0x96     //Opcode Type Configuration Register
#define SPI_PREOP       0x94     //Prefix Opcode Configuration Register
#define SB_RCBA         0xFED1C000

extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE  *gST;

typedef EFI_STATUS (EFIAPI *FLASH_READ_WRITE)(
    VOID* FlashAddress, UINTN Size, VOID* DataBuffer
);

typedef struct _FLASH_PROTOCOL {
    FLASH_READ_WRITE Write; //Write only
}FLASH_PROTOCOL;

typedef struct {

	UINTN					OEM_PLATFORM;
	UINT16  				*OEM_FileName;
	UINTN  					OEM_FileNameSize;
	UINT32					OEM_BinFRBA;
	UINTN 					OEM_FlashBlockSize;
	UINT32 					OEM_FlashWriteSize;
	UINTN 					OEM_DescriptorFlashBlockSize;
	FLASH_PROTOCOL 				*OEM_Flash;


	UINT8   				*OEM_BinBuffer;
	UINTN 					OEM_BinSize;
	UINTN                           	OEM_BIOSSize;
	UINTN                           	OEM_FlashSize;
  UINTN					OEM_DescriptorFlashSize;
	


}OEM_FLASHAP_DATA;



VOID
OemFlashDeviceWriteEnable (VOID)
{
    
    UINTN OemFlash_Status_Register	= 0xFED1F804;	
    UINT32 OemFlash_Protected_Register[5] = {0};
		
    OemFlash_Status_Register &=  0x7FFF;					
		
		OemFlash_Protected_Register[0] = *(UINT32*)( 	OemFlash_Status_Register + 0x70);		
		*(UINT32*)( OemFlash_Status_Register + 0x70) &= 0x7FFF7FFF;				
    
}

VOID
FlashDeviceWriteEnable  (VOID)
{
    OemFlashDeviceWriteEnable ();

}

VOID OEM_FlashWriteEnable(OEM_FLASHAP_DATA *oemflashapdata)
{			
       	//OEM_AsmFlashSmi(oemflashapdata->OEM_FuncBuffer, 0x20,SW_SMI_IO_ADDRESS);
	FlashDeviceWriteEnable();
}

VOID OEM_WriteFile(OEM_FLASHAP_DATA *oemflashapdata,UINT16 number)
{
    EFI_STATUS Status;
  
        
  UINT16                          *ShowWriteFlash = NULL;
	UINT32         			*BlockAddress 	= 0;
	

	Status = gBS->AllocatePool(EfiBootServicesData, 0x60 * sizeof(UINT16), &ShowWriteFlash);
        gBS->SetMem(ShowWriteFlash, 0x60 * sizeof(UINT16), 0);

	*BlockAddress =(UINT32)(number * oemflashapdata->OEM_FlashBlockSize + (0xFFFFFFFF - oemflashapdata->OEM_FlashSize + 1));
          *BlockAddress &= (0xFFFFFFFF - oemflashapdata->OEM_FlashBlockSize + 1);

	OEM_FlashWriteEnable(oemflashapdata);
	Status = oemflashapdata->OEM_Flash->Write(BlockAddress,
						  oemflashapdata->OEM_FlashBlockSize, 
						  (UINT8*)(oemflashapdata->OEM_BinBuffer+number*oemflashapdata->OEM_FlashBlockSize+oemflashapdata->OEM_FlashSize-oemflashapdata->OEM_FlashSize));
}



INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{
//     UINT16 number=1;
//     OEM_FLASHAP_DATA OEM;
//     OEM.OEM_PLATFORM=0;
//      CpuDeadLoop ();
//     OEM_WriteFile(&OEM,number);
  
  
  EFI_STATUS Status;
  UINT32 SPIBAR=0xFED1F800; 
  
  
 
  
//   MmioWrite8(0xDC,IoRead8(0xDC)|0x01);
//   
//   MmioWrite16(SPIBAR+0x4,MmioRead16(SPIBAR+0x4)&0x7FFF);
//   MmioWrite32(SPIBAR+0x74,MmioRead16(SPIBAR+0x74)&0x7FFF7FFF);
  
  
//   MmioWrite32(SPIBAR+SPI_OPMENU,0xC7);
//   MmioWrite16(SPIBAR+SPI_PREOP,0x06);
  
  MmioWrite32(SPIBAR+SPI_OPMENU,0x02);
  MmioWrite16(SPIBAR+SPI_OPTYPE,0x03);
  MmioWrite16(SPIBAR+SPI_PREOP,0x06);
//   MmioWrite8(SPIBAR+0x90,MmioRead8(SPIBAR+0x90)&0xFB);
  MmioWrite32(SPIBAR+0x08,0x05);
  Status=MmioWrite32(SPIBAR+0x10,0x15);
  
  MmioWrite16(SPIBAR+0x91,0x0106); 
  


//     MmioWrite8(SPIBAR+0x06,0x07);
//     MmioWrite8(SPIBAR+0x04,0x08);
//     MmioWrite32(SPIBAR+0x08,0x10); 
//     MmioWrite8(SPIBAR+0x07,0x01);
//     Status=MmioWrite32(SPIBAR+0x10,0x20);
//     MmioWrite8(SPIBAR+0x06,0x05);
    
              
//   Status=MmioWrite8(SPIBAR+0x90,0xFF);     
//   Print (L"Status: %r\n", Status);  
//   Status=MmioWrite32(SPIBAR+0x08,0x50);     
//   Print (L"Status: %r\n", Status);
//   Status=MmioWrite8(SPIBAR+0x07,0x01);
//   Print (L"Status: %r\n", Status);
//   Status=MmioWrite8(SPIBAR+0x06,0x01);
//   Print (L"Status: %r\n", Status);
//   Status=MmioRead32(SPIBAR+0x10);
//   Print (L"Status: %r\n", Status);   




  return(0);
}


