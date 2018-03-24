//
//  anti.c
//  DVIA-v2
//
//  Created by Prateek Gianchandani on 3/21/18.
//  Copyright © 2018 HighAltitudeHacks. All rights reserved.
//

#include "anti.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#import <dlfcn.h>
#import <sys/types.h>
#import <sys/stat.h>
#import <mach-o/dyld.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#import <mach/mach.h>
#import <mach/mach_host.h>
#include <libgen.h>
#include <mach/processor_info.h>

typedef int (*ptrace_ptr_t)(int _request, pid_t _pid, caddr_t _addr, int _data);
#if !defined(PT_DENY_ATTACH)
#define PT_DENY_ATTACH 31
#endif  // !defined(PT_DENY_ATTACH)
processor_info_array_t cpuInfo, prevCpuInfo;
mach_msg_type_number_t numCpuInfo, numPrevCpuInfo;
unsigned numCPUs;

void disable_gdb()
{
    void* handle = dlopen(0, RTLD_GLOBAL | RTLD_NOW);
    ptrace_ptr_t ptrace_ptr = dlsym(handle, "ptrace");
    ptrace_ptr(PT_DENY_ATTACH, 0, 0, 0);
    dlclose(handle);
}

int detect_injected_dylds()
{
    //Get count of all currently loaded DYLDs
    uint32_t count = _dyld_image_count();
    for(uint32_t i = 0; i < count; i++)
    {
        //Get the name along with the full path of all the injeced DYLDs
        const char *dyld = _dyld_get_image_name(i);
       // printf("LibraryName:%s\n",basename(dyld));
        //Cycript for RuntimeManipulation, MobileSubstrate for any form of injection, and SSLKillSwitch2 if someone tries to bypas SSLPinning, additionally these stings can be obfuscated in the code as well
        if(!strstr(dyld, "MobileSubstrate") || !strstr(dyld, "cycript") || !strstr(dyld, "SSLKillSwitch") || !strstr(dyld, "SSLKillSwitch2")) {
            continue;
        }
        else {
            //Suspicious injected library found, exit the program
            exit(0);
        }
    }
    return 0;
}

