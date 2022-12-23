#pragma once

#include <stdlib.h>
#include <stdio.h>

int getNumberOfLines(FILE *fp); 

extern void takeTime(); 
extern void printTime(char* label);
extern double getTime(); 
extern void setTime(double time);
extern void saveTime(char* filename, char* label); 
