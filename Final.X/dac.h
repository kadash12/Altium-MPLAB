/*
 * Header for dac functions.
 */
#ifndef DAC_H
#define	DAC_H

//Sine Wave
char sin[50] = {
    0x80,0x8f,0x9f,0xae,0xbd,0xca,0xd7,0xe2,0xeb,0xf3,
    0xf9,0xfd,0xff,0xff,0xfd,0xf9,0xf3,0xeb,0xe2,0xd7,
    0xca,0xbd,0xae,0x9f,0x8f,0x80,0x70,0x60,0x51,0x42,
    0x35,0x28,0x1d,0x14,0xc,0x6,0x2,0x0,0x0,0x2,
    0x6,0xc,0x14,0x1d,0x28,0x35,0x42,0x51,0x60,0x70
};
//Triangle Wave
char triangle[50] = {
    0xa,0x14,0x1f,0x29,0x33,0x3d,0x47,0x52,0x5c,0x66,
    0x70,0x7a,0x85,0x8f,0x99,0xa3,0xad,0xb8,0xc2,0xcc,
    0xd6,0xe0,0xeb,0xf5,0xff,0xf5,0xeb,0xe0,0xd6,0xcc,
    0xc2,0xb8,0xad,0xa3,0x99,0x8f,0x85,0x7a,0x70,0x66,
    0x5c,0x52,0x47,0x3d,0x33,0x29,0x1f,0x14,0xa,0x0
};
//Sawtooth Wave
char sawtooth[50] = {
    0x5,0xa,0xf,0x14,0x1a,0x1f,0x24,0x29,0x2e,0x33,
    0x38,0x3d,0x42,0x47,0x4d,0x52,0x57,0x5c,0x61,0x66,
    0x6b,0x70,0x75,0x7a,0x80,0x85,0x8a,0x8f,0x94,0x99,
    0x9e,0xa3,0xa8,0xad,0xb3,0xb8,0xbd,0xc2,0xc7,0xcc,
    0xd1,0xd6,0xdb,0xe0,0xe6,0xeb,0xf0,0xf5,0xfa,0xff
};

void spi_init();
void spi_write(unsigned char data);

#endif	/* DAC_H */

