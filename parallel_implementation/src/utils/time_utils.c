#include "time_utils.h"

TimeUtils timeUtils = { 0, 0, -1, NULL, 0 };

void takeTime(int pid) {
    if (pid == timeUtils.referenceProcess) {
        double end = MPI_Wtime();
        timeUtils.lastElapsedTime = end - timeUtils.lastTimeStamp;
        timeUtils.lastTimeStamp = end;
    }
}

void printTime(int pid, char *label) {
    if (pid == timeUtils.referenceProcess)
        printf("%s: %f\n", label, timeUtils.lastElapsedTime);
}

double getTime() {
    return timeUtils.lastElapsedTime;
}

void setTime(int pid, double time) {
    if (pid == timeUtils.referenceProcess)
        timeUtils.lastElapsedTime = time;
}

void saveTime(int pid, char *filename, char *label) {
    if (pid != timeUtils.referenceProcess)
        return;

    if (timeUtils.lastFilename != filename) {
        timeUtils.indexOfFile = -1;
        timeUtils.lastFilename = filename;
    }

    // open csv file
    FILE *fp = openFile(filename, APPEND, 0);
    if (fp == NULL) {
        fp = openFile(filename, WRITE, 0);
        timeUtils.indexOfFile = 0;
        if (fp == NULL) {
            fprintf(stderr, "Error opening file\n");
            exit(1);
        }
    }

    // read index of last row from file
    if (timeUtils.indexOfFile == -1)
        timeUtils.indexOfFile = getNumberOfLines(fp);

    // write to csv file
    fprintf(fp, "%d,%s,%f\n", timeUtils.indexOfFile, label, timeUtils.lastElapsedTime);
    timeUtils.indexOfFile++;

    // close csv file
    fclose(fp);
}

void setTimeUtilsReferenceProcess(int pid) {
    timeUtils.referenceProcess = pid;

    // reset time
    timeUtils.lastTimeStamp = 0;
    timeUtils.lastElapsedTime = 0;
}

void timeCheckPoint(int pid, char *label) {
	takeTime(pid);
	printTime(pid, label);
	float time = getTime(pid, label);
	addLogData(pid, floatToString(time));
}
