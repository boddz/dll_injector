/**
 * A Simple DLL Injector.
 * 
 * This is a simple DLL injector designed to inject a specified DLL module into a given process as a thread. Written in
 * C (standard c99), please refer to the `README.md` file in the source directory to see how to use this program.
 * 
 * 
 * Copyright (c) 2024 <github.com/boddz>. All rights reserved.
 * 
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>, or refer to the `LICENSE` file which should also be included
 * in the source directory.
 * 
 */


#include <stdio.h>
#include <stdarg.h>

/* Windows API <https://learn.microsoft.com/en-us/windows/win32/api/>: */
#include <Windows.h>
#include <Psapi.h>
#include <processthreadsapi.h>

#include "autoproc.h" // All headers are included here, but I left the above just for readability.
#include "utils.h"    //


int main(const int argc, const char** argv)
{
    const char* this_fp_raw = argv[0];
    const char* this_file = extract_filename(this_fp_raw, strlen(this_fp_raw));

    if(argc < 3) quick_error(EXIT_FAILURE, "too little args used.\n%s%s%s", HELP_ME_TOO_LITTLE);
    BOOL auto_flag_enabled = FALSE;
    const char* varg_pid_raw = argv[1];
    const char* varg_dll_raw = argv[2];
    if(!strcmp(varg_pid_raw, "auto")) { // Adjust arg pointers if flag auto is used;
        if(argc < 4) quick_error(EXIT_FAILURE, "invalid auto PID args count used.\n%s%s%s", HELP_ME_AUTO_PID);
        auto_flag_enabled = TRUE;
        varg_pid_raw = argv[2];
        varg_dll_raw = argv[3];
    }
    TCHAR abs_dll_path[1024] = {0};
    DWORD abs_dll_path_len = GetFullPathNameA(varg_dll_raw, sizeof(abs_dll_path),
                                              (LPSTR) abs_dll_path, NULL); // Abs path for the DLL module payload.
    if(!abs_dll_path_len)
        quick_windows_fmt_error(EXIT_FAILURE, "%sfailed to get dll module absolute path", "");

    DWORD atoll_result = atoll(varg_pid_raw);
    if(!atoll_result) { // PID 0 is not supported here, and I mean you shouldn't attach to it anyways so cba.
        if(auto_flag_enabled) {
            HANDLE main_auto;
            HANDLE auto_proc_res = get_a_process_by_name(varg_pid_raw, &main_auto);
            if(!auto_proc_res) quick_error(EXIT_FAILURE, "could not auto get a pid for %s", varg_pid_raw);
            atoll_result = GetProcessId(auto_proc_res);
            printf("Found auto PID: %lu\n", atoll_result);
        } else {
            quick_error(EXIT_FAILURE, "the PID should be a non-floating point number, not `%s` (PID of 0 not supported)",
                        varg_pid_raw);
        }
    }

    FILE* fhandle; // Just to check the file path exists, file handle is not directly needed so instantly closed.
    fopen_s(&fhandle, varg_dll_raw, "r");
    if(!fhandle) {
        fclose(fhandle);
        quick_error(EXIT_FAILURE, "could not open file handle for DLL `%s`", varg_dll_raw);
    }

    HANDLE main_phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, atoll_result); // Handle on the main PID process.
    if(!main_phandle) quick_windows_fmt_error(GetLastError(), "failed to open main process (PID: %s)",
                                              varg_pid_raw);

    // Adjust main process' virtual memory to account for the new DLL path.
    LPVOID vmem_alloc_base_address = VirtualAllocEx(main_phandle, NULL, abs_dll_path_len,
                                                  MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if(!vmem_alloc_base_address) quick_windows_fmt_error(GetLastError(),
                                                         "%sfailed to add len for dll path in virtual memory", "");

    // Write the DLL path into the virtual memory.
    if(!WriteProcessMemory(main_phandle, vmem_alloc_base_address, (LPCVOID) abs_dll_path, abs_dll_path_len, NULL))
        quick_windows_fmt_error(GetLastError(), "%sfailed to write dll path to memory", "");

    // Create the thread which will cause the DLL specified in virtual memory to be loaded (executing it).
    HANDLE remote_thread = CreateRemoteThread(main_phandle, NULL, 0, (LPTHREAD_START_ROUTINE) LoadLibraryA,
                                              vmem_alloc_base_address, 0, NULL);

    if(!remote_thread) quick_windows_fmt_error(GetLastError(), "%sfailed to create remote thread to load DLL", "");

    fprintf(stdout, "Injection to main process complete!\n");

    return EXIT_SUCCESS;
}
