#include "data_logger.h"

static const int MAX_DATA_LOGGER_ROW_SIZE = 1000;

Logger logger = { NULL, 0, NULL, 0, 0, false};

void initDataLogger(int pid, bool enable) {
    logger.DataLoggerReferenceProcess = pid;
    logger.active = enable;

    if (enable) {
        logger.DataLogHeader = (char*)malloc(sizeof(char) * MAX_DATA_LOGGER_ROW_SIZE);
        logger.DataLogRow = (char*)malloc(sizeof(char) * MAX_DATA_LOGGER_ROW_SIZE);

        addLogColumn(pid, "N.Processes");
        addLogColumn(pid, "N.Threads");
        addLogColumn(pid, "N.Characters");
        addLogColumn(pid, "Time");
    }
}
void addLogColumn(int pid, const char *name) {
    if (pid != logger.DataLoggerReferenceProcess)
        return;

    logger.itemsInHeader++;
    strcat(logger.DataLogHeader, name);
    strcat(logger.DataLogHeader, ",");
}

void addLogData(int pid, char *data) {
    if (pid != logger.DataLoggerReferenceProcess)
        return;

    if (data == NULL) {
        fprintf(stderr, "Error: invalid log data value\n");
        return;
    }

    strcat(logger.DataLogRow, data);
    strcat(logger.DataLogRow, ",");

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
        fprintf(fp, "%s,%s\n", "Index", logger.DataLogHeader);
        indexOfFile++;
    }

    fprintf(fp, "%i,%s\n", indexOfFile, logger.DataLogRow);
    fclose(fp);
}

void setDataLoggerReferenceProcess(int pid) {
    if(!logger.active) {
        logger.active = true;
        initDataLogger(pid, true);
    }
    else
        logger.DataLoggerReferenceProcess = pid;
}

void terminateDataLogger() {
    if (!logger.active)
        return;

    logger.active = false;
    freeBuffer(logger.DataLogHeader);
    freeBuffer(logger.DataLogRow);
}
