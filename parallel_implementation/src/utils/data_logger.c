

#include "data_logger.h"

void initLogHeader(){
    char* header = "Index,N. Processes";
    
    itemsInHeader = 2;
    itemsInRow = 0;
    numberOfRows = 0;

    DataLogHeaderSize = strlen(header);
    DataLogHeader = (char*)malloc(DataLogHeaderSize*sizeof(char));
    strcpy(DataLogHeader, header);

    DataLogRowSize = 0;
    DataLogRow = (char*)malloc(DataLogRowSize*sizeof(char));
}

void addLogColumn(const char* name){
    DataLogHeaderSize += strlen(name);
    itemsInHeader++;
    DataLogHeader = (char*)realloc(DataLogHeader, DataLogHeaderSize*sizeof(char));
    strcat(DataLogHeader, name);
}

void addLogData(const char *data){
    DataLogRowSize += strlen(data);
    DataLogRow = (char*)realloc(DataLogRow, DataLogRowSize*sizeof(char));
    strcat(DataLogRow, data);

    itemsInRow++;
    if (itemsInRow == itemsInHeader){
        itemsInRow = 0;
        numberOfRows++;

        // write line to file
    }
}


void saveRowToFile(char* filename) {        
    // open csv file
    // FILE *fp = fopen(filename, "a+");
    // if (fp == NULL) {
    //     fp = fopen(filename, "w");
    //     if (fp == NULL) {
    //         printf("Error opening file\n");
    //         exit(1);
    //     }
    // }

    // read index of last row from file
    // if (TimeUtils_indexOfFile == -1)
    //     TimeUtils_indexOfFile = getNumberOfLines(fp);

    // write to csv file
    // fprintf(fp, "%d,%s,%f\n", TimeUtils_indexOfFile, label, TimeUtils_lastElapsedTime);
    // TimeUtils_indexOfFile++; 

    // close csv file
    // fclose(fp);
}
