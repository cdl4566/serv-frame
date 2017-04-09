#include "endian.h"

uint16_t u8_u16(uint8_t *data, EUNM_ENDIAN1 endian) {
	uint16_t u16_data = 0;

	if (endian == LITTLE_ENDIAN1) {
		u16_data = *(data+1);
		u16_data <<= 8;
		u16_data |= *(data+0);	
	} else if (endian == BIG_ENDIAN1) {
		u16_data = *(data+0);
		u16_data <<= 8;
		u16_data |= *(data+1);	
	}	
	return (uint16_t)(u16_data);	
}


uint32_t u8_u32(uint8_t *data, EUNM_ENDIAN1 endian) {
	uint32_t u32_data = 0;

	if (endian == LITTLE_ENDIAN1) {
		u32_data = *(data+3);
		u32_data <<= 8;
		u32_data |= *(data+2);
		u32_data <<= 8;
		u32_data |= *(data+1);
		u32_data <<= 8;
		u32_data |= *(data+0);		
	} else if (endian == BIG_ENDIAN1) {
		u32_data = *(data+0);
		u32_data <<= 8;
		u32_data |= *(data+1);
		u32_data <<= 8;
		u32_data |= *(data+2);
		u32_data <<= 8;
		u32_data |= *(data+3);	
	}	

	return (uint32_t)(u32_data);	
}

