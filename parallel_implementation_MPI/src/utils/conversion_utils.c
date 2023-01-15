#include "conversion_utils.h"

bool checkNumber(int number) {
	int size = snprintf(NULL, 0, "%d", number);
    
    if (size > MAX_DIGITS + 1)
        return false;

	return true;
}

char* intToString(int number) {
    if (!checkNumber(number))
        return NULL;

	char *str = (char*)malloc(sizeof(char) * (MAX_DIGITS+1));
	sprintf(str, "%d", number);

	return str;
}

char* floatToString(float number) {
	if (!checkNumber(number))
        return NULL;

	char *str = (char*)malloc(sizeof(char) * (MAX_DIGITS+1));
	gcvt(number, MAX_DIGITS, str);

	return str;
}

// return negavite value if it fails 
int stringToInt(char* str) {
	int number = atoi(str);
	return number;
}

int roundUp(int numToRound, int multiple) {
    if (multiple == 0)
        return numToRound;

    int remainder = abs(numToRound) % multiple;
    if (remainder == 0)
        return numToRound;

    if (numToRound < 0)
        return -(abs(numToRound) - remainder);
    else
        return numToRound + multiple - remainder;
}
