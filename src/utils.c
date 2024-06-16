#include "utils.h"


/**
 * Exit using `exit_code` after printing a prefixed error with standard printf style va_args formatting.
 */
void manual_error(const int exit_code, const char* prefix, const char* fmt, ...)
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
const char* extract_filename(const char* fp, const size_t fp_len)
{
    size_t index_rev = fp_len;
    for(; index_rev > 0; --index_rev) if(fp[index_rev] == '\\' || fp[index_rev] == '/' /* Just incase */) break;
    return &fp[index_rev] + 1;
}


/**
 * Check if a file ext matches a comparison case. 
 */
bool same_file_ext(const char* ext, const char* compare)
{
    int ext_len = strlen(ext);
    char* ext_starts = (char*) ext;
    for(int reverse_i = ext_len; reverse_i > 0; --reverse_i) {
        if(ext[reverse_i] == '.') ext_starts = (char*) &ext[reverse_i];
    }
    return strcmp(ext_starts, compare) ? false : true;
}
