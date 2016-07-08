#include "ServerOutput.h"
#include <stdio.h>
#include "../Utility/Mutex.h"

void GlobalPrint(unsigned int id, const char* str)
{
	static Mutex mu;
	mu.Lock();
	printf("Server [%d]: %s\n", id, str);
	mu.Unlock();
}