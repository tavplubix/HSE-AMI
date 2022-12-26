
int
bitcount(STYPE value) {
    UTYPE uvalue = value;
    int counter = 0;
    do {
        counter += uvalue & 1;
    } while (uvalue >>= 1);
    return counter;
}

