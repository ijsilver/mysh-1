#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio_ext.h>
#include <unistd.h>
#include "commands.h"
#include "built_in.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <pthread.h>
#include"built_in.h"

#define BUFF_SIZE 1024
#define SOCKET_PATH "tpf_unix_sock.server"
#define SERVER_PATH "tpf_unix_sock.server"
#define CLIENT_PATH "tpf_unix_sock.clent"



static struct built_in_command built_in_commands[] = {
  { "cd", do_cd, validate_cd_argv },
  { "pwd", do_pwd, validate_pwd_argv },
  { "fg", do_fg, validate_fg_argv },
  
};



void * server(void*** argvp ){
	int server_socket, client_socket, client_addr_size;
	char buf[256];
	char ***argv=*(char ***)argvp;
	struct sockaddr_un server_addr;
	struct sockaddr_un client_addr;
	
	memset(&server_addr,0, sizeof(struct sockaddr_un));
	memset(&client_addr, 0, sizeof(struct sockaddr_un));
	memset(buf, 0, 256);

	server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if(server_socket < 0) {printf("server socket fail\n"); 
				exit(0);}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX; 
	strcpy(server_addr.sun_path, SOCKET_PATH);
	int len = sizeof(server_addr);
	unlink(SOCKET_PATH);
int state =bind(server_socket,(struct sockaddr*)&server_addr, len);
	if(state ==-1) {
		printf("bind error\n");
		exit(1);
	}
state = listen(server_socket, 5);
	if(state ==-1){
		printf("listen error\n");
		exit(1);
	}
while(1){
	client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &len);

if(client_socket == -1){
	printf("client connect fail\n");
	close(server_socket);
	close(client_socket);
	exit(1);
	}


int pid;
if((pid=fork())<0)
	perror("fork fail\n");
else if (pid !=0)
	sleep(1);
else {
	
	dup2(client_socket,0);
	execv(argv[0][0],argv[0]);
	}
}


close(client_socket);

}

void * client(void*** argvp){

	int client_socket;
	char ***argv=*(char ***)argvp;
	struct sockaddr_un server_addr;
	struct sockaddr_un client_addr;
	char buf[256];

	memset(&server_addr, 0, sizeof(struct sockaddr_un));
	memset(&client_addr,0, sizeof(struct sockaddr_un));
	

	client_socket = socket (AF_UNIX, SOCK_STREAM, 0);
	if(client_socket == -1)
	{	
		printf("client socket fail\n");
		exit(1);
	}
	bzero(&client_addr, sizeof(client_addr));
	client_addr.sun_family = AF_UNIX;
	strcpy(client_addr.sun_path, CLIENT_PATH);
	int len = sizeof(client_addr);
	
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SERVER_PATH);

	if(connect(client_socket, (struct sockaddr*)&server_addr, len) == -1)
	{
		printf("connect fail\n");
		exit(1);
	}
		

  
	int pid;
	if((pid = fork())<0)
	perror("fork fail\n");
	else if (pid != 0)
		sleep(1);
	else {
		dup2(client_socket,1);
		close(client_socket);
		execv(argv[0][0],argv[0]);
		exit(1);
	} 
	close(client_socket);
	exit(0);
}

static int is_built_in_command(const char* command_name)
{
  static const int n_built_in_commands = sizeof(built_in_commands) / sizeof(built_in_commands[0]);

  for (int i = 0; i < n_built_in_commands; ++i) {
    if (strcmp(command_name, built_in_commands[i].command_name) == 0) {
      return i;
    }
  }

  return -1; 
}

int evaluate_command(int n_commands, struct single_command (*commands)[512])
{
  if (n_commands > 0) {
    struct single_command* com = (*commands);

    assert(com->argc != 0);

    int built_in_pos = is_built_in_command(com->argv[0]);
    if (built_in_pos != -1) {
      if (built_in_commands[built_in_pos].command_validate(com->argc, com->argv)) {
        if (built_in_commands[built_in_pos].command_do(com->argc, com->argv) != 0) {
          fprintf(stderr, "%s: Error occurs\n", com->argv[0]);
        }
      } else {
        fprintf(stderr, "%s: Invalid arguments\n", com->argv[0]);
        return -1;
      }
    } else if (strcmp(com->argv[0], "") == 0) {
      return 0;
    } else if (strcmp(com->argv[0], "exit") == 0) {
      return 1;
    } else if (strcmp(com->argv[0], "vim") == 0){	
	for(int i=0;i<n_commands; i++){
		int pid, status;
		if((pid = fork())<0) perror("Don't execute fork");
		else if (pid >0) wait(&status);
		else  { 
			char temp[512] = "/usr/bin/";
			strcat(temp, com->argv[0]);
			strcpy(com->argv,temp);
			bgpid_num=getpid();
			printf("\n%d\n",bgpid_num);
			execv(com->argv[0], com->argv);
			if(execv(com->argv[0], com->argv)) 
			{ 	
				fprintf(stderr, "%s: command not found\n", com->argv[0]);
      				return -1;
			}
	
		}
	}
	return 0;
    } else if(n_commands >=2){
		
		int pid, status;
		pthread_t pt[2];
		int a =5;
		char ***pointer;
		pointer=&(com->argv);
		if((pid = fork())<0)
			perror("fork fail\n");
		if(pid > 0){
			com = (*commands)+1;
			pointer=&(com->argv);
			pthread_create(&pt[0], NULL, &server, (void*)&pointer);
			sleep(2);
		}		
		else {
		pthread_create(&pt[1], NULL, &client, (void*)&pointer);
		sleep(2);	
		}
		return 0;
	} else {


		for(int i=0;i<n_commands; i++){
			int status;
			int pid;	
		
			if((pid = fork())<0) perror("Don't execute fork");
			else if (pid>0){
	 			printf("%s\n",com->argv[com->argc-1]);
				if(strcmp(com->argv[com->argc-1],"&"))
				wait(&status);
			}
			else  {
			bgpid_num = getpid();
			printf("\n%d\n",bgpid_num);
        		execv(com->argv[0], com->argv);

			if(execv(com->argv[0], com->argv)) 
			{	 	
			char c[100] = "/bin/";
			strcat(c, com->argv[0]);
			com->argv[0] = c;
			execv(com->argv[0], com->argv);
			if(execv(com->argv[0],com->argv)){
				fprintf(stderr, "%s: command not found\n", com->argv[0]);
      				return -1;
				} 
			}
		}
	}
	
     
    }

	
  }
  return 0;
}

void free_commands(int n_commands, struct single_command (*commands)[512])
{
  for (int i = 0; i < n_commands; ++i) {
    struct single_command *com = (*commands) + i;
    int argc = com->argc;
    char** argv = com->argv;

    for (int j = 0; j < argc; ++j) {
      free(argv[j]);
    }

    free(argv);
  }

  memset((*commands), 0, sizeof(struct single_command) * n_commands);
}
