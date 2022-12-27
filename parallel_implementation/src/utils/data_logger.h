#pragma once

#include <string.h>
#include <stdlib.h>

#include "../include/global_constants.h"
#include "file_utils.h"

char* DataLogHeader;
int itemsInHeader;

char* DataLogRow; 
int itemsInRow;

int DataLoggerReferenceProcess; 

int MAX_DATA_LOGGER_ROW_SIZE;

extern void initDataLogger();
extern void addLogColumn(int pid, const char *columnName);
extern void addLogData(int pid, const char *data);

extern void setDataLoggerReferenceProcess(int pid);

void saveRowToFile(char* filename);