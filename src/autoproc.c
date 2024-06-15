#include "autoproc.h"
#include "utils.h"


HANDLE get_a_process_by_name(const char* proc_name, HANDLE out_proc_handle)
{
    unsigned int i;
    DWORD bytes_stored_in_array, number_of_procs, proc_array[4096];

    if ( !EnumProcesses( proc_array, sizeof(proc_array), &bytes_stored_in_array ) )
    {
        return NULL;
    }

    number_of_procs = bytes_stored_in_array / sizeof(DWORD);

    for ( i = 0; i < number_of_procs; i++ )
    {
        if( proc_array[i] != 0 )
        {
            char proc_file_path_tmp[1024] = {0};
            HANDLE phandle_tmp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc_array[i]);
            if(!phandle_tmp) continue;
            GetProcessImageFileNameA(phandle_tmp, (char*) &proc_file_path_tmp, sizeof(proc_file_path_tmp));
            memcpy(&proc_file_path_tmp, extract_filename(proc_file_path_tmp, sizeof(proc_file_path_tmp)), sizeof(proc_file_path_tmp));
            if(!strcmp(proc_name, proc_file_path_tmp)) {
                memcpy(&out_proc_handle, &phandle_tmp, sizeof(HANDLE));
                return out_proc_handle;
            };
        }
    }

    return NULL;
}

