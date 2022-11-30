#include "print_utils.h"

void printFormattedChar(char c) {
	switch (c) {
	case '\n':
		printf("\\n");
		break;
	case '\0':
		printf("\\0");
		break;
	case '\t':
		printf("\\t");
		break;
	case ' ':
		printf("space");
		break;		
	default:
		printf("%c", c);
		break;
	}
}
