#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>

#define clear() printf("\033[H\033[J")
#define MAX_LIMIT 1024
#define MAX_ARGS 10

char input[MAX_LIMIT];
char parsed[MAX_ARGS];
char seps[] = " \t";

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
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("Dir: %s> ", cwd);
}

void take_input() {
    fgets(input, MAX_LIMIT, stdin);
}

void process_input() {
    // int i;
    // for (i = 0; i < MAX_ARGS; i++)
    // {
    //     parsed = strtok(input);
    // }

    char *token;
    int count = 0;
    
    token = strtok( input, seps );
    // printf("%s", parsed[0]);
    // printf("%s", parsed[1]);
    while( token != NULL && count < 9)
    {
        /* While there are tokens in "string" */
        printf( " %s\n", token );
        /* Get next token: */
        token = strtok( NULL, seps );
        count++;
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
