#pragma once

#include "../include/global_constants.h"

#include "file_utils.h"
#include "data_logger.h"
#include "conversion_utils.h"

typedef struct TimeUtils {
    double lastTimeStamp;
    double lastElapsedTime;
    int indexOfFile;
    char *lastFilename;
    int referenceProcess;
} TimeUtils;

extern void takeTime(int pid);
extern void printTime(int pid, char *label);
extern double getTime();
extern void setTime(int pid, double time);
extern void saveTime(int pid, char *filename, char *label);
extern void timeCheckPoint(int pid, char *label);
extern void setTimeUtilsReferenceProcess(int pid);
