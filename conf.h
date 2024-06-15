#pragma once

#ifndef CONF_H
#define CONF_H
#include <stdint.h>

typedef struct {
	uint32_t screen_width;
	uint32_t screen_height;
	uint32_t frame_delay;
} conf;

int readconf(conf *c, char *filepath);

#endif
