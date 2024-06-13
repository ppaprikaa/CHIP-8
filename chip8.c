#include "chip8.h"

#include <stdint.h>
#include <string.h>

int8_t fontset[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

uint8_t fetch_x(chip *ch) {
	return (ch->mem[ch->pc] << 8 | ch->mem[ch->pc + 1] & 0x0F00) >> 8;
}

uint8_t fetch_y(chip *ch) {
	return (ch->mem[ch->pc] << 8 | ch->mem[ch->pc + 1] & 0x00F0) >> 4;
}

uint16_t fetch_nnn(chip *ch) {
	return (ch->mem[ch->pc] << 8 | ch->mem[ch->pc + 1]) & 0x0FFF;
}

uint8_t fetch_kk(chip *ch) {
	return (ch->mem[ch->pc] << 8 | ch->mem[ch->pc + 1]) & 0x00FF;
}

uint8_t fetch_n(chip *ch) {
	return (ch->mem[ch->pc] << 8 | ch->mem[ch->pc + 1]) & 0x000F;
}

// 00E0
void chip_cls(chip *ch) {
	memset(&ch->framebuffer, 0, 2048);
	ch->render = 1;
}

// 00EE
void chip_ret(chip *ch) {
	ch->sp--;
	ch->pc = ch->stack[ch->sp];
}

// 1nnn
void chip_jump_nnn(chip *ch) {
	ch->pc = fetch_nnn(ch);
}

// 2nnn
void chip_call_nnn(chip *ch) {
	ch->stack[ch->sp] = ch->pc;
	ch->pc++;
	ch->pc = fetch_nnn(ch);
}

// 3xkk
void chip_se_xkk(chip *ch) {
	if (ch->registers[fetch_x(ch)] == fetch_kk(ch)) ch->pc += 2;
}

// 4xkk
void chip_sne_xkk(chip *ch) {
	if (ch->registers[fetch_x(ch)] != fetch_kk(ch)) ch->pc += 2;
}

// 5xy0
void chip_se_xy(chip *ch) {
	if (ch->registers[fetch_x(ch)] == ch->registers[fetch_y(ch)]) ch->pc += 2;
}

// 6xkk
void chip_ld_xkk(chip *ch) {
	ch->registers[fetch_x(ch)] = fetch_kk(ch);
}

// 7xkk
void chip_add_xkk(chip *ch) {
	uint8_t x = fetch_x(ch);
	ch->registers[x] = ch->registers[x] + fetch_kk(ch);
}

// 8xy0
void chip_ld_xy(chip *ch) {
	ch->registers[fetch_x(ch)] = ch->registers[fetch_y(ch)];
}

// 8xy1
void chip_or_xy(chip *ch) {
	uint8_t x = fetch_x(ch);
	ch->registers[x] = ch->registers[x] | ch->registers[fetch_y(ch)];
}
