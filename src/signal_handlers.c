#include "signal_handlers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void catch_sigint(int signalNo)
{	
	signal(SIGINT, &catch_sigint);
	printf("\nShell doesn't close!\n");
	
	
  // TODO: File this!
}

void catch_sigtstp(int signalNo)
{	
	signal(SIGTSTP, &catch_sigtstp);
	printf("\nIt doesn't move!\n");
  // TODO: File this!
}



