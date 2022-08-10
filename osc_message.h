#pragma once
#include <string>

class osc_message
{
protected:
    osc_message(std::string address, char type);
public:
    osc_message(char* data, int size);
};

class osc_float_message : public osc_message
{
public:
    osc_float_message(std::string address, float value);
};

class osc_string_message : public osc_message
{
public:
    osc_string_message(std::string address, std::string value);
};
