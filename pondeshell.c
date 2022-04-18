#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <errno.h>
#include <regex.h>
#include <fcntl.h>

#define clear() printf("\033[H\033[J")
#define MAX_LIMIT 1024
#define MAX_ARGS 8

int command_handler();
void ioredirection();
void exec_command();
void fix_command_args();
void printList();
void insertToLinkedList();
int removeFromLinkedList();
int isEmpty();
void inputHandler();

char input[MAX_LIMIT];
char args_str[MAX_LIMIT];
char *parsed[MAX_ARGS];
char *args[MAX_ARGS];
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

struct node
{
    int pid;
    char *command;
    struct node *next;
};

struct node *head = NULL;
struct node *tail = NULL;

void printList()
{
    struct node *ptr = head;
    printf("\n[ ");
    if (ptr == NULL)
    {
        printf("There are no background processes \n");
    }
    while (ptr != NULL)
    {
        printf("Pid: %d, command: %s\n", ptr->pid, ptr->command);
    }
    printf(" ]");
}

void insertToLinkedList(int pid, char *commandArgs)
{
    if (isEmpty() == 1)
    {
        head = (struct node *)malloc(sizeof(struct node));
        head->pid = pid;
        head->command = strdup(commandArgs);
        tail = head;
    }
    else
    {
        struct node *newNode = (struct node *)malloc(sizeof(struct node));
        newNode->pid = pid;
        newNode->command = strdup(commandArgs);
        tail->next = newNode;
        tail = newNode;
    }
}

int removeFromLinkedList(int pid)
{
    struct node *ptr = head;
    struct node *previous = head;

    if (isEmpty() == 1)
    {
        return 0;
    }

    while (ptr != NULL)
    {
        if (ptr->pid == pid)
        {
            if (ptr == head)
            {
                head = NULL;
                tail = NULL;
            }
            else
            {
                previous->next = ptr->next;
            }
        }
        free(ptr);
        break;
    }

    previous = ptr;
    ptr = ptr->next;
    return 1;
}

int isEmpty()
{
    struct node *ptr = head;
    if (ptr == NULL)
    {
        return 1;
    };
    return 0;
}

void catchZombie()
{
    int status;

    int pid = waitpid(-1, &status, WNOHANG);

    if (pid > 0)
    {
        removeFromLinkedList(pid);
        catchZombie();
    }
}

void fix_command_args()
{

    for (int i = 0; i < MAX_ARGS; i++)
    {
        if (parsed[i] == NULL || parsed[i] == "\0")
        {
            args[i] = NULL;
            continue;
        }

        args[i] = parsed[i];
    }
}

int command_handler(int runBackgroundProcess)
{
    int num_commands = 3, commandswitch = 0;
    char *commands[num_commands];
    commands[0] = "exit";
    commands[1] = "cd";
    commands[2] = "jobs";
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
        printList();
        return 1;

    default:
        exec_command(runBackgroundProcess);
        break;
    }
    return 0;
}

void inputHandler()
{

    char *token;
    char *midl_token;
    char lastChar;
    int count = 0;

    int runBackgroundProcess;
    memset(input, 0, sizeof(input));

    if ((midl_token = fgets(input, MAX_LIMIT, stdin)) == NULL)
    {
        perror("Could not find &");
    }

    strcpy(args_str, input);
    args_str[strlen(args_str) - 1] = '\0';

    lastChar = midl_token[(strlen(midl_token) - 2)];

    if (lastChar == '&')
    {
        runBackgroundProcess = 1;
        midl_token[(strlen(midl_token) - 2)] = '\0';
    }
    else
    {
        runBackgroundProcess = 0;
    }

    token = strtok(input, seps);
    while (token != NULL)
    {
        parsed[count++] = token;
        token = strtok(NULL, seps);
    }

    parsed[count] = NULL;
    command_handler(runBackgroundProcess);
}

void exec_command(int runBackgroundProcess)
{
    int options = 0;

    if (runBackgroundProcess == 1)
    {
        options = WNOHANG;
    }
    pid_t pid = fork();

    if (pid < 0)
    {
        printf("Failed to create child process\n");
    }
    else if (pid == 0)
    {

        ioredirection();
        fix_command_args();
        for (int i = 0; i < MAX_ARGS; i++)
        {
            printf("%s \n", args[i]);
        }
        if (execvp(*args, args) < 0)
        {
            perror("could not execute command");
        }

        exit(0);
    }
    else
    {
        int status;
        waitpid(pid, &status, options);
        if (runBackgroundProcess)
        {
            insertToLinkedList(pid, *args);
        }
        printf("Exit status [%s]: %i\n", args_str, status);
        return;
    }
}

void ioredirection()
{

    int in = -1, out = -1;
    int count = 0;

    char i_location[128];
    char o_location[128];
    while (parsed[count] != NULL)
    {
        if (strcmp(parsed[count], "<") == 0)
        {
            in = count;

            parsed[count] = "\0";

            strcpy(i_location, parsed[count + 1]);
            parsed[count + 1] = "\0";
        }
        if (strcmp(parsed[count], ">") == 0)
        {
            out = count;
            parsed[count] = "\0";

            strcpy(o_location, parsed[count + 1]);
            parsed[count + 1] = "\0";
        }
        count++;
    }
    fix_command_args();

    if (in < 0 && out < 0)
    {
        return;
    }

    if (in > 0)
    {
        int file = open(i_location, O_RDONLY, 0);
        if (file < 0)
        {
            perror("File could not be opened");
            exit(0);
        }
        dup2(file, 0);
        close(file);
    }

    if (out > 0)
    {
        int file1 = creat(o_location, 0644);
        if (file1 < 0)
        {
            perror("Could not open the file");
            exit(0);
        }
        dup2(file1, 1);
        close(file1);
    }
}

int main(int argc, char const *argv[])
{
    init_shell();
    while (1)
    {
        printDir();
        inputHandler();
        catchZombie();
    }
    return 0;
}