#pragma once

#include "../include/global_constants.h"

#include "file_utils.h"

typedef struct DataLogger {
    char *logHeader;
    int itemsInHeader;

    char *logRow;
    int itemsInRow;

    int referenceProcess;

    bool active;
} DataLogger;

void saveRowToFile(char *filename);

extern void initDataLogger(int pid, bool enable);
extern void terminateDataLogger();

extern void addLogColumn(int pid, const char *columnName);
extern void addLogData(int pid, char *data);

extern void setDataLoggerReferenceProcess(int pid);
extern void enableDataLogger(int pid);
