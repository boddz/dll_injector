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


#define HELP_ME_VA_ARGS "usage:\n\t", this_file, " <pid> <dll_file>"


/**
 * Combines the str result of `GetLastError()` with the internal `quick_error()` macro for actually good output on
 * process handling errors and stuff.
 */
#define quick_windows_fmt_error(last_err_code, fmt, ...) { \
        const DWORD last_err = GetLastError();  \
        char fmt_msg_buffer[1024] = {0}, join_fmt_msg[1024] = {0}; \
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, last_err, LANG_SYSTEM_DEFAULT, (char*) &fmt_msg_buffer, \
                      sizeof(fmt_msg_buffer), NULL); \
        sprintf_s((char*) &join_fmt_msg, sizeof(join_fmt_msg), fmt, __VA_ARGS__); \
        fmt_msg_buffer[0] = tolower(fmt_msg_buffer[0]); \
        quick_error(EXIT_FAILURE, "%s: %s\033[1A", join_fmt_msg, fmt_msg_buffer); \
}


#define quick_error(code, fmt, ...) { \
    char tmp_prefix_buffer[1024] = {0}; \
    sprintf_s((char*) &tmp_prefix_buffer, sizeof(tmp_prefix_buffer), "%s: fatal error: ", this_file); \
    manual_error(EXIT_FAILURE, tmp_prefix_buffer, fmt, __VA_ARGS__); \
}


/**
 * Exit using `exit_code` after printing a prefixed error with standard printf style va_args formatting.
 */
static void manual_error(const int exit_code, const char* prefix, const char* fmt, ...)
{
    char va_str_buffer[1024] = {0};
    sprintf_s((char*) &va_str_buffer, sizeof(va_str_buffer), "%s", prefix);
    fprintf(stderr, "%s", va_str_buffer);

    va_list args_list;
    va_start(args_list, fmt);
    vsprintf_s((char*) &va_str_buffer, sizeof(va_str_buffer), fmt, args_list);
    fprintf(stderr, "%s\n", va_str_buffer);

    exit(exit_code);
}


/**
 * Retrieve the file name from at the end of a file path.
 */
static const char* extract_filename(const char* fp, const size_t fp_len)
{
    size_t index_rev = fp_len;
    for(; index_rev > 0; --index_rev) if(fp[index_rev] == '\\' || fp[index_rev] == '/' /* Just incase */) break;
    return &fp[index_rev] + 1;
}


int main(const int argc, const char** argv)
{
    const char* this_fp_raw = argv[0];
    const char* this_file = extract_filename(this_fp_raw, strlen(this_fp_raw));

    if(argc != 3) quick_error(EXIT_FAILURE, "too little args used.\n%s%s%s", HELP_ME_VA_ARGS);
    const char* varg_pid_raw = argv[1];
    const char* varg_dll_raw = argv[2];
    TCHAR abs_dll_path[1024] = {0};
    DWORD abs_dll_path_len = GetFullPathNameA(varg_dll_raw, sizeof(abs_dll_path),
                                              (LPSTR) abs_dll_path, NULL); // Abs path for the DLL module payload.
    if(!abs_dll_path_len)
        quick_windows_fmt_error(EXIT_FAILURE, "%sfailed to get dll module absolute path", "");

    const DWORD atoll_result = atoll(varg_pid_raw);
    if(!atoll_result && varg_pid_raw[0] != '0')
        quick_error(EXIT_FAILURE, "the PID should be a non-floating point number, not `%s`", varg_pid_raw);

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

    return EXIT_SUCCESS;
}
