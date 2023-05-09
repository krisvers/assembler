#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

char comments[3] = {
	'#', '/', ';'
};

char instructions[16][3] = {
	"ldi",
	"ldm",
	"str",
	"add",
	"sub",
	"jnz",
	"swb",
	"swc",
	"swd",
	"swe",
	"or\0",
	"and",
	"not",
	"shl",
	"shr",
	"hlt",
};

void decipher(char * str, size_t len) {
	for (; len; --len) {
		putc(*(str - len), stdout);
	}
}

int main(int argc, char ** argv) {
	if (argc != 2) {
		printf("Provide file name\n");
		return 1;
	}

	FILE * fp = fopen(argv[1], "r");
	if (fp == NULL) {
		printf("Possibly non-existent file\n");
		return 2;
	}

	fseek(fp, 0L, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	char * file = malloc(size);

	{
		char c = 0;
		size_t index = 0;
		while (c != EOF) {
			c = fgetc(fp);
			file[index++] = c;
		}
	}

	fclose(fp);

	{
		char buffer[40];
		uint8_t line = 0;

		for (size_t i = 0; i < size; i++) {
			if (line) {
				if (file[i] == '\n' || file[i] == comments[0] || file[i] == comments[1] || file[i] == comments[2]) {
					decipher(buffer, line);
					line = 0;
					continue;
				}

				buffer[line++] = file[i];

				continue;
			}

			if (file[i] == '\n' || file[i] == comments[0] || file[i] == comments[1] || file[i] == comments[2]) {
				continue;
			}

			if (file[i] != '\t' && file[i] != ' ') {
				buffer[line++] = file[i];
			}
		}
	}

	free(file);
}
