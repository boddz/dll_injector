/**
 * Error handling/ small misc helper functions to be used internally.
 */


#ifndef _UTILS_H_
#define _UTILS_H_ 1


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>


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
void manual_error(const int exit_code, const char* prefix, const char* fmt, ...);


/**
 * Retrieve the file name from at the end of a file path.
 */
const char* extract_filename(const char* fp, const size_t fp_len);



#endif
