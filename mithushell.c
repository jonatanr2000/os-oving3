#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
//#include <errno.h>
#include <mach/error.h>
//#include <error.h>
#include <regex.h>
#include <fcntl.h>

#define clear() printf("\033[H\033[J")
#define MAX_LIMIT 1024
#define MAX_ARGS 10

int command_handler();
int ioredirection();
void exec_command();

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
    char *username = getenv("USER");
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
    sleep(1);
    clear();
}

void printDir()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("Dir: %s: ", cwd);
}

void process_input()
{

    char *token;
    int count = 0;

    token = strtok(input, seps);

    while (token != NULL)
    {
        /* While there are tokens in "string" */
        parsed[count++] = token;
        /* Get next token: */
        token = strtok(NULL, seps);
    }
    parsed[count] = NULL;

    // for (int i = 0; i < MAX_ARGS; i++) {
    //       printf("%s \n", parsed[i]);
    // }

    printf("%c", parsed);
    if (ioredirection() == 1) {
        return;
    }
    command_handler();
}


int command_handler()
{

    int num_commands = 4, commandswitch = 0;
    char *commands[num_commands];

    // TOOD fix exit to cmd d/control d
    commands[0] = "exit";
    commands[1] = "cd";
    commands[2] = "help";
    commands[3] = "wrong";

    int i;

    for (i = 0; i < num_commands; i++)
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
        printf("Goodbye motherfucker!");
        exit(0);
        break;

    case 2:
        chdir(parsed[1]);
        return 1;

    case 3:
        /* code */
        break;

    case 4:
        printf("\nHello.\nMind that this is "
               "not a place to play around."
               "\nUse help to know more..\n");
        return 1;

    default:
        exec_command();
        break;
    }
    return 0;
}


void exec_command()
{

    // Create child process
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
            //error(0, errno, "Failed run command.");
            //error();
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

int ioredirection() {

    int pid = fork();

    if(pid==0){
    
    int in = -1, out = -1;
    int count = 0;
    
    char ip[128];
    char op[128];

    while (parsed[count] != NULL)
    {
    
        if (strcmp(parsed[count], "<") == 0)
        {
            in = count;
            parsed[count] = NULL;
            strcpy(ip, parsed[count+1]);
        }
        if (strcmp(parsed[count], ">") == 0)
        {
            out = count;
            parsed[count] = NULL;
            strcpy(op, parsed[count+1]);
        }
        
        count++;
    }

    if (in < 0 && out < 0)
    {
        return 0;
    }

    if (in > 0)
    {   
        int file = open(ip, O_RDONLY, 0);
        if (file <0 )
        {
            perror("File could not be opened");
            exit(0);
        }
        dup2(file, 0);
        close(file);
    }

    if (out > 0)
    {
       int file1 = creat(op, 0644);
       if(file1 < 0) {
           perror("Could not open the file");
           exit(0);
       }
       dup2(file1, 1);
       close(file1);
    }

    //TODO execute kommando etter io redirection.
  
    }

    else {
        int status;
        waitpid(pid, &status, 0);
        return status;
    }

}


int main(int argc, char const *argv[])
{
    init_shell();
    // stdout_redirection();
    while (1)
    {
        //printDir();
        // exec_command();
        process_input();
    }

    return 0;
}