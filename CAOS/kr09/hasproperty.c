static unsigned calc_property(unsigned long long value)
{
    value = (value & 0x0f0f0f0f0f0f0f0full) + ((value >> 4) & 0x0f0f0f0f0f0f0f0full);
    value = value + (value >> 8);
    value = value + (value >> 16);
    unsigned value2 = value + (value >> 32);
    value2 = (value2 & 0xf) + ((value2 >> 4) & 0xf);
    value2 = value2 + (value2 >> 4);
    return value2 & 0xf;  
}

int has_property(unsigned long long value)
{
    return calc_property(value) == 2;
}
