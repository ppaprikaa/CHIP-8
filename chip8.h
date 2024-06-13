#pragma once

#ifndef CHIP8_HEADER
#define CHIP8_HEADER
#include <stdint.h>

typedef struct {
	uint16_t pc;
	uint8_t registers[16];
	uint16_t i;
	uint16_t stack[16];
	uint16_t sp;

	uint8_t st;
	uint8_t dt;

	uint8_t mem[4096];
	uint8_t framebuffer[64 * 32];
	uint8_t keyboard[16];
} chip;

#endif
