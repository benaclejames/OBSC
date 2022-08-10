#include "osc_message.h"

osc_message::osc_message(std::string address, char type)
{
    
}

osc_message::osc_message(char* data, int size)
{
    
}

osc_float_message::osc_float_message(std::string address, float value): osc_message(address, 'f')
{
    
}

osc_string_message::osc_string_message(std::string address, std::string value) : osc_message(address, 's')
{
    
}


