#include <io.h>
#include <string.h>

ConBuffer::ConBuffer(const uint32_t size) :
    pt_in(-1), pt_out(-1)
{
}

ConBuffer::~ConBuffer()
{
}

void ConBuffer::InChar(uint8_t in_ch)
{
    buffer[(++pt_in) % BUFFER_SIZE] = in_ch;
}

uint8_t ConBuffer::OutChar()
{
    if (pt_in == pt_out)    return NULL_CHAR;
    return buffer[(++pt_out) % BUFFER_SIZE];
}

ConBuffer ros_cb;

void SendChar(char ch)
{
    ros_cb.InChar(ch);
}

char GetChar()
{
    return ros_cb.OutChar();
}

static uint8_t ros_output_x = 0, ros_output_y = 0;
void ros_output_str(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;
    static uint32_t temp_val;

    // 80 * 25
    for (int i = 0; str[i] != '\0'; ++i)
    {
        switch(str[i])
        {
            case '\n':
                ros_output_x = 0, ++ros_output_y;
                break;
            default:
                temp_val = CalcPos(ros_output_x, ros_output_y),
                VideoMemory[temp_val] = (VideoMemory[temp_val] & 0xFF00) | str[i],
                ++ros_output_x;
                break;
        }

        if (ros_output_x >= 80)
            ros_output_x = 0, ++ros_output_y;

        if (ros_output_y >= 25)
        {
            for (ros_output_y = 0; ros_output_y < 25; ++ros_output_y)
                for (ros_output_x = 0; ros_output_x < 80; ++ros_output_x)
                    temp_val = CalcPos(ros_output_x, ros_output_y),
                    VideoMemory[temp_val] = (VideoMemory[temp_val] & 0xFF00) | ' ';
            ros_output_x = 0, ros_output_y = 0;
        }
    }
}

void ros_input_str(char str[])
{
    char *ch = "x";
    uint16_t index = 0;
    while ((ch[0] = GetChar()) != '\n')
    {
        if (ch[0] == NULL_CHAR) continue;
        else if (ch[0] == '\b')
        {
            
        }
        str[index++] = ch[0];
        ros_output_str(ch);
        if (index == 1024)  return;
    }

    ros_output_str("\n");
}

int printf(const char *fmt, ...)
{
    int i;
    char buf[1024];
    va_list arg = (va_list)((char*)(&fmt) + 4);
    i = vsprintf(buf, fmt, arg);
    ros_output_str(buf);
    return i;
}
