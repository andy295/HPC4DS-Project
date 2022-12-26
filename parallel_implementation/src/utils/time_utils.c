#include "time_utils.h"

double TimeUtils_lastTimeStamp = 0;
double TimeUtils_lastElapsedTime = 0;
int TimeUtils_indexOfFile = -1;
char* TimeUtils_lastFilename;

int ReferenceProcess = 0;

void takeTime(int pid) {
    if (pid == ReferenceProcess){
        double end = MPI_Wtime();
        TimeUtils_lastElapsedTime = end - TimeUtils_lastTimeStamp;
        TimeUtils_lastTimeStamp = end;
    }
}

void printTime(int pid, char* label) {
    if (pid == ReferenceProcess)
        printf("%s: %f\n", label, TimeUtils_lastElapsedTime);
}

double getTime() {
    return TimeUtils_lastElapsedTime;
}

void setTime(int pid, double time) {
    if (pid == ReferenceProcess)
        TimeUtils_lastElapsedTime = time;
}

// function to get number of lines in a file
int getNumberOfLines(FILE *fp) {
    int lines = 0;
    char * line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fp) != -1)
        lines++;

    return lines;
}

void saveTime(int pid, char* filename, char* label) {
    if (pid != ReferenceProcess)
        return;
        
    if (TimeUtils_lastFilename != filename) {
        TimeUtils_indexOfFile = -1;
        TimeUtils_lastFilename = filename;
    }

    // open csv file
    FILE *fp = fopen(filename, "a+");
    if (fp == NULL) {
        fp = fopen(filename, "w");
        TimeUtils_indexOfFile = 0;
        if (fp == NULL) {
            printf("Error opening file\n");
            exit(1);
        }
    }

    // read index of last row from file
    if (TimeUtils_indexOfFile == -1)
        TimeUtils_indexOfFile = getNumberOfLines(fp);

    // write to csv file
    fprintf(fp, "%d,%s,%f\n", TimeUtils_indexOfFile, label, TimeUtils_lastElapsedTime);
    TimeUtils_indexOfFile++; 

    // close csv file
    fclose(fp);
}

void setReferenceProcess(int pid) {
    ReferenceProcess = pid;

    // reset time
    TimeUtils_lastTimeStamp = 0;
    TimeUtils_lastElapsedTime = 0;
}