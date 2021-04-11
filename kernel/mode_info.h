#ifndef mode_info_h
#define mode_info_h

typedef struct mode_info_block {
    unsigned short attributes;
    unsigned char windowA, windowB;
    unsigned short granularity;
    unsigned short windowSize;
    unsigned short segmentA, segmentB;
    unsigned int winFuncPtr;
    unsigned short pitch;

    unsigned short resolutionX, resolutionY;
    unsigned char wChar, yChar, planes, bpp, banks;
    unsigned char memoryModel, bankSize, imagePages;
    unsigned char reserved0;

    unsigned char readMask, redPosition;
    unsigned char greenMask, greenPosition;
    unsigned char blueMask, bluePosition;
    unsigned char reservedMask, reservedPosition;
    unsigned char directColorAttributes;

    // linear frame buffer
    unsigned int buffer;
    unsigned int offScreenMemOff;
    unsigned short offScreenMemSize;
    unsigned char  reserved1 [206];
} mode_info_t;

#endif
