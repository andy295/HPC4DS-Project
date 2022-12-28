#include "data_logger.h"

void initDataLogger() {
    DataLoggerReferenceProcess = 0; 
    
    itemsInHeader = 0;
    itemsInRow = 0;

    MAX_DATA_LOGGER_ROW_SIZE = 1000;

    DataLogHeader = (char*)malloc(MAX_DATA_LOGGER_ROW_SIZE * sizeof(char));
    DataLogRow = (char*)malloc(MAX_DATA_LOGGER_ROW_SIZE * sizeof(char));
}

void addLogColumn(int pid, const char *name) {
    if (pid != DataLoggerReferenceProcess)
        return;

    itemsInHeader++;
    strcat(DataLogHeader, name);
    strcat(DataLogHeader, ",");
}

void addLogData(int pid, const char *data) {
    if (pid != DataLoggerReferenceProcess)
        return;

    strcat(DataLogRow, data);
    strcat(DataLogRow, ",");

    itemsInRow++;
    if (itemsInRow == itemsInHeader) {
        itemsInRow = 0;

        saveRowToFile(DATA_LOGGER_FILE);
    }
}

void saveRowToFile(char *filename) {        
    // open csv file
    FILE *fp = fopen(filename, "a+");
    if (fp == NULL) {
        fp = fopen(filename, "w");

        if (fp == NULL) {
            printf("Error opening file\n");
            exit(1);
        }
    }

    int indexOfFile = getNumberOfLines(fp);
    if (indexOfFile == 0) {
        fprintf(fp, "%s,%s\n", "Index", DataLogHeader);
        indexOfFile++;
    }

    fprintf(fp, "%i,%s\n", indexOfFile, DataLogRow);
    fclose(fp);
}

void setDataLoggerReferenceProcess(int pid) {
    DataLoggerReferenceProcess = pid;
}
