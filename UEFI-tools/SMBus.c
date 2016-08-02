#include  <protocol/SmbusHc.h>
#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>
#include  <IndustryStandard/SmBus.h>
#include  <Library/IoLib.h>




#define EFI_SMBUS_HC_PROTOCOL_GUID \
  {0xe49d33ed, 0x513d, 0x4634, { 0xb6, 0x98, 0x6f, 0x55, 0xaa, 0x75, 0x1c, 0x1b} }

  
 

extern EFI_BOOT_SERVICES *gBS;

EFI_GUID EfiSmbusHcProtocolGuid=EFI_SMBUS_HC_PROTOCOL_GUID;

/*
INTN 
SMBusReady()
{

     UINT8 m,n,s=0,i;

     for(i=0;i<0x80;i++)
     {
                              
       m=IoRead8(0xF000);
    

       

       n=m;   

       m=m&0x02;                // BIT1:INTR is 1, ready 
       if(m!=0)
       goto ret;

       m=n;    

       m=m&0xbf;                // Bit6:INUSE_STS is 0, ready 
       if(m==0)
       goto ret;

       m=n;    

       m=m&0x04;                // Bit2EV_ERR not 0, error 
       if(m)
       break;
      }

      s=1;

ret:
   return s;
} 
*/

INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{ 
  
  EFI_STATUS Status;
  EFI_HANDLE *SmbusHandle;
  UINTN HandleCount,HandleIndex;
  STATIC EFI_SMBUS_HC_PROTOCOL *Smbus;
  EFI_SMBUS_DEVICE_ADDRESS SlaveAddress;
  EFI_SMBUS_OPERATION Operation=EfiSmbusReadBlock;
  UINTN  Count;
  UINT8  Buffer7=0;          
  UINTN  Length=8;
  SlaveAddress.SmbusDeviceAddress=0xD3;
  
  
  
  Status=IoWrite8(0xF000,0xFE);
  Status=IoWrite8(0xF000+0x04,0xD3);
  Status=IoWrite8(0xF000+0x03,0x00);
//   Status=IoWrite8(0xF000+0x0D,0x02);
//   Print (L"Status: %r\n", Status);
  Status=IoWrite8(0xF000+0x02,0x54);
  Status=IoRead8(0xF000+0x05);
  Print (L"Status: %r\n", Status);
  
  for(Count=8;Count>1;Count--)
  {
  Status=IoRead8(0xF000+0x07);
  Print (L"Status: %x\n", Status);
  Status=IoWrite8(0xF000,0xFE);

  }

//  CpuDeadLoop (); 
  
  Status=gBS->LocateHandleBuffer(
        ByProtocol,
				&EfiSmbusHcProtocolGuid, 
				NULL,
        &HandleCount,
				&SmbusHandle);
   
   Print (L"Status: %r\n", Status);
   
   for (HandleIndex =0;HandleIndex < HandleCount; HandleIndex++)
   {
       Status = gBS->HandleProtocol(
                   SmbusHandle[HandleIndex],
                   &EfiSmbusHcProtocolGuid,
                   (VOID**)&Smbus);
                   
                  Print (L"Status: %r\n", Status);
   }     
    
        
        

   
   
//    
//    if(!SMBusReady())
//     Print(L"Device error\n");
//     else
//   {
   
   
   Status=IoWrite8(0xF000,0xFE);
   Status=Smbus->Execute(Smbus,SlaveAddress,0x00,Operation,FALSE,&Length,&Buffer7);  
   
   Print(L"We read %x\n",Buffer7);
   Print (L"Status: %r\n", Status);
//    }
   
  
  return 0;
}
