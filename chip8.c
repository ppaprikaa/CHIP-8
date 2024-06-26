#include "chip8.h"
#include "consts.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint8_t fontset[80] = {
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

int chip_init(chip *ch) {
	ch->i = 0;
	ch->pc = 0x200;
	ch->sp = 0;
	ch->st = 0;
	ch->dt = 0;

	memset(ch->stack, 0, 16);
	memset(ch->registers, 0, 16);
	memset(ch->keyboard, 0, 16);
	memset(ch->mem, 0, MEMORY_CAP);
	memset(ch->framebuffer, 0, DISPLAY_LEN);
	memcpy(ch->mem, fontset, 80*sizeof(uint8_t));

	return 0;
}

int load_rom(chip *ch, char *rom_filepath) {
	FILE *fp = fopen(rom_filepath, "r");
	if (!fp) {
		fprintf(stderr, "error: <load_rom> failed to open file: %s\n", rom_filepath);
		return 1;
	}

	fseek(fp, 0, SEEK_END);
	int len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fread(ch->mem + 0x200, sizeof(uint16_t), len, fp);

	return 0;
}

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

// 8xy2
void chip_and_xy(chip *ch) {
	uint8_t x = fetch_x(ch);
	ch->registers[x] = ch->registers[x] & ch->registers[fetch_y(ch)];
}

// 8xy3
void chip_xor_xy(chip *ch) {
	uint8_t x = fetch_x(ch);
	ch->registers[x] = ch->registers[x] ^ ch->registers[fetch_y(ch)];
}

// 8xy4
void chip_add_xy(chip *ch) {
	uint8_t x = fetch_x(ch);
	uint8_t y = fetch_y(ch);

	int result = (int)ch->registers[x] + (int)ch->registers[y];
	ch->registers[0x0F] = result > 255 ? 1 : 0;
	ch->registers[x] = (uint8_t)result;
}

//8xy5
void chip_sub_xy(chip *ch) {
	uint8_t x = fetch_x(ch);
	uint8_t y = fetch_y(ch);
	
	ch->registers[0x0F] = ch->registers[x] > ch->registers[y] ? 1 : 0;
	ch->registers[x] = ch->registers[x] - ch->registers[y];
}

// 8xy6
void chip_shr_xy(chip *ch) {
	uint8_t x = fetch_x(ch);

	ch->registers[0x0F] = ch->registers[x] & 0x01;
	ch->registers[x] = ch->registers[x] / 2;
}

// 8xy7
void chip_subn_xy(chip *ch) {
	uint8_t x = fetch_x(ch);
	uint8_t y = fetch_y(ch);

	ch->registers[0x0F] = ch->registers[y] > ch->registers[x] ? 1 : 0;
	ch->registers[x] = ch->registers[y] - ch->registers[x];
}

// 8xyE
void chip_shl_xy(chip *ch) {
	uint8_t x = fetch_x(ch);

	ch->registers[0x0F] = ch->registers[x] & 0x80;
	ch->registers[x] = ch->registers[x] * 2;
}

// 9xy0
void chip_sne_xy(chip *ch) {
	if (ch->registers[fetch_x(ch)] != ch->registers[fetch_y(ch)]) ch->pc += 2;
}

// Annn
void chip_ld_innn(chip *ch) {
	ch->i = fetch_nnn(ch);
}

// Bnnn
void chip_jump_0nnn(chip *ch) {
	ch->pc = ch->registers[0x00] + fetch_nnn(ch);
}

// Cxkk
void chip_rnd_xkk(chip *ch) {
	ch->registers[fetch_x(ch)] = fetch_kk(ch) & (uint8_t)random();
}

// Dxyn
void chip_drw_xyn(chip *ch) {
	ch->registers[0x0F] = 0;

	uint8_t x = fetch_x(ch);
	uint8_t y = fetch_y(ch);
	uint8_t n = fetch_n(ch);

	size_t xpos = (size_t)ch->registers[x] % DISPLAY_WIDTH;
	size_t ypos = (size_t)ch->registers[y] % DISPLAY_HEIGHT;

	for (size_t row = 0; row < n; row++) {
		uint8_t bits = ch->mem[(size_t)ch->i + row];
		size_t cy = (ypos + row) % DISPLAY_HEIGHT;

		for (size_t col = 0; col < 8; col++) {	
			size_t cx = (xpos + col) % DISPLAY_WIDTH;
			uint8_t current_color = ch->framebuffer[cy * DISPLAY_WIDTH + cx];
			uint8_t color = bits & (0x01 << (7 - (uint8_t)col));
			if (color > 0) {
				if (current_color > 0) {
					ch->framebuffer[cy * DISPLAY_WIDTH + cx] = 0;
					ch->registers[0x0F] = 1;
				} else {
					ch->framebuffer[cy * DISPLAY_WIDTH + cx] = 1;
				}
			}

			if (cx == DISPLAY_WIDTH - 1) {
				break;
			}
		}
		if (cy == DISPLAY_HEIGHT - 1) {
			break;
		}
	}
	ch->render = 1;
}

// Ex9E
void chip_skp_xk(chip *ch) {
	if (ch->keyboard[ch->registers[fetch_x(ch)]]) ch->pc += 2;
}

// ExA1
void chip_sknp_xk(chip *ch) {
	if (!ch->keyboard[ch->registers[fetch_x(ch)]]) ch->pc += 2;
}

// Fx07
void chip_ld_xdt(chip *ch) {
	ch->registers[fetch_x(ch)] = ch->dt;
}

// Fx0A
void chip_ld_kx(chip *ch) {
	int pressed = 0;
	for (uint8_t i = 0; i < 16; i++)	{
		if (ch->keyboard[i]) {
			ch->registers[fetch_x(ch)] = i;
			pressed = 1;
		}
	}

	if (!pressed) {
		ch->pc -= 2;
	}
}

// Fx15
void chip_ld_dtx(chip *ch) {
	ch->dt = ch->registers[fetch_x(ch)];
}

// Fx18
void chip_ld_stx(chip *ch) {
	ch->st = ch->registers[fetch_x(ch)];
}

// Fx1E
void chip_add_ix(chip *ch) {
	ch->i += ch->registers[fetch_x(ch)];
}

// Fx29
void chip_ld_ifx(chip *ch) {
	ch->i = (uint16_t)ch->registers[fetch_x(ch)] * 0x05;
}

// Fx33
void chip_ld_imx(chip *ch) {
	uint8_t num = ch->registers[fetch_x(ch)];
	uint16_t i = ch->i;
	ch->mem[i] = num / 100;
	ch->mem[i + 1] = (num - (ch->mem[i] * 100)) / 10;
	ch->mem[i + 2] = num - ch->mem[i] * 100 - ch->mem[i + 1] * 10;
}

// Fx55
void chip_ld_irmx(chip *ch) {
	for (size_t i = 0; i < fetch_x(ch); i++) ch->mem[ch->i + i] = ch->registers[i];
}

// Fx65
void chip_ld_imrx(chip *ch) {
	for (size_t i = 0; i < fetch_x(ch); i++) ch->registers[i] = ch->mem[ch->i + i];
}
