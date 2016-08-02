
#include <IndustryStandard/SmBios.h>       // from EDK2. GNU_EFI libsmbios.h is defective
#include <Library/UefiLib.h>
#include <Library/ShellCEntryLib.h>
#include <protocol/SmBios.h>
#include <Library/MemoryAllocationLib.h>


#define EFI_SMBIOS_PROTOCOL_GUID \
    { 0x3583ff6, 0xcb36, 0x4940, { 0x94, 0x7e, 0xb9, 0xb3, 0x9f, 0x4a, 0xfa, 0xf7 }}  
  
 
EFI_GUID  SMBIOSProtocolGuid = EFI_SMBIOS_PROTOCOL_GUID;

extern EFI_SYSTEM_TABLE			 *gST;
extern EFI_BOOT_SERVICES     *gBS;
extern EFI_HANDLE             gImageHandle;




typedef struct {
 UINTN     datasize;
 EFI_STATUS         Status;
 UINT8     *FruDataByte;             
 UINTN     FruDataSize;
 UINT8     FruDataSizeLS;
 UINT8     FruDataSizeMS;

 //---------------COMMON HEADER Format-------------------------------
 UINT8     Common_Version;
 UINT8     Common_Internal_Offset;
 UINT8     Common_Chassis_Offset;
 UINT8     Common_Board_Offset;
 UINT8     Common_Product_Offset;
 UINT8     Common_MultiRecord_Offset;
 UINT8     Common_PAD;
 UINT8     Common_Checksum;

 //--------------SMBIOS Type 0 --------------------------------------
     UINT8     Type0_BiosVendorSize;
     UINT8     Type0_BiosVersionSize;
     UINT8     Type0_BiosReleaseDateSize;
     UINT8     *Type0_BiosVendor;
     UINT8     *Type0_BiosVersion;
     UINT8     *Type0_BiosReleaseDate;

 //--------------SMBIOS Type 1 --------------------------------------
     UINT8     Type1_ManufacturerSize;
     UINT8     Type1_ProductNameSize;
     UINT8     Type1_SKUNumberSize;
     UINT8     Type1_VersionSize;
     UINT8     Type1_Serial_NumberSize; //Phoen_WIL0818
     UINT8      Type1_UuidSize;
     CHAR8     *Type1_Manufacturer ;   //Product Manufacturer
      UINT8     *Type1_ProductName;   //Product Product Name
      UINT8     *Type1_SKUNumber;   //Product Product Number
      UINT8      *Type1_Version;    //Product Version
      UINT8      *Type1_Serial_Number;   //Product Serial Number
      UINT8       *Type1_UUID; //Phoen_WIL0818
//        EFI_GUID        Type1_UUID;

 //--------------SMBIOS Type 2 --------------------------------------
     UINT8                       Type2_ManufacturerSize;
     UINT8                       Type2_ProductNameSize;
     UINT8                       Type2_VersionSize;
     UINT8                       Type2_SerialNumberSize;
     UINT8                       Type2_AssetTagSize;
     UINT8                       Type2_LocationSize;
     UINT8                       *Type2_Manufacturer;   //Board Manufacturer
     UINT8                       *Type2_ProductName;   //Board Product Name
     UINT8                       *Type2_Version;
     UINT8                       *Type2_SerialNumber;   //Board Serial Number
     UINT8                       *Type2_AssetTag;   //Board Part Number
     UINT8                       *Type2_Location;

 //--------------SMBIOS Type 3 --------------------------------------
     UINT8                       Type3_ManufacturerSize;
     UINT8                       Type3_VersionSize;
     UINT8                       Type3_SerialNumberSize;
     UINT8                       Type3_AssetTagSize;
     UINT8                       *Type3_Manufacturer;
     UINT8                       *Type3_Version;
     UINT8                       *Type3_SerialNumber;   //Chassis Serial Number
     UINT8                       *Type3_AssetTag;   //Chassis Part Number

 //--------------SMBIOS Type 10 -------------------------------------
     UINT8                       Type10_DescriptionSize;
     UINT8                       *Type10_Description;

 //-----------------------------------------------------------------
//Phoen_WIL0814 Start

//  FUNC_0x52_DATA_BUFFER*  UpdateData;
 UINTN     UpdateDataSize;

}OEM_SMBIOSAP_DATA;



 
EFI_STATUS
UefiMain (
IN EFI_HANDLE image, 
IN EFI_SYSTEM_TABLE *systab
)
{
                     
    EFI_STATUS Status;
    UINTN               mType1ManufacturerNumber=1 ;
    EFI_SMBIOS_HANDLE  Handle=0xFFFE;
    EFI_HANDLE ProducerHandle;
    OEM_SMBIOSAP_DATA *oemsmbiosap=NULL;
//     UINT8 *K ="Holy\n";
   
    
     EFI_SMBIOS_TABLE_HEADER *Record; 
     EFI_SMBIOS_PROTOCOL  *mSmbios =NULL ;
     oemsmbiosap->Type1_Manufacturer="Hello";      //  Dereferencing null pointers??
     
 

    Status=gBS->InstallProtocolInterface(
    &gImageHandle,
    &SMBIOSProtocolGuid,
    EFI_NATIVE_INTERFACE,
    mSmbios
    );
    Print (L"status :%r\n",Status) ;
     
    
    Status=gBS->LocateProtocol(
		&SMBIOSProtocolGuid, 
		NULL,
		(VOID **)&mSmbios);
    

    Print (L"status :%r\n",Status) ;
   

         
    Status=mSmbios->GetNext (
                       mSmbios,
                       &Handle,
                       NULL,
                       &Record,
                       &ProducerHandle
                       );
                       
     
     Print(L"Status:%r\n SmbiosHandle:%d\n Three value %d %d %d \n ProducerHandle:%d\n",Status,Handle,Record->Type,Record->Length,Record->Handle,ProducerHandle);
     
     
        
         
      Status=mSmbios->UpdateString (
      mSmbios, 
      &Handle, 
      &mType1ManufacturerNumber,
      oemsmbiosap->Type1_Manufacturer
      );
      
      if(EFI_ERROR(Status)){
      
      Print(L"status:%r",Status);
      return Status;
      }

         
    return Status;
}


