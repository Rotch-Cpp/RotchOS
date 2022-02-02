#include <sys/pit.h>

PITDriver::PITDriver(InterruptManager* manager): 
    InterruptHandler(manager, 0x20),
    dataport(0x40),
    commandport(0x43),
    msg(0x20)
{
    count = 0;
    commandport.Write(0x34);
    dataport.Write(0x9c);
    dataport.Write(0x2e);
}

PITDriver::~PITDriver()
{
}

uint32_t PITDriver::HandleInterrupt(uint32_t esp)
{
    msg.Write(0x60);
    ++count;
    return esp;
}

uint32_t PITDriver::Count()
{
    return this->count;
}