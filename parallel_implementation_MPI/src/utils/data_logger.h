#pragma once

#include "../include/global_constants.h"

#include "file_utils.h"

typedef struct Logger {
    char *DataLogHeader;
    int itemsInHeader;

    char *DataLogRow;
    int itemsInRow;

    int DataLoggerReferenceProcess;

    bool active;
} Logger;

void saveRowToFile(char *filename);

extern void initDataLogger(int pid, bool enable);
extern void terminateDataLogger();

extern void addLogColumn(int pid, const char *columnName);
extern void addLogData(int pid, char *data);

extern void setDataLoggerReferenceProcess(int pid);