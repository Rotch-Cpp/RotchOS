// ********************************
// gdt.h
// global descriptor table
// Copyright 2022 Rotch
// ********************************
#ifndef _GDT_H_
#define _GDT_H_

#include <types.h>

// class GDT
class GlobalDescriptorTable
{
public:
    // class SD
    // Descript a segment
    class SegmentDescriptor
    {
    private: 
        uint16_t limit_lo;  // 0, 1
        uint16_t base_lo;   // 2, 3
        uint8_t base_mid;   // 4
        uint8_t access;     // 5
        uint8_t limit_hi;   // 6
        uint8_t base_hi;    // 7

    public:
        SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t access);
        uint32_t Base();
        uint32_t Limit();
    } __attribute__((packed));

    SegmentDescriptor nullSegmentSelector;
    SegmentDescriptor unusedSegmentSelector;
    SegmentDescriptor codeSegmentSelector;
    SegmentDescriptor dataSegmentSelector;

public:
    GlobalDescriptorTable();
    ~GlobalDescriptorTable();

    uint16_t CodeSegmentSelector();
    uint16_t DataSegmentSelector();
};

#endif /* _GDT_H_ */