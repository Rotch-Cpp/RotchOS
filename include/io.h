// ********************************
// io.h
// Rotch OS IO system
// Copyright 2022 Rotch
// ********************************
#ifndef _IO_H_
#define _IO_H_

#include <types.h>
#include <stdarg.h>

#define CalcPos(x, y)   ((y << 6) + (y << 4) + x)

extern void ros_output_str(char* str);
extern void ros_input_str(char str[]);
int vsprintf(char *buf, const char *fmt, va_list args);
extern void SendChar(char inch);
extern char GetChar();

// IO API
extern int printf(const char *fmt, ...);
#define scanf	ros_input_str

#define BUFFER_SIZE 1024
#define NULL_CHAR   0

class ConBuffer
{
public:
    ConBuffer(const uint32_t size = BUFFER_SIZE);
    ~ConBuffer();
public:
    void InChar(uint8_t in_ch);
    uint8_t OutChar();
private:
    volatile uint32_t pt_in;
    volatile uint32_t pt_out;
    char buffer[BUFFER_SIZE];
};

extern ConBuffer ros_cb;

#endif /* _IO_H_ */