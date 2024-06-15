#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

int main(int argc, char *argv[argc + 1]) {
	if (argc < 2) {
		fprintf(stderr, "error: rom is not defined\n");
		return EXIT_FAILURE;
	}

	chip c;
	conf conf;

	if (readconf(&conf, "chip8.conf")) return EXIT_FAILURE;

	if (chip_init(&c)) return EXIT_FAILURE;

	if (load_rom(&c, argv[1])) return EXIT_FAILURE;	
	
	if (chip_run(&c, &conf)) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
