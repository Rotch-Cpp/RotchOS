#include <sys/keyboard.h>
#include <io.h>

KeyboardDriver::KeyboardDriver(InterruptManager* manager): 
    InterruptHandler(manager, 0x21),
    dataport(0x60),
    commandport(0x64),
    shiftMod(0),
    altMod(0),
    ctrlMod(0)
{
    while (commandport.Read() & 0x1)    dataport.Read();

    commandport.Write(0xAE);
    commandport.Write(0x20);
    
    uint8_t status = (dataport.Read() | 1) & ~0x10;
    
    commandport.Write(0x60);
    dataport.Write(status);
    dataport.Write(0xF4);

    // Support standard keyboard
    for (int _i = 0; _i < 256; ++_i)    keyTable[_i] = 0;

    keyTable[0x01] = '`',
    keyTable[0x02] = '1',
    keyTable[0x03] = '2',
    keyTable[0x04] = '3',
    keyTable[0x05] = '4',
    keyTable[0x06] = '5',
    keyTable[0x07] = '6',
    keyTable[0x08] = '7',
    keyTable[0x09] = '8',
    keyTable[0x0A] = '9',
    keyTable[0x0B] = '0',

    keyTable[0x10] = 'q',
    keyTable[0x11] = 'w',
    keyTable[0x12] = 'e',
    keyTable[0x13] = 'r',
    keyTable[0x14] = 't',
    keyTable[0x15] = 'y',
    keyTable[0x16] = 'u',
    keyTable[0x17] = 'i',
    keyTable[0x18] = 'o',
    keyTable[0x19] = 'p',

    keyTable[0x1E] = 'a',
    keyTable[0x1F] = 's',
    keyTable[0x20] = 'd',
    keyTable[0x21] = 'f',
    keyTable[0x22] = 'g',
    keyTable[0x23] = 'h',
    keyTable[0x24] = 'j',
    keyTable[0x25] = 'k',
    keyTable[0x26] = 'l',

    keyTable[0x2C] = 'z',
    keyTable[0x2D] = 'x',
    keyTable[0x2E] = 'c',
    keyTable[0x2F] = 'v',
    keyTable[0x30] = 'b',
    keyTable[0x31] = 'n',
    keyTable[0x32] = 'm',
    keyTable[0x33] = ',',
    keyTable[0x34] = '.',
    keyTable[0x35] = '-',

    keyTable[0x1C] = '\n',
    keyTable[0x39] = ' ';

    // shift mode
    keyTable[0x81] = '~',
    keyTable[0x82] = '!',
    keyTable[0x83] = '@',
    keyTable[0x84] = '#',
    keyTable[0x85] = '$',
    keyTable[0x86] = '%',
    keyTable[0x87] = '^',
    keyTable[0x88] = '&',
    keyTable[0x89] = '*',
    keyTable[0x8A] = '(',
    keyTable[0x8B] = ')',

    keyTable[0x90] = 'Q',
    keyTable[0x91] = 'W',
    keyTable[0x92] = 'E',
    keyTable[0x93] = 'R',
    keyTable[0x94] = 'T',
    keyTable[0x95] = 'Y',
    keyTable[0x96] = 'U',
    keyTable[0x97] = 'I',
    keyTable[0x98] = 'O',
    keyTable[0x99] = 'P',

    keyTable[0x9E] = 'A',
    keyTable[0x9F] = 'S',
    keyTable[0xA0] = 'D',
    keyTable[0xA1] = 'F',
    keyTable[0xA2] = 'G',
    keyTable[0xA3] = 'H',
    keyTable[0xA4] = 'J',
    keyTable[0xA5] = 'K',
    keyTable[0xA6] = 'L',

    keyTable[0xAC] = 'Z',
    keyTable[0xAD] = 'X',
    keyTable[0xAE] = 'C',
    keyTable[0xAF] = 'V',
    keyTable[0xB0] = 'B',
    keyTable[0xB1] = 'N',
    keyTable[0xB2] = 'M',
    keyTable[0xB3] = '<',
    keyTable[0xB4] = '>',
    keyTable[0xB5] = '?',

    keyTable[0x0E] = '\b',
    keyTable[0x9C] = '\n',
    keyTable[0xB9] = ' ';
}

KeyboardDriver::~KeyboardDriver()
{
}

// Handle keyboard interrupts
uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataport.Read();
    char* charFmt = "C";
    char* undefinedC = "[INT 0x00]";
    char* hex = "0123456789ABCDEF";
    // KEY_DOWN
    if (key < 0x80)
    {
        if (key == 0x2A)    shiftMod = 1;
        else if (keyTable[key])
        {
            SendChar(keyTable[key + (shiftMod << 7)]);
        }
        else
        {
            //undefinedC[7] = hex[(key >> 4) & 0xF];
            //undefinedC[8] = hex[key & 0xF];
            //ros_output_str(undefinedC);
        }
    }
    // KEY_UP
    else
    {
        switch (key)
        {
            case 0xAA:
            shiftMod = 0;
            break;
        
        default:
            break;
        }
    }

    return esp;
}