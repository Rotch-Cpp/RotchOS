#ifndef _CMD_H_
#define _CMD_H_

#define VERSION_OS    "Dev 0.00"
#define VERSION_GCC   "11.2.0"
#define VERSION_ENV   "Linux 5.14"
#define VERSION_ENV2  "Ubuntu 21.10"

#include <io.h>
#include <sys/driver.h>

#define DclCmdFunc(NAME) \
    void rosc_##NAME (char *param) {
#define EndCmdFunc  }

DclCmdFunc(help)
    printf("Unsolved!\n");
EndCmdFunc

DclCmdFunc(showinfo)
    printf("Welcome to Rotch Operating System " VERSION_OS "\n");
    printf("GCC Version: " VERSION_GCC "\n");
    printf("Enviroment:  " VERSION_ENV " - " VERSION_ENV2 "\n");
    printf("Copyright (C) 2022 Rotch. All rights reserved.\n\n");
EndCmdFunc

DclCmdFunc(cls)
    printf("Unsolved!\n");
EndCmdFunc

DclCmdFunc(shutdown)
    printf("Unsolved!\n");
EndCmdFunc

DclCmdFunc(runcount)
    printf("Total run time: %d.%d s\n", _pit->Count() / 100, _pit->Count() % 100);
EndCmdFunc

#endif  /* _CMD_H_ */
