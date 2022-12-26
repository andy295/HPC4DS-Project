
#include <string.h>
#include <stdlib.h>

#define OUTPUT_FILE_NAME "../../output/data_log.csv"

char* DataLogHeader;
int DataLogHeaderSize;
int itemsInHeader;

char* DataLogRow; 
int DataLogRowSize; 
int itemsInRow;

int numberOfRows;

extern void initLogHeader();
extern void addLogColumn(const char *columnName);
extern void addLogData(const char *data);
