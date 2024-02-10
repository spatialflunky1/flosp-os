void memcpy(unsigned char* source, unsigned char* dest, unsigned int nbytes) {
    for (int i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}
