#ifndef APP_CRC_CRC_H
#define APP_CRC_CRC_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

uint16_t CRCCode_get(const uint8_t *data, int len);


#endif
