
#pragma once

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

extern void takeTime(); 
extern void printTime(char* label);
extern double getTime(); 
extern void setTime(double time);
extern void saveTime(char* filename, char* label); 

int getNumberOfLines(FILE *fp); 
