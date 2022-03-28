#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <errno.h>
#include <mach/error.h>

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

// initalizes shell
void init_shell()
{
    clear();
    printf("\n\n\n\n******************"
           "************************");
    printf("\n\n\n\tWELCOME TO MATRIX");
    printf("\n\n\tBLUE OR RED PILL?");
    printf("\n\n\n\n*******************"
           "***********************");
    char *username = getenv("USER");
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
    sleep(3);
    clear();
}

// prints directory path
void printDir()
{
    getcwd(cwd, sizeof(cwd));
    printf("Dir: %s: ", cwd);
}

// take input from user
void take_input()
{
    fgets(input, MAX_LIMIT, stdin);
}

// process input from user
void process_input()
{
    char *token;
    int count = 0;
    token = strtok(input, seps);
    while (token != NULL)
    {
        parsed[count++] = token;
        token = strtok(NULL, seps);
    }
    parsed[count] = NULL;
    int status = command_handler();
}

int command_handler()
{

    int num_commands = 3, commandswitch = 0;
    char *commands[num_commands];

    commands[0] = "exit";
    commands[1] = "cd";
    commands[2] = "help";
    // int num_commands = sizeof(commands);

    for (int i = 0; i < num_commands; i++)
    {
        if (strcmp(parsed[0], commands[i]) == 0)
        {
            commandswitch = i + 1;
            break;
        }
    }

    switch (commandswitch)
    {
    case 1:
        printf("Exit's shell. Have a nice day!\n");
        exit(0);
        break;

    case 2:
        chdir(parsed[1]);
        return 1;

    case 3:
        printf("List of available commands: \n");
        for (int i = 0; i < num_commands; i++)
        {
            printf("\t%s \n", commands[i]);
        }
        return 1;

    default:
        exec_command();
        break;
    }

    return 0;
}

// execute command
void exec_command()
{
    pid_t pid = fork();

    if (pid < 0)
    {
        printf("Failed to create child process\n");
    }
    else if (pid == 0)
    {
        printf("Executing %s\n", parsed[0]);
        if (execvp(parsed[0], parsed) < 0)
        {
            unix_err(errno);
        }
        exit(0);
    }
    else
    {
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
        process_input();
    }

    return 0;
}
