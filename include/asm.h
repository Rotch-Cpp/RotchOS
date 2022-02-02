// ********************************
// asm.h
// Define some functions with ASM
// Copyright 2022 Rotch
// ********************************
#ifndef _ASM_H_
#define _ASM_H_

#define sti()   asm("sti"::)
#define sti_hlt()   asm("sti\nhlt"::);
#define cli()   asm("cli"::)
#define nop()   asm("nop"::)

#endif  /* _ASM_H_ */