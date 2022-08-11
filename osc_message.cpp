#include <util/base.h>
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
    memcpy(message, data, size);

    // Read the address string
    int len = strlen(message)+1;
    address = new char[len];
    memcpy(address, message, len);
    // Offset the writerIndex by the address length quantized to 4 bytes
    writerIndex = quantize(len, 4);
    // Ensure the next 2 bytes are a comma and the type
    if (message[writerIndex++] != ',') {
        blog(LOG_ERROR, "[OBSC] Expected comma after address");
    }

    type = message[writerIndex++];
    writerIndex += 2; // Skip the two null bytes
}

osc_float_message::osc_float_message(char* address, float value): osc_message(address, 'f')
{
    char* bytes = static_cast<char*>(static_cast<void*>(&value));
    swap_endianness(bytes, sizeof value);
    memcpy(&message[writerIndex], bytes, sizeof value);
    writerIndex += sizeof value;
}

float osc_float_message::parse() {
    float value;
    memcpy(&value, &message[writerIndex], sizeof value);
    swap_endianness((char*)&value, sizeof value);
    return value;
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

int osc_int_message::parse() {
    int value;
    memcpy(&value, &message[writerIndex], sizeof value);
    swap_endianness((char*)&value, sizeof value);
    return value;
}
