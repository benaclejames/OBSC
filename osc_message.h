#pragma once
#include <string>

class osc_message
{

protected:
    osc_message(char* address, char type);

    inline static int quantize(int in, int multiple) {
        return in + multiple - in % multiple;
    }

    inline static void swap_endianness(char* inBytes, int size) {
        for (int i=0, j = size - 1; i < size/2; i++, j--) {
            char temp = inBytes[i];
            inBytes[i] = inBytes[j];
            inBytes[j] = temp;
        }
    }

public:
    osc_message(char* data, int size);
    int writerIndex;
    char message[256];
};

class osc_float_message : public osc_message
{
public:
    osc_float_message(char* address, float value);
};

class osc_string_message : public osc_message
{
public:
    osc_string_message(char* address, char* value);
};

class osc_int_message : public osc_message {
public:
    osc_int_message(char* address, int value);
};

class osc_bool_message : public osc_message {
public:
    osc_bool_message(char* address, bool value) : osc_message(address, value ? 'T' : 'F') {}
};
