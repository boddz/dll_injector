/**
 * Helpers for automatically grabbing a process ID by an executable name. 
 */

#ifndef _AUTOPROC_H_
#define _AUTOPROC_H_ 1

#include <Windows.h>
#include <Psapi.h>
#include <processthreadsapi.h>

#include "utils.h"


HANDLE get_a_process_by_name(const char* proc_name, HANDLE out_proc_handle);


#endif //_AUTOPROC_H_
