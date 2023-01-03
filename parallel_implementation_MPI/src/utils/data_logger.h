#pragma once

#include "../include/global_constants.h"

#include "file_utils.h"

typedef struct Logger {
    char *dataLogHeader;
    int itemsInHeader;

    char *dataLogRow;
    int itemsInRow;

    int dataLoggerReferenceProcess;

    bool active;
} Logger;

void saveRowToFile(char *filename);

extern void initDataLogger(int pid, bool enable);
extern void terminateDataLogger();

extern void addLogColumn(int pid, const char *columnName);
extern void addLogData(int pid, char *data);

extern void setDataLoggerReferenceProcess(int pid);