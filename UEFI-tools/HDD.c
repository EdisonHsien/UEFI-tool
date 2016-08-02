/** @file
    A simple, basic, EDK II native, "hello" application to verify that
    we can build applications without LibC.

    Copyright (c) 2010 - 2011, Intel Corporation. All rights reserved.<BR>
    This program and the accompanying materials
    are licensed and made available under the terms and conditions of the BSD License
    which accompanies this distribution. The full text of the license may be found at
    http://opensource.org/licenses/bsd-license.

    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/
#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>
#include  <Protocol/AtaPassThru.h>
#include  <Library/IoLib.h>
#include  <Library/ShellLib.h>
#include  <Industrystandard/Atapi.h>
#include  <Protocol/DiskInfo.h>
#define   EFI_ATA_PASS_THRU_PROTOCOL_GUID \
  { \
    0x1d3de7f0, 0x807, 0x424f, {0xaa, 0x69, 0x11, 0xa5, 0x4e, 0x19, 0xa4, 0x6f } \
  }
#define EFI_DISK_INFO_PROTOCOL_GUID \
  { \
    0xd432a67f, 0x14dc, 0x484b, {0xb3, 0xbb, 0x3f, 0x2, 0x91, 0x84, 0x93, 0x27 } \
  }

#define ABAR  0xFBB01100  // AHCI Base Address
#define PxCLB 0x72546400
#define FIS   0x72546480
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL  0x00000002
#define ATA_TIMEOUT                       EFI_TIMER_PERIOD_SECONDS (3)

extern EFI_HANDLE  gImageHandle;
extern EFI_BOOT_SERVICES *gBS;


VOID *
EFIAPI
InternalMemSetMem (
  OUT     VOID                      *Buffer,
  IN      UINTN                     Length,
  IN      UINT8                     Value
  )
{
  //
  // Declare the local variables that actually move the data elements as
  // volatile to prevent the optimizer from replacing this function with
  // the intrinsic memset()
  //
  volatile UINT8                    *Pointer;

  Pointer = (UINT8*)Buffer;
  while (Length-- > 0) {
    *(Pointer++) = Value;
  }
  return Buffer;
}

VOID *
EFIAPI
InternalMemZeroMem (
  OUT     VOID                      *Buffer,
  IN      UINTN                     Length
  )
{
  return InternalMemSetMem (Buffer, Length, 0);
}

VOID *
EFIAPI
ZeroMem (
  OUT VOID  *Buffer,
  IN UINTN  Length
  )
{
 
  return InternalMemZeroMem (Buffer, Length);
}



INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{

EFI_STATUS  Status;
// EFI_GUID gEfiDiskInfoProtocolGuid=EFI_DISK_INFO_PROTOCOL_GUID;
// EFI_DISK_INFO_PROTOCOL  *This;
// UINTN *IdentifyData=NULL;
// UINT32 IdentifyDataSize=0;



//      UINTN Index =1;
//      UINT32 PRDT =0x040FE4F0;  //Physical Region Descriptor Table
//      UINT32 FFIS= MmioRead32(FIS+0x8C);
//      UINT32 FFFIS=MmioRead32(FIS+0x8C+0x16);
//      UINT8 AABAR=MmioRead8(ABAR+0x18);
//      UINTN FUCK=0xFBB01138;
//      UINT8  YOU=0x01;
  
  
  EFI_GUID  gEfiAtaPassThruProtocolGuid=EFI_ATA_PASS_THRU_PROTOCOL_GUID;
  EFI_ATA_PASS_THRU_PROTOCOL *AtaPassThru=NULL;
  
  
  UINT16 Port=0xFFFF;
  UINT16 PortMultiplierPort=0xFFFF;
  ATA_IDENTIFY_DATA    *IdentifyData=NULL;
  EFI_ATA_PASS_THRU_COMMAND_PACKET  *Packet=NULL;
  EFI_ATA_COMMAND_BLOCK   *Acb=NULL;
//   Acb=ZeroMem(Acb,sizeof (EFI_ATA_COMMAND_BLOCK));
  Acb->AtaCommand= 0xb0 ;
  Acb->AtaSectorCount=0x01;
  Acb->AtaCylinderLow=0x4F;
  Acb->AtaCylinderHigh=0xC2;
  Acb->AtaFeatures=0xD0;
   Status=gBS->InstallProtocolInterface(
         &gImageHandle,
         &gEfiAtaPassThruProtocolGuid,
         EFI_NATIVE_INTERFACE,
         AtaPassThru
         );
  Print(L"Status:%r\n",Status);
  Status=gBS->LocateProtocol( &gEfiAtaPassThruProtocolGuid,
                             NULL,
                             (VOID **)&AtaPassThru
                             );
  Print(L"Status:%r\n",Status);

  Status=AtaPassThru->GetNextPort(AtaPassThru,&Port);
  Print(L"Status:%r\n",Status);
  Print(L"Port Number:%x\n",Port);
  
  Status=AtaPassThru->GetNextDevice(AtaPassThru,Port,&PortMultiplierPort);
  Print(L"Status:%r\n",Status);
  Print(L"MultiplierPort Number:%x\n",PortMultiplierPort);
  
  Acb->AtaDeviceHead = 0xA0;
//   Packet=ZeroMem(Packet,sizeof (EFI_ATA_PASS_THRU_COMMAND_PACKET));
  Packet->InTransferLength = 0;
  Packet->Timeout = ATA_TIMEOUT;
  Packet->Length  = EFI_ATA_PASS_THRU_LENGTH_BYTES;
  Packet->Protocol= EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_OUT;
  Packet->OutTransferLength = 512;
  Packet->OutDataBuffer=IdentifyData;
  Packet->InDataBuffer =NULL;
  Packet->Acb=Acb;
 
  
  Status=AtaPassThru->PassThru(AtaPassThru, Port, PortMultiplierPort, Packet, NULL);
  Print(L"Status:%r\n",Status);



  
//      MmioWrite8(FIS+0x02,0xEC);
//      FFIS|=1;  
//      MmioWrite32(FIS+0x8C,FFIS);
//      FFFIS|=(1<<15);
//      MmioWrite32(FIS+0x8C+0x16,FFFIS);
//      AABAR|=(1<<4);
//      Status=MmioWrite8(ABAR+0x18,AABAR);
//      Print(L"Status:%r\n",Status);
//      
//      Status=MmioWrite32(FUCK,YOU);
//      Print(L"Status:%r\n",Status);
//      AABAR|=1;
//      Status=MmioWrite8(ABAR+0x18,AABAR);
//      Print(L"Status:%r\n",Status);
//      for (Index=1;Index<105;Index++)
//      {
//       Print(L"%02x ",MmioRead8(PRDT+0x10));
//       if(Index%15==0){
//       Print(L"\n");
//       }
//       
//       PRDT+=0x01;
//       };


//   Status=gBS->LocateProtocol( &gEfiDiskInfoProtocolGuid,
//                              NULL,
//                              (VOID **)&This
//                              );
//   Print(L"Status:%r\n",Status); 
//   Status=This->Identify(This,IdentifyData,&IdentifyDataSize);
//   Print(L"IdentifyData:%r  DataSize:%d\n",*IdentifyData,IdentifyDataSize);   


      

  return EFI_SUCCESS;
}
