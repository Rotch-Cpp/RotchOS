#include <sys/mouse.h>

MouseDriver::MouseDriver(InterruptManager* manager): 
    InterruptHandler(manager, 0x2C),
    dataport(0x60),
    commandport(0x64)
{
    uint16_t* VideoMemory = (uint16_t*)0xb8000;
    offset = 0;
    buttons = 0;
    x = 40;
    y = 12;

    uint16_t pos = CalcPos(x, y);
    VideoMemory[pos] = (VideoMemory[pos] & 0x0F00) << 4
                        | (VideoMemory[pos] & 0xF000) >> 4
                        | (VideoMemory[pos] & 0x00FF);

    while (inb (0x64) & 0x01) { inb (0x60); }
    commandport.Write(0xA8);
    commandport.Write(0x20);
    uint8_t status = dataport.Read() | 2;

    commandport.Write(0x60);
    dataport.Write(status);

    commandport.Write(0xD4);
    dataport.Write(0xF4);
    dataport.Read();
}

MouseDriver::~MouseDriver()
{
}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t status = commandport.Read();
    if (!(status & 0x20))   return esp;

    buffer[offset] = dataport.Read();
    offset = (offset + 1) % 3;
    uint16_t pos = CalcPos(x, y);
    int8_t tmp;

    if (offset == 0)
    {
        if (buffer[1] != 0 || buffer[2] != 0)
        {
            static uint16_t* VideoMemory = (uint16_t*)0xb8000;
            VideoMemory[pos] = (VideoMemory[pos] & 0x0F00) << 4
                             | (VideoMemory[pos] & 0xF000) >> 4
                             | (VideoMemory[pos] & 0x00FF);

            offset_x += buffer[1];
            (offset_x >= 5 || offset_x <= -5) && (
                tmp = offset_x / 5, 
                x += tmp, 
                offset_x -= tmp * 5
            );
            (x >= 80) && (x = 79);
            (x < 0) && (x = 0);

            
            offset_y -= buffer[2];
            (offset_y >= 5 || offset_y <= -5) && (
                tmp = offset_y / 5, 
                y += tmp, 
                offset_y -= tmp * 5
            );
            (y >= 25) && (y = 24);
            (y < 0) && (y = 0);
            pos = CalcPos(x, y);

            VideoMemory[pos] = (VideoMemory[pos] & 0x0F00) << 4
                             | (VideoMemory[pos] & 0xF000) >> 4
                             | (VideoMemory[pos] & 0x00FF);
        }
    }
    return esp;
}