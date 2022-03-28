#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include <error.h>
#include <errno.h>

#define clear() printf("\033[H\033[J")
#define MAX_LIMIT 1024
#define MAX_ARGS 10

int command_handler();
void command_executer();
void exec_command();

char cwd[MAX_LIMIT];
char input[MAX_LIMIT];
char *parsed[MAX_ARGS];
char seps[] = " \t\r\n";

void init_shell()
{
    clear();
    printf("\n\n\n\n******************"
        "************************");
    printf("\n\n\n\t****MY SHELL****");
    printf("\n\n\t-USE AT YOUR OWN RISK-");
    printf("\n\n\n\n*******************"
        "***********************");
    char* username = getenv("USER");
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
    sleep(1);
    clear();
}

void printDir()
{
    getcwd(cwd, sizeof(cwd));
    printf("Dir: %s: ", cwd);
}

void take_input() {
    fgets(input, MAX_LIMIT, stdin);
}

void process_input() {

    char *token;
    int count = 0;
    
    token = strtok( input, seps );

    while( token != NULL )
    {
        /* While there are tokens in "string" */
        parsed[count++] = token;
        /* Get next token: */
        token = strtok( NULL, seps );
    }
    parsed[count] = NULL;

    
    // for (int i = 0; i < MAX_ARGS; i++) {     
    //     printf("%s \n", parsed[i]);     
    // }    
    int status;
    
    if (command_handler() == 1 ) {
        return;
    } else {
        //TODO
        exec_command();
    }
}

// void command_executer() {

//     // TODO: fix zombies
//     int exit_status;
//     pid_t pid = fork();

//     if (pid < 0) {
//         printf("Failed to create child process\n");
//         exit(0);                
//     } else if (pid == 0) {
//         exit_status = command_handler();
//         exit(0);
//     } else {
//         wait(0);
//         int status;
//         waitpid(pid, &status, 0);
//         printf("child process finished\n");
//         return;
//     }
// }

int command_handler() {

    int num_commands = 3, commandswitch = 0;
    char* commands[num_commands];
    
    commands[0] = "exit";
    commands[1] = "cd";
    commands[2] = "help";

    int i;

    for (i = 0; i < num_commands; i++)
    {
        if (strcmp(parsed[0], commands[i]) == 0) {
            commandswitch = i + 1;
            break;
        }
    }

    switch (commandswitch)
    {
    case 1:
        printf("Goodbye motherfucker!\n");
        exit(0);
        break;
    
    case 2:
        chdir(parsed[1]);
        return 1;

    case 3:
        printf("List of available commands: \n");
        for (int i = 0; i < num_commands; i++) {     
            printf("\t%s \n", commands[i]);     
        }
        return 1;
    
    default:
        // printf("Command '%s' not found.\n", parsed[0]);
        break;
    }
    
    return 0;
}

void exec_command() {

    // Create child process
    pid_t pid = fork();

    if (pid < 0) {
        printf("Failed to create child process\n");
    } else if (pid == 0) {
        printf("Executing %s\n", parsed[0]);
        if (execvp(parsed[0], parsed) < 0) {
            error(0, errno, "Could not execute command");
        }
        exit(0);
    } else {
        int status;
        waitpid(pid, &status, 0);
        return;
    }
}
  

int main(int argc, char const *argv[])
{
    init_shell();

    while (1)
    {   
        printDir();
        take_input();
        //exec_command();
        process_input();
        
    }
    

    return 0;
}
