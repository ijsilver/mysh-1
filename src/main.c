#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "commands.h"
#include "built_in.h"
#include "utils.h"
#include "signal_handlers.h"

int main()
{
	
  char buf[8096];

  while (1) {
	signal(SIGINT, &catch_sigint);
	signal(SIGTSTP, &catch_sigtstp);
    fgets(buf, 8096, stdin);
    struct single_command commands[512];
    int n_commands = 0;
    int background = 0;
    if(buf[strlen(buf)-1]=='&'){
	background = 1;
	buf[strlen(buf)]=0;
	memset(running,NULL,1024);
	strcpy(running,buf);
	break;
    }
    mysh_parse_command(buf, &n_commands, &commands);
    if(background){
	bgpid_num = fork();
	if(bgpid_num){
		free_commands(n_commands, &commands);
		n_commands =0;
		continue;
	}
    }
    int ret = evaluate_command(n_commands, &commands);
    free_commands(n_commands, &commands);
    n_commands = 0;
   /* if(bgpid_num == 0){
	if(ret == 0){
		fprintf(stdout,"%d done %s\n",getpid(),running);
	}
	free_commands(n_commands,&commands);
	n_commands = 0;
	exit(0);
    }*/

    free_commands(n_commands, &commands);
    n_commands = 0;
	
    if (ret == 1) {
      break;
    } 
	memset(buf,NULL,8096);
  }
  return 0;
}
