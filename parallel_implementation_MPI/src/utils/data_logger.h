#pragma once

#include "../include/global_constants.h"

#include "file_utils.h"

enum DataLoggerType {
    ENCODING,
    DECODING
};

typedef struct DataLogger {
    char *logHeader;
    int itemsInHeader;

    char *logRow;
    int itemsInRow;

    int referenceProcess;

    bool active;
} DataLogger;

void saveRowToFile(char *filename);

extern void initDataLogger(int pid, bool enable, int type);
extern void terminateDataLogger();

extern void addLogColumn(int pid, const char *columnName);
extern void addLogData(int pid, char *data);

extern void setDataLoggerReferenceProcess(int pid, int type);
extern void enableDataLogger(int pid, int type);
