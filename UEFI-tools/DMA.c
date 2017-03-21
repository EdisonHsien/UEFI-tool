#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>
#include  <Library/IoLib.h>
#include  <Protocol/BlockIo.h>
#include  <Protocol/IsaIo.h>
#include  <Protocol/DevicePath.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PcdLib.h>

//What the fuck it's DMA


// logical block address
typedef UINT64 EFI_LBA;  

typedef struct _EFI_ISA_IO_PROTOCOL EFI_ISA_IO_PROTOCOL;

typedef enum {
  FdcDisk0   = 0,
  FdcDisk1   = 1,
  FdcMaxDisk = 2
} EFI_FDC_DISK;

typedef struct {
EFI_ISA_IO_PROTOCOL *IsaIo;
EFI_FDC_DISK         Disk;
}FDC_BLK_IO_DEV;

typedef struct {
  UINT8 CommandCode;
  UINT8 DiskHeadSel;
  UINT8 Cylinder;
  UINT8 Head;
  UINT8 Sector;
  UINT8 Number;
  UINT8 EndOfTrack;
  UINT8 GapLength;
  UINT8 DataLength;
} FDD_COMMAND_PACKET1;

typedef struct {
  UINT8 Status0;
  UINT8 Status1;
  UINT8 Status2;
  UINT8 Cylinder;
  UINT8 Head;
  UINT8 Sector;
  UINT8 Number;
} FDD_RESULT_PACKET;

UINTN
EFIAPI
MicroSecondDelay (
  IN      UINTN                     MicroSeconds
  )
{
  ASSERT (FALSE);
  return MicroSeconds;
}

/**
  Setup DMA channels to read data.

  @param  FdcBlkIoDev      Instance of FDC_BLK_IO_DEV.
  @param  Buffer           Memory buffer for DMA transfer.
  @param  BlockSize        the number of the bytes in one block.
  @param  NumberOfBlocks   Number of blocks to read.

**/
VOID
SetDMA (
  IN FDC_BLK_IO_DEV   *FdcBlkIoDev,
  IN VOID             *Buffer,
  IN UINTN            BlockSize,
  IN UINTN            NumberOfBlocks
  )
{
  UINT8 Data;
  UINTN Count;

  //
  // Mask DMA channel 2;
  //
  IoWrite8 (0x0a, 0x04 | 2);

  //
  // Clear first/last flip flop
  //
  IoWrite8 (0x0c, 0x04 | 2);

  //
  // Set mode
  //
  IoWrite8 (0x0b, 0x40 | 0x04 | 2);

  //
  // Set base address and page register
  //
  Data = (UINT8) (UINTN) Buffer;
  IoWrite8 (0x04, Data);
  Data = (UINT8) ((UINTN) Buffer >> 8);
  IoWrite8 (0x04, Data);

  Data = (UINT8) ((UINTN) Buffer >> 16);
  IoWrite8 (0x81, Data);

  //
  // Set count register
  //
  Count = BlockSize * NumberOfBlocks - 1;
  Data  = (UINT8) (Count & 0xff);
  IoWrite8 (0x05, Data);
  Data = (UINT8) (Count >> 8);
  IoWrite8 (0x05, Data);

  //
  // Clear channel 2 mask
  //
  IoWrite8 (0x0a, 0x02);
  
}

VOID
FillPara (
  IN  FDC_BLK_IO_DEV       *FdcDev,
  IN  EFI_LBA              Lba,
  IN  FDD_COMMAND_PACKET1  *Command
  )
{
  UINT8 EndOfTrack;

  //
  // Get EndOfTrack from the Para table
  //
  EndOfTrack = 0x12;

  //
  // Fill the command parameter
  //
  if (FdcDev->Disk == FdcDisk0) {
    Command->DiskHeadSel = 0;
  } else {
    Command->DiskHeadSel = 1;
  }

  Command->Cylinder = (UINT8) ((UINTN) Lba / EndOfTrack / 2);
  Command->Head     = (UINT8) ((UINTN) Lba / EndOfTrack % 2);
  Command->Sector   = (UINT8) ((UINT8) ((UINTN) Lba % EndOfTrack) + 1);
  Command->DiskHeadSel = (UINT8) (Command->DiskHeadSel | (Command->Head << 2));
  Command->Number     = 0x02;
  Command->EndOfTrack = 0x12;
  Command->GapLength  = 0x1b;
  Command->DataLength = 0xff;
}

EFI_STATUS
ReadWriteDataSector (
  IN  FDC_BLK_IO_DEV  *FdcDev,
  IN  VOID            *HostAddress,
  IN  EFI_LBA         Lba,
  IN  UINTN           NumberOfBlocks,
  IN  BOOLEAN         Read
  )
{
  
  FDD_COMMAND_PACKET1                           Command;
  FDD_RESULT_PACKET                             Result;
  UINTN                                         Index;
  UINTN                                         Times;
  UINT8                                         *CommandPointer;

//   UINT64                          DeviceAddress;

//   UINTN                                         NumberofBytes;

  
  
  
 


/*  Status = Seek (FdcDev, Lba);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }
  
  SetDMA(FdcDev, Buffer, ,NumberOfBlocks);  */

  //
  // Allocate Read or Write command packet
  //
  ZeroMem (&Command, sizeof (FDD_COMMAND_PACKET1));
  Command.CommandCode = 0x06 | 0x00000080 | 0x00000040 | 0x00000020;


  FillPara (FdcDev, Lba, &Command);

  //
  // Write command bytes to FDC
  //
  CommandPointer = (UINT8 *) (&Command);
 /* for (Index = 0; Index < sizeof (FDD_COMMAND_PACKET1); Index++) {
    if (EFI_ERROR (DataOutByte (FdcDev, CommandPointer++))) {
      return EFI_DEVICE_ERROR;
    }
  } */
  //
  // wait for some time
  //
  Times = (1000000 / 50) + 1;
  do {
//     if ((FdcReadPort (FdcDev, 4) & 0xc0) == 0xc0) {
//       break;
//     }

    MicroSecondDelay (50);
    Times = Times - 1;
  } while (Times > 0);

  if (Times == 0) {
    return EFI_TIMEOUT;
  }
  //
  // Read result bytes from FDC
  //
  CommandPointer = (UINT8 *) (&Result);
  for (Index = 0; Index < sizeof (FDD_RESULT_PACKET); Index++) {
/*    if (EFI_ERROR (DataInByte (FdcDev, CommandPointer++))) {
      return EFI_DEVICE_ERROR;
    } */
  }
 

  return 0;
}




INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{

  
 
 

  return(0);
}
