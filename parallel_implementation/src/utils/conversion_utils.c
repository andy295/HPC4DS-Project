#include "conversion_utils.h"

char* intToString(int number) {
	char* str = (char*)malloc(10 * sizeof(char));
	sprintf(str, "%d", number);
	return str;
}

char* floatToString(float number) {
	char* str = (char*)malloc(10 * sizeof(char));
	sprintf(str, "%f", number);
	return str;
}
