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
#include  <Protocol/PciRootBridgeIo.h>

#define EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID \
  { \
    0x2f707ebb, 0x4a1a, 0x11d4, {0x9a, 0x38, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d } \
  }
#define PCI_MAX_BUS     255
#define PCI_MAX_DEVICE  31
#define PCI_MAX_FUNC    7
extern EFI_BOOT_SERVICES *gBS;


typedef struct {
  UINT16  VendorId;
  UINT16  DeviceId;
  UINT16  Command;
  UINT16  Status;
  UINT8   RevisionID;
  UINT8   ClassCode[3];
  UINT8   CacheLineSize;
  UINT8   LatencyTimer;
  UINT8   HeaderType;
  UINT8   BIST;
} PCI_DEVICE_INDEPENDENT_REGION;

typedef struct {
  UINT32  Bar[6];
  UINT32  CISPtr;
  UINT16  SubsystemVendorID;
  UINT16  SubsystemID;
  UINT32  ExpansionRomBar;
  UINT8   CapabilityPtr;
  UINT8   Reserved1[3];
  UINT32  Reserved2;
  UINT8   InterruptLine;
  UINT8   InterruptPin;
  UINT8   MinGnt;
  UINT8   MaxLat;
} PCI_DEVICE_HEADER_TYPE_REGION;

typedef struct {
  PCI_DEVICE_INDEPENDENT_REGION Hdr;
  PCI_DEVICE_HEADER_TYPE_REGION Device;
} PCI_TYPE00;


INTN
EFIAPI
ShellAppMain (
    IN UINTN Argc,
    IN CHAR16 **Argv
    )
{
    EFI_STATUS                          Status;
    UINTN                               NoHandles;
    EFI_HANDLE                          *HandleBuffer=NULL;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *PCIIo;
    PCI_DEVICE_INDEPENDENT_REGION       PciHeader;
    UINT16                              Bus;
    UINT16                              Device;
    UINT16                              Function;
    UINT16                              offset;
    UINT64                              Address;
    UINT8                               data;
    EFI_GUID                            GuidPciRootBridgeIoProtocol=EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID;
//     PCI_TYPE00          Pci;
  

    

    
    NoHandles = 0;
    Status = gBS->LocateHandleBuffer(ByProtocol,
                          &GuidPciRootBridgeIoProtocol,
                          NULL,
                          &NoHandles,
                          &HandleBuffer
                          );
    if (EFI_ERROR(Status)) {
        Print(L"Status:%r\n",Status);
        return 0;
        }

    Status = gBS->HandleProtocol (
                              HandleBuffer[0],
                              &GuidPciRootBridgeIoProtocol,
                              (VOID*)&PCIIo
                              );
    if (EFI_ERROR(Status)) {
        Print(L"Can not get the DEVICE_IO interface\n");
        gBS->FreePool(HandleBuffer);
        return 0;
        }
     

    for(Bus = 0; Bus <= PCI_MAX_BUS; Bus++){
        for(Device = 0; Device <= PCI_MAX_DEVICE; Device++){
            for(Function = 0 ; Function <= PCI_MAX_FUNC; Function++){
            
                offset = 0;
                Address = EFI_PCI_ADDRESS(Bus, Device, Function, offset);

                PCIIo->Pci.Read (
                            PCIIo,
                            EfiPciWidthUint16,
                            Address,
                            1,
                            &PciHeader.VendorId
                            );
                if (PciHeader.VendorId == 0xffff && Function == 0) {
                    break;
                }
                if (PciHeader.VendorId == 0xffff) {
                    continue;
                }

//                 PCIIo->Pci.Read (
//                             PCIIo,
//                             EfiPciWidthUint32,
//                             Address,
//                             sizeof (PciHeader) / sizeof (UINT32),
//                             &Pci
//                             );
                Print(L"Bus is %x, Device is %x, Function is %x\n", Bus, Device, Function);
                Print(L"Vendor ID is %x\n", PciHeader.VendorId);

                for(offset = 0; offset < 256; offset++){

                    Address = EFI_PCI_ADDRESS(Bus, Device, Function, offset);
                    PCIIo->Pci.Read (
                                PCIIo,
                                EfiPciWidthUint8,
                                Address,
                                1,
                                &data
                                );

                        Print(L"%2x  ", data);
                }
                Print(L"\n");
            }
        }
    }
    
    return EFI_SUCCESS;
}


