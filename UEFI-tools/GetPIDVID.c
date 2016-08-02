#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>
#include  <Protocol/Usb2HostController.h>
#include  <Protocol/UsbIo.h>

extern EFI_BOOT_SERVICES         *gBS;

EFI_GUID  gEfiUsbIoProtocolGuid   = 
	{ 0x2B2F68D6, 0x0CD2, 0x44CF, 
		{ 0x8E, 0x8B, 0xBB, 0xA2, 0x0B, 0x1B, 0x5B, 0x75 }};

EFI_GUID  gEfiUsb2HcProtocolGuid =
{ \
    0x3e745226, 0x9818, 0x45b6, {0xa2, 0xac, 0xd7, 0xcd, 0xe, 0x8b, 0xa2, 0xbc } \
  };

  
UINTN GetUSB( )
{
  EFI_STATUS  Status;
  UINTN       HandleIndex, HandleCount;
  EFI_HANDLE  *DevicePathHandleBuffer = NULL;
  EFI_USB_IO_PROTOCOL 			*USBIO;
  EFI_USB_DEVICE_DESCRIPTOR     DeviceDescriptor;
  
  Status = gBS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiUsbIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &DevicePathHandleBuffer);
  
  if (EFI_ERROR(Status)) 
  {
    Print(L"ERROR : Get USBIO count fail.\n");
    return 0;
  }   

  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) 
  { 
     Status = gBS->HandleProtocol(
                      DevicePathHandleBuffer[HandleIndex],
                      &gEfiUsbIoProtocolGuid,
                      (VOID**)&USBIO);
         
      if (EFI_ERROR(Status))
      {
        Print(L"ERROR : Open USBIO fail.\n");
        gBS->FreePool(DevicePathHandleBuffer);  
        return 0;
      }

      Status = USBIO->UsbGetDeviceDescriptor(USBIO, &DeviceDescriptor);     
      if (EFI_ERROR(Status))
      {
        Print(L"ERROR : Usb Get Device Descriptor fail.\n");
        gBS->FreePool(DevicePathHandleBuffer);  
        return 0;
      }
      
      Print(L"VendorID = %04X, ProductID = %04X\n", 
                              DeviceDescriptor.IdVendor, 
                              DeviceDescriptor.IdProduct);      
 
  }
  gBS->FreePool(DevicePathHandleBuffer);       
  return HandleCount;
}

UINTN GetSpeed()
{
  EFI_STATUS  Status;
  UINTN       HandleIndex, HandleCount;
  EFI_HANDLE  *DevicePathHandleBuffer = NULL;
  EFI_USB2_HC_PROTOCOL 			*USB2HC;
  UINT8 MaxSpeed =0;
  UINT8 PortNumber =0;
  UINT8 Is64BitCapable =0;
 
  
  Status = gBS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiUsb2HcProtocolGuid,
                  NULL,
                  &HandleCount,
                  &DevicePathHandleBuffer);
  
  if (EFI_ERROR(Status)) 
  {
    Print(L"ERROR : Get USB2HC count fail.\n");
    return 0;
  }   

  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) 
  { 
     Status = gBS->HandleProtocol(
                      DevicePathHandleBuffer[HandleIndex],
                      &gEfiUsb2HcProtocolGuid,
                      (VOID**)&USB2HC);
         
      if (EFI_ERROR(Status))
      {
        Print(L"ERROR : Open USBIO fail.\n");
        gBS->FreePool(DevicePathHandleBuffer);  
        return 0;
      }

      
      Status = USB2HC->GetCapability(USB2HC, &MaxSpeed, &PortNumber, &Is64BitCapable);     
      if (EFI_ERROR(Status))
      {
        Print(L"ERROR : Get USB2HC Capability fail.\n");
        gBS->FreePool(DevicePathHandleBuffer);  
        return 0;
      }
      
      
          
      Print(L"MaxSpeed=%d\n  Handle=%X\n ", MaxSpeed,DevicePathHandleBuffer[HandleIndex]);      
 
  }
  gBS->FreePool(DevicePathHandleBuffer);       
  return HandleCount;
}



int
EFIAPI
main (
  IN int Argc,
  IN CHAR16 **Argv
  )
{
GetSpeed();
//   GetUSB( );
  return EFI_SUCCESS;
}

