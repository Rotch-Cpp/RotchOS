// ********************************
// gdt.cpp
// Copyright 2022 Rotch
// ********************************

#include <gdt.h>

GlobalDescriptorTable::GlobalDescriptorTable():
    nullSegmentSelector(0, 0, 0), 
    unusedSegmentSelector(0, 0, 0), 
    codeSegmentSelector(0, 4194304, 0x9A), 
    dataSegmentSelector(0, 4194304, 0x92)   // 4194304 = 64 * 1024 * 64
{
    // LGDT
    uint32_t _pgdt[2];
    _pgdt[0] = sizeof(GlobalDescriptorTable) << 16;
    _pgdt[1] = (uint32_t) this;

    asm volatile("lgdt (%0)": :"p" (((uint8_t*) _pgdt) + 2));
}

GlobalDescriptorTable::~GlobalDescriptorTable()
{
}

uint16_t GlobalDescriptorTable::CodeSegmentSelector()
{
    return (uint8_t*) &codeSegmentSelector - (uint8_t*) this;
}

uint16_t GlobalDescriptorTable::DataSegmentSelector()
{
    return (uint8_t*) &dataSegmentSelector - (uint8_t*) this;
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(
    uint32_t base, uint32_t limit, uint8_t ac
)
{
    uint8_t* _target = (uint8_t*) this;

    (limit <= 65536) && (
        _target[6] = 0x40, true
    ) || (
        ((limit & 0xFFF) != 0xFFF) && (
            limit = (limit >> 12) - 1, true
        ) || (
            limit = limit >> 12
        ),
        _target[6] = 0xC0
    );

    // limit low
    _target[0] = limit & 0xFF,
    _target[1] = (limit >> 8) & 0xFF,
    // limit high
    _target[6] |= (limit >> 16) & 0xF,

    // base low
    _target[2] = base & 0xFF,
    _target[3] = (base >> 8) & 0xFF,
    // base mid
    _target[4] = (base >> 16) & 0xFF,
    // base high
    _target[7] = (base >> 24) & 0xFF,

    // access
    _target[5] = ac;
}

// Make base
uint32_t GlobalDescriptorTable::SegmentDescriptor::Base()
{
    uint8_t* _target = (uint8_t*)this;

    uint32_t _result = _target[7];
    _result = (_result << 8) + _target[4],
    _result = (_result << 8) + _target[3],
    _result = (_result << 8) + _target[2];

    return _result;
}

// Make limit
uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit()
{
    uint8_t* _target = (uint8_t*)this;

    uint32_t _result = _target[6] & 0xF;
    _result = (_result << 8) + _target[1],
    _result = (_result << 8) + _target[0];

    ((_target[6] & 0xC0) == 0xC0) && (
        _result = (_result << 12) | 0xFFF
    );

    return _result;
}

