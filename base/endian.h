#ifndef BASE_ENDIAN_ENDIAN_H
#define BASE_ENDIAN_ENDIAN_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

typedef enum {
	LITTLE_ENDIAN1,
	BIG_ENDIAN1,	   
} EUNM_ENDIAN1;

uint16_t u8_u16(uint8_t *data, EUNM_ENDIAN1 endian);
uint32_t u8_u32(uint8_t *data, EUNM_ENDIAN1 endian);

#endif