// ********************************
// kernel.cpp
// source of Rotch OS kernel
// Copyright 2022 Rotch
// ********************************

#include <types.h>
#include <io.h>
#include <time.h>
#include <gdt.h>
#include <sys/driver.h>
#include <string.h>
#include <asm.h>
#include <rotch/cmd.h>

// call constructors
// * it's called before `kernelMain` to initialize
typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for (constructor* i = &start_ctors; i != &end_ctors; ++i)
        (*i)();
}

void CmdHandler();
typedef void (*HandleFunc)(char*);
typedef struct _cmd_list
{
    char cmd[16];
    HandleFunc hf;
} CmdDictionary;
CmdDictionary cd[16];
int8_t NR_CMD = -1;
bool handled = 0;

#define MakeCmd(CMD, FUNC) \
    strcpy(cd[++NR_CMD].cmd, CMD); \
    cd[NR_CMD].hf = rosc_##FUNC;

void init_cmd()
{
    MakeCmd("help",      help);
    MakeCmd("info",      showinfo);
    MakeCmd("shutdown",  shutdown);
    MakeCmd("cls",       cls);
    MakeCmd("runcount",  runcount);
}

// main function of the kernel
extern "C" void kernelMain(const void* multiboot_structure, uint32_t argv)
{
    printf("Welcome to Rotch Operating System " VERSION_OS "\n");
    printf("Copyright (C) 2022 Rotch. All rights reserved.\n\n");

    // init
    GlobalDescriptorTable gdt;
    InterruptManager interrupts(0x20, &gdt);
    KeyboardDriver keyboard(&interrupts);
    MouseDriver mouse(&interrupts);
    PITDriver pit(&interrupts);
    _gdt = &gdt;
    _interrupts = &interrupts;
    _keyboard = &keyboard;
    _mouse = _mouse;
    _pit = &pit;
    init_time();
    init_cmd();

    interrupts.Activate();
    
    CmdHandler();
}

char input[1024];

void CmdHandler()
{
    int cmd_len;
    for (;;)
    {
        printf("Rotch OS > ");
        scanf(input);
        cmd_len = strlen(input);
        handled = false;
        // to lower case
        for (int _i = 0; _i < cmd_len; ++_i)    input[_i] |= 32;

        // Handle cmd
        for (int8_t _i = 0; _i <= NR_CMD; ++_i)
            if (strcmp(input, cd[_i].cmd) == 0)
            {
                cd[_i].hf("");
                handled = true;
                break;
            }
            
        
        // Unhandled
        (!handled) && (printf("Command `%s` is undefined!\n", input));

        // clear
        for (int _i = 0; _i < 1024; ++_i)   input[_i] = 0;
    }
}