#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

FILE * outfp;

char comments[3] = {
	'#', '/', ';'
};

struct Macro {
	char * name;
	size_t hash;
	char ** argv;
	size_t argc;
	char ** instructionv;
	size_t instructionc;
};

struct MNode {
	struct Macro m;
	void * greater;
	void * lesser;
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

struct MNode * macro_tree = NULL;

size_t hash(char * str) {
	size_t h = 5381;

	for (; *str != '\0'; str++) {
		h = h * 33 ^ *str;
	}

	return h;
}

void define_macro(char * name, char ** argv, size_t argc, char ** instructionv, size_t instructionc) {
	struct MNode * mn = malloc(sizeof(struct MNode));
	if (mn == NULL) {
		printf("malloc error\ntried defining macro: %s, %p, %lu, %p, %lu\n", name, (void *) argv, argc, (void *) instructionv, instructionc);
		return;
	}

	mn->m.name = malloc(strlen(name) + 1);
	mn->m.hash = hash(name);
	mn->m.argv = argv;
	mn->m.argc = argc;
	mn->m.instructionv = instructionv;
	mn->m.instructionc = instructionc;

	strcpy(mn->m.name, name);

	if (macro_tree == NULL) {
		macro_tree = mn;

		return;
	}

	struct MNode * c = macro_tree;
	while (1) {
		if (mn->m.hash > c->m.hash) {
			if (c->greater == NULL) {
				c->greater = mn;
				break;
			}

			c = c->greater;
			continue;
		}

		if (mn->m.hash == c->m.hash) {
			printf("Error: macro \"%s\" already defined!\n", mn->m.name);
			return;
		}

		if (c->lesser == NULL) {
			c->lesser = mn;
			break;
		}

		c = c->lesser;
		continue;
	}
}

struct Macro * get_macro(char * name) {
	size_t h = hash(name);

	struct MNode * c = macro_tree;
	while (c != NULL) {
		if (h == c->m.hash) {
			return &c->m;
		}

		if (h > c->m.hash) {
			c = c->greater;
			continue;
		}

		c = c->lesser;
		continue;
	}

	printf("Error: macro \"%s\" does not exist!\n", name);
	return NULL;
}

void run_macro(struct Macro * m) {
	printf("macro \"%s\":\n  instructions:\n", m->name);

	for (size_t i = 0; i < m->instructionc; i++) {
		printf("    %lu: %s\n", i, m->instructionv[i]);
	}

	printf("  args:\n");

	for (size_t i = 0; i < m->argc; i++) {
		printf("    %lu: %s\n", i, m->argv[i]);
	}
}

void convert(char * opcode, char * value) {
	int v = atoi(value);

	for (uint8_t i = 0; i < 16; i++) {
		if (strncmp(instructions[i], opcode, 3) == 0) {
			fprintf(outfp, "%c%c", (i << 4) | (v >> 8), v & 0xFF);
		}
	}
}

void decipher(char * str, size_t len) {
	char * opcode = malloc(3);
	char * value = malloc(4);

	uint8_t which = 0;
	uint8_t j = 0;

	for (size_t i = 0; i < len; i++) {
which_if:
		if (which == 1) {
			switch (str[i]) {
				case '#': return;
				case '/': return;
				case ';': return;
				default: break;
			}

			if (str[i] == ' ') {
				which = 2;
				j = 0;
				continue;
			}

			if (str[i] == '\0') {
				value[0] = '0'; value[1] = '0'; value[2] = '0'; value[3] = '0'; 

				convert(opcode, value);

				return;
			}

			opcode[j++] = str[i];
		} else if (which == 2) {
			switch (str[i]) {
				case '#': return;
				case '/': return;
				case ';': return;
				default: break;
			}

			if (str[i] == ' ') {
				continue;
			}

			if (str[i] == '\0') {
				convert(opcode, value);

				return;
			}

			value[j++] = str[i];
		} else {
			switch (str[i]) {
				case '#': return;
				case '/': return;
				case ';': return;
				default: break;
			}

			if (str[i] == ' ' || str[i] == '\t') {
				continue;
			}

			which = 1;
			goto which_if;
		}
	}

	convert(opcode, value);
}

void preprocess(char * line, size_t len) {
	line = line; len = len;
}

size_t strlen_nl(const char * str) {
	size_t sz = 0;
	for (; *str != '\n' && *str != '\0'; str++) { sz++; };

	return sz;
}

size_t linelen(const char * str) {
	size_t len = 0;
	for (; *str != '\0'; str++) { if (*str == '\n') { len++; } };

	return len;
}

char * get_line(char * str) {
	size_t len = strlen_nl(str);
	char * tmp = malloc(len + 1);

	strncpy(tmp, str, len);

	tmp[len] = '\0';

	return tmp;
}

char * get_line_index(char * str, size_t index) {
	size_t list_i = 0;

	while (index > 0) {
		list_i += strlen_nl(str + list_i) + 1;		

		index--;
	}

	size_t len = strlen_nl(str + list_i);
	char * tmp = malloc(len + 1);

	strncpy(tmp, str + list_i, len);

	tmp[len] = '\0';

	return tmp;
}

int main(int argc, char ** argv) {
	if (argc < 2) {
		printf("See %s --help or %s -h\n", argv[0], argv[0]);
		return 1;
	}

	if (argc >= 2) {
		if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
			printf("USAGE: %s [input assembly file] {output binary file}\nIf no output file is provided, it will output to \"out.bin\"\n", argv[0]);
			return -1;
		}
	}

	FILE * fp = fopen(argv[1], "r");
	if (fp == NULL) {
		printf("File %s can not be read from\n", argv[1]);
		return 2;
	}

	outfp = fopen((argc >= 3) ? argv[2] : "out.bin", "w");
	if (outfp == NULL) {
		printf("File %s can not be opened for writing\n", (argc >= 3) ? argv[2] : "out.bin");
		return 3;
	}

	fseek(fp, 0L, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	char * file = malloc(size);

	{
		int c = 0;
		size_t index = 0;
		while (c != EOF) {
			c = fgetc(fp);
			file[index++] = c;
		}

		file[index - 1] = '\0';
	}


	fclose(fp);

//	printf("%s", file);

	char * line;
	size_t len = 0;
	size_t llen = linelen(file);

	for (size_t i = 0; i < llen; i++) {
		line = get_line_index(file, i);
		len = strlen(line);

		preprocess(line, len);
		decipher(line, len);
	}

	fclose(outfp);

	free(file);
}
