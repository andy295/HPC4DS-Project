#include "data_logger.h"

static const int MAX_DATA_LOGGER_ROW_SIZE = 1000;

DataLogger dataLogger = { NULL, 0, NULL, 0, 0, false};

void initDataLogger(int pid, bool enable) {
    dataLogger.referenceProcess = pid;
    dataLogger.active = enable;

    if (enable) {
        dataLogger.logHeader = (char*)malloc(sizeof(char) * MAX_DATA_LOGGER_ROW_SIZE);
        dataLogger.logRow = (char*)malloc(sizeof(char) * MAX_DATA_LOGGER_ROW_SIZE);

        addLogColumn(pid, "N.Processes");
        addLogColumn(pid, "N.Threads");
        addLogColumn(pid, "N.Characters");
    }
}
void addLogColumn(int pid, const char *name) {
    if (pid != dataLogger.referenceProcess)
        return;

    dataLogger.itemsInHeader++;
    strcat(dataLogger.logHeader, name);
    strcat(dataLogger.logHeader, ",");
}

void addLogData(int pid, char *data) {
    if (pid != dataLogger.referenceProcess)
        return;

    if (data == NULL) {
        fprintf(stderr, "Error: invalid log data value\n");
        return;
    }

    strcat(dataLogger.logRow, data);
    strcat(dataLogger.logRow, ",");

    dataLogger.itemsInRow++;
    if (dataLogger.itemsInRow == dataLogger.itemsInHeader) {
        dataLogger.itemsInRow = 0;

        saveRowToFile(DATA_LOGGER_FILE);
    }

    freeBuffer(data);
}

void saveRowToFile(char *filename) {        
    // open csv file
    FILE *fp = openFile(filename, APPEND, 0);
    if (fp == NULL) {
        fp = openFile(filename, WRITE, 0);
        if (fp == NULL) {
            fprintf(stderr, "Error opening file\n");
            exit(1);
        }
    }

    int indexOfFile = getNumberOfLines(fp);
    if (indexOfFile == 0) {
        fprintf(fp, "%s,%s\n", "Index", dataLogger.logHeader);
        indexOfFile++;
    }

    fprintf(fp, "%i,%s\n", indexOfFile, dataLogger.logRow);
    fclose(fp);
}

void setDataLoggerReferenceProcess(int pid) {
    if(dataLogger.active)
        dataLogger.referenceProcess = pid;
    else
        initDataLogger(pid, true);
}

void terminateDataLogger() {
    if (!dataLogger.active)
        return;

    dataLogger.active = false;
    freeBuffer(dataLogger.logHeader);
    freeBuffer(dataLogger.logRow);
}

void enableDataLogger(int pid) {
    if (dataLogger.active)
        return;

    initDataLogger(pid, true);
}
