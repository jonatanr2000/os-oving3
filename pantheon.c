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
    /*
    initializes shell
    */
    clear();
    printf("\n\n\n\n******************"
           "************************");
    printf("\n\n\n\t******FLUSH******");
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
    /*
    Prints directory
    */
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("Dir: %s: ", cwd);
}

struct node
{
    /*
    Creates a struct node
    */
    int pid;
    char *command;
    struct node *next;
};

struct node *head = NULL;
struct node *tail = NULL;

void printList()
{
    /* 
    Prints the background tasks
    */

    struct node *ptr = head;
    printf("Background tasks running:\n");
    if (ptr == NULL)
    {
        printf("~There are no background tasks~\n");
    }
    while (ptr != NULL)
    {
        printf("- PID: %d, command: %s\n", ptr->pid, ptr->command);
        ptr = ptr->next;
    }
}


void insertToLinkedList(int pid)
{
    /*
    Inserts given process into linked list of current processes
    */
    if (isEmpty() == 1)
    {
        head = (struct node *)malloc(sizeof(struct node));
        head->pid = pid;
        head->command = strdup(args_str);
        tail = head;
    }
    else
    {
        struct node *newNode = (struct node *)malloc(sizeof(struct node));
        newNode->pid = pid;
        newNode->command = strdup(args_str);
        tail->next = newNode;
        tail = newNode;
    }
}

int removeFromLinkedList(int pid)
{
    /*
    Removes given process from linked list of current processes
    */

    struct node *ptr = head;
    struct node *previous = head;

    if (isEmpty() == 1)
    {
        return 0;
    }

    if(ptr != NULL && ptr->pid==pid) {
        head = ptr ->next;
        free(ptr);
        return 1;
    }

    while (ptr != NULL && ptr->pid!=pid)
    {
        previous = ptr;
        ptr = ptr-> next;
    }

    previous->next = ptr->next;
    free(ptr);
    return 1;
}

int isEmpty()
{
    /*
    Checks if linked list of working processes is empty
    */
    struct node *ptr = head;
    if (ptr == NULL)
    {
        return 1;
    };
    return 0;
}

char* getCommandArgsFromPID(int pid) {
    
    /*
    Returns the command args as a string from the given PID if PID is in list
    of working background processes
    */

    struct node *ptr = head;
    char* command_args = "\0"; 

    if (isEmpty() == 1)
    {
        return command_args;
    }
    while (ptr != NULL) {
        if (ptr->pid==pid) {
            command_args = ptr->command;
            break;
        }
        ptr = ptr-> next;
    }

    return command_args;
}

void catchZombie()
{
    /*
    Checks for terminated processes and removes them from linked list of ongoing processes
    */
    int status;
    int pid = waitpid(-1, &status, WNOHANG);

    if (pid > 0)
    {


        char* pid_command;
        pid_command = getCommandArgsFromPID(pid);

        removeFromLinkedList(pid);
        
        if (WIFEXITED(status))
        {
            printf("Background process with PID: %d exited with status %d [%s]\n", pid, WEXITSTATUS(status), pid_command);
        }

        catchZombie();
    }
}

void fix_command_args()
{
    /*
    Removes all null-values from command array parsed and fills args-array with
    non-null values
    */
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
    /*
    Checks if any of the pre-determined commands are called. If not,
    exec_args function will be run which uses execvp() to execture 
    commands parsed by user
    */
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
        printf("Goodbye g!");
        exit(0);
        break;

    case 2:
        chdir(parsed[1]);
        return 1;

    case 3:
        catchZombie();
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
    /*
    Splits input args into a list of arguments seperated by separation tokens (parsed is the array used)
    */

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
    /*
    */
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

        if (execvp(*args, args) < 0)
        {
            perror("could not execute command");
        }

        exit(0);
    }
    else
    {
        int status;

        if (runBackgroundProcess)
        {
            insertToLinkedList(pid);
        }
        else 
        {
            waitpid(pid, &status, options);
            printf("PID: %d exited with status %d [%s]\n", pid, WEXITSTATUS(status), args_str);
        }
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
    init_shell(); // Displays a small welcome-message
    while (1)
    {
        printDir(); // Prints working directory
        inputHandler(); // Takes care of the input and runs commands
        catchZombie(); // Catches zombie processes and removes unneccessary processes
    }
    return 0;
}