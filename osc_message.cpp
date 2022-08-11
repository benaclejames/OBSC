#include "osc_message.h"

osc_message::osc_message(char* address, char type) : address(address), type(type)
{
    int len = strlen(address)+1;
    writerIndex = quantize(len, 4);
    memcpy(message, address, writerIndex);

    message[writerIndex++] = ',';
    message[writerIndex++] = type;
    message[writerIndex++] = '\0';
    message[writerIndex++] = '\0';
}

osc_message::osc_message(char* data, int size)
{
    // Read the address string
    int len = strlen(data)+1;
    address = new char[len];
    memcpy(address, data, len);

    // Offset the writerIndex by the address length quantized to 4 bytes
    writerIndex = quantize(len, 4);

    // Ensure the next 2 bytes are a comma and the type
    if (data[writerIndex++] != ',') {
        throw "Invalid OSC message";
    }

    type = data[writerIndex++];
    writerIndex += 2;
}

osc_float_message::osc_float_message(char* address, float value): osc_message(address, 'f')
{
    char* bytes = static_cast<char*>(static_cast<void*>(&value));
    swap_endianness(bytes, sizeof value);
    memcpy(&message[writerIndex], bytes, sizeof value);
    writerIndex += sizeof value;
}

osc_string_message::osc_string_message(char* address, char* value) : osc_message(address, 's')
{
    int len = strlen(value)+1;
    int quantizedLength = quantize(len, 4);
    memcpy(&message[writerIndex], value, quantizedLength);
    writerIndex += quantizedLength;
}


osc_int_message::osc_int_message(char* address, int value) : osc_message(address, 'i') {
    char* bytes = static_cast<char*>(static_cast<void*>(&value));
    swap_endianness(bytes, sizeof value);
    memcpy(&message[writerIndex], bytes, sizeof value);
    writerIndex += sizeof value;
}
