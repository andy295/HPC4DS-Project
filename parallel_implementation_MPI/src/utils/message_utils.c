#include "message_utils.h"

bool useMasterProcess(int *proc_number, bool withMaster) {
	if (*proc_number % 2 != 0 && withMaster) {
		--(*proc_number);
		return false;
	}
	else if (*proc_number % 2 != 0 && !withMaster) {
		++(*proc_number);
		return true;
	}

	return withMaster;
}

bool calculateSenderReceiver(int proc_number, int pid, int *sender, int *receiver) {
    if (pid == 0 && proc_number % 2 != 0)
        return false;

    if (proc_number % 2 == 0) {
        if (pid % 2 == 0) {
            *receiver = pid;
            *sender = pid + 1;
        } else {
            *receiver = pid - 1;
            *sender = pid;
        }
    } else {
        if (pid % 2 == 0) {
            *receiver = pid - 1;
            *sender = pid;
        } else {
            *receiver = pid;
            *sender = pid + 1;
        }
    }

    return true;
}