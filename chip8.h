#pragma once

#ifndef CHIP8_H
#define CHIP8_H
#include <stdint.h>

#include "consts.h"
#include "conf.h"

typedef struct {
	uint16_t pc;
	uint8_t registers[16];
	uint16_t i;
	uint16_t stack[16];
	uint16_t sp;

	uint8_t st;
	uint8_t dt;

	uint8_t mem[MEMORY_CAP];
	uint8_t framebuffer[DISPLAY_LEN];
	uint8_t keyboard[16];

	// if 1 then render screen
	// else do not render
	int render;
} chip;

// initialization function
// returns 1 on fail, 0 on success
int chip_init(chip *ch);

// rom filepath
// return 1 on fail, for example failed to open rom file
// return 0 on success
int chip_run(chip *ch, conf *conf);

// rom filepath
// return 1 on fail
// return 0 on success
int load_rom(chip *ch, char *rom_filepath);

#endif
