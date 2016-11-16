#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char *cpuVendor(char *result);

uint16_t in_b(uint16_t reg);
uint16_t out_b(uint16_t reg, uint16_t val);

uint32_t in_l(uint32_t reg);
uint32_t out_l(uint32_t reg, uint32_t val);

#endif