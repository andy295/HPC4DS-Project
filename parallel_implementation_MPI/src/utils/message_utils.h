#pragma once

#include "../include/global_constants.h"

extern bool useMasterProcess(int *proc_number, bool withMaster);
extern bool calculateSenderReceiver(int proc_number, int pid, int *sender, int *receiver);
