#include "data_logger.h"

static const int MAX_DATA_LOGGER_ROW_SIZE = 1000;

Logger logger = { NULL, 0, NULL, 0, 0, false};

void initDataLogger(int pid, bool enable) {
    logger.dataLoggerReferenceProcess = pid;
    logger.active = enable;

    if (enable) {
        logger.dataLogHeader = (char*)malloc(sizeof(char) * MAX_DATA_LOGGER_ROW_SIZE);
        logger.dataLogRow = (char*)malloc(sizeof(char) * MAX_DATA_LOGGER_ROW_SIZE);

        addLogColumn(pid, "N.Processes");
        addLogColumn(pid, "N.Threads");
        addLogColumn(pid, "N.Characters");
        addLogColumn(pid, "Time");
    }
}
void addLogColumn(int pid, const char *name) {
    if (pid != logger.dataLoggerReferenceProcess)
        return;

    logger.itemsInHeader++;
    strcat(logger.dataLogHeader, name);
    strcat(logger.dataLogHeader, ",");
}

void addLogData(int pid, char *data) {
    if (pid != logger.dataLoggerReferenceProcess)
        return;

    if (data == NULL) {
        fprintf(stderr, "Error: invalid log data value\n");
        return;
    }

    strcat(logger.dataLogRow, data);
    strcat(logger.dataLogRow, ",");

    logger.itemsInRow++;
    if (logger.itemsInRow == logger.itemsInHeader) {
        logger.itemsInRow = 0;

        saveRowToFile(DATA_LOGGER_FILE);
    }

    freeBuffer(data);
}

void saveRowToFile(char *filename) {        
    // open csv file
    FILE *fp = fopen(filename, "a+");
    if (fp == NULL) {
        fp = fopen(filename, "w");

        if (fp == NULL) {
            fprintf(stderr, "Error opening file\n");
            exit(1);
        }
    }

    int indexOfFile = getNumberOfLines(fp);
    if (indexOfFile == 0) {
        fprintf(fp, "%s,%s\n", "Index", logger.dataLogHeader);
        indexOfFile++;
    }

    fprintf(fp, "%i,%s\n", indexOfFile, logger.dataLogRow);
    fclose(fp);
}

void setDataLoggerReferenceProcess(int pid) {
    if(!logger.active) {
        logger.active = true;
        initDataLogger(pid, true);
    }
    else
        logger.dataLoggerReferenceProcess = pid;
}

void terminateDataLogger() {
    if (!logger.active)
        return;

    logger.active = false;
    freeBuffer(logger.dataLogHeader);
    freeBuffer(logger.dataLogRow);
}
