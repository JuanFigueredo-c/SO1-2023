#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

typedef struct{
    int elems;
    char** args;
} Command;

typedef struct{
    int cantCommands;
    Command** commands;
} CommandSet;

Command* init_command(){
    Command* command = malloc(sizeof(Command));
    command->elems = 0;
    command->args = malloc(sizeof(char*)*256);
    return command;
}

void dest_command(Command* command){
    for(int i = 0; i < command->elems; i++)
        free(command->args[i]);
    free(command);
}

CommandSet* init_CommandSet(){
    CommandSet* commandImput = malloc(sizeof(CommandSet));
    commandImput->cantCommands = 0;
    commandImput->commands = malloc(sizeof(Command)*256);
    return commandImput;
}

void dest_CommandSet(CommandSet* commandImput){
    for(int i = 0; i < commandImput->cantCommands; i++)
        dest_command(commandImput->commands[i]);
    free(commandImput->commands);
    free(commandImput);
}

/* La funcion parsea cada comando y separa todos sus argumentos */

void parse_command(Command* command, char* unparsedCommand){
    char *token = malloc(sizeof(char)*1024); 
    char s[2] = " ";
    unparsedCommand[strlen(unparsedCommand)] = '\0';
    token = strtok(unparsedCommand, s);
    while(token != NULL) {
		command->args[command->elems] = malloc(sizeof(char)*strlen(token));
		strcpy(command->args[command->elems],token);
	    command->elems++;
        token = strtok(NULL,s);
    }
    command->args[command->elems] = NULL;
    free(token);
}

/* La funcion parsea el grupo de comandos separados por el operador '|'
en varios comandos separados */

void parse_command_set(CommandSet* commandImput){
    char *token = malloc(sizeof(char)*256); 
    char *input = malloc(sizeof(char)*1024);
    char aux_command[256][256];
    input = fgets(input, 256, stdin);
    char s[2] = {'|','\n'};
    token = strtok(input, s);
    while(token != NULL) {
		strcpy(aux_command[commandImput->cantCommands],token);
        commandImput->cantCommands++;
        token = strtok(NULL,s);
    }
    for(int j = 0; j < commandImput->cantCommands; j++){
        commandImput->commands[j] = init_command();
        parse_command(commandImput->commands[j],aux_command[j]);
    }
    free(token);
    free(input);
}

/* La funcion ejecuta un comando recibido */

int execute_command(Command* command){
    int elems = command->elems;
    if(command->elems > 1 && strcmp(command->args[elems - 2],">")==0)	{
        int fd = open(command->args[elems - 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
        command->args[elems - 2] = NULL;
        dup2(fd,1);
    } 
    int error = execvp(command->args[0], command->args);
    printf("the '%s' command does not exists \n",command->args[0]);
    exit(1);
    return error;
}

/* La funcion recursivamente se encarga de anidar los comandos recibidos y crear pipes
conectando cada comando. La salida del primer comando se escribe en la entrada estandar 
de el segundo comando, y asi para el resto. */

void command_nesting(Command** commands, int cant_commands, int fdPipe[2]){
    if(cant_commands == 1){
        execute_command(commands[cant_commands - 1]);
    }
    else{
        int auxFdPipe[2];
        int auxFd;
        pipe(auxFdPipe);
        pid_t pid = fork();
        if(pid == 0){
            close(auxFdPipe[0]);
            auxFd = dup(1);
            dup2(auxFdPipe[1],1);
            command_nesting(commands, cant_commands - 1, auxFdPipe);
            close(auxFdPipe[1]);
            dup2(auxFd, 1);
            exit(1);
        }
        else{
            char buffer[1024];
            close(auxFdPipe[1]);
            dup2(auxFdPipe[0],0);
            if(fdPipe != NULL){
                close(fdPipe[0]);
                dup2(fdPipe[0], 1);
            }
            execute_command(commands[cant_commands - 1]);
            close(auxFdPipe[1]);
            close(fdPipe[1]);
        }
    }   
}

void shell() {

    printf("------------------- MINI SHELL -----------------------\n");
    printf("> Press 'q' to quit\n");
    CommandSet* commandImput;
    pid_t pid;
    int flag = 1;
    while(flag != 0){
        printf("> ");
        commandImput = init_CommandSet();
        parse_command_set(commandImput);
        if(strcmp(commandImput->commands[0]->args[0], "q") == 0)
            flag = 0;
        else{
            pid = fork();
            if(pid == 0){
                command_nesting(commandImput->commands, commandImput->cantCommands, NULL);   
                exit(1);
            }
            else {
                wait(NULL);
                dest_CommandSet(commandImput);
            }
        }
    }
}


int main(){
    shell();
    return 0;
}