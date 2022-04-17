#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <errno.h>
//#include <mach/error.h>
#include <error.h>
#include <regex.h>
#include <fcntl.h>

#define clear() printf("\033[H\033[J")
#define MAX_LIMIT 1024
#define MAX_ARGS 30

int command_handler();
int ioredirection();
void exec_command();
void fix_command_args();
void printList();
void insertToLinkedList();
int removeFromLinkedList();
int isEmpty();

char input[MAX_LIMIT];
char args_str[MAX_LIMIT];
char *parsed[MAX_ARGS];
char *args[MAX_ARGS];
char seps[] = " \t\r\n";

// background process
struct node
{
    int data;
    char *command;
    struct node *next;
};

struct node *head = NULL;
struct node *current = NULL;

// display the list
void printList()
{
    struct node *ptr = head;
    printf("\n[ ");
    if (ptr == NULL)
    {
        printf("There are no background processes");
    }
    // start from the beginning
    while (ptr != NULL)
    {
        printf("Pid: %d, command: %s\n", ptr->data, ptr->command);
    }
    printf(" ]");
}

// insert link at the first location
void insertToLinkedList(int data, char *commandArgs)
{
    if (isEmpty() == 1)
    {
        head = (struct node *)malloc(sizeof(struct node));
        head->data = data;
        head->command = strdup(commandArgs);
        current = head;
    }
    else
    {
        struct node *inputNode = (struct node *)malloc(sizeof(struct node));
        inputNode->data = data;
        inputNode->command = strdup(commandArgs);
        current->next = inputNode;
        current = inputNode;
    }
}

// delete a link with given key
int removeFromLinkedList(int data)
{
    struct node *current = head;
    struct node *previous = NULL;

    if (isEmpty() == 1)
    {
        return 0;
    }
}

// if list is empty
int isEmpty()
{
    if (head == NULL)
    {
        return 1;
    };
    return 0;
}

// background process

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

    // Saving the argument string for later use
    strcpy(args_str, input);
    args_str[strlen(args_str) - 1] = '\0';

    char *token;
    char *midl_token;
    char lastChar;
    int count = 0;

    // background process kode
    int runBackgroundProcess;
    memset(input, 0, sizeof(input));

    if ((midl_token = fgets(input, MAX_LIMIT, stdin)) == NULL)
    {
        //print_error();
        perror("Could not find &");
        //unix_err(errno);
    }
    lastChar = midl_token[(strlen(midl_token) - 2)];
    printf("%c \n", lastChar);

    if (lastChar == '&')
    {
        runBackgroundProcess = 1;
        //printf("%s \n", midl_token);
        midl_token[(strlen(midl_token) - 2)] = '\0';
        //printf("%s \n", midl_token);
    }
    else
    {
        runBackgroundProcess = 0;
    }
    printf("%d \n", runBackgroundProcess);
    // background process kode

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

    // printf("%c", parsed);
    if (ioredirection() == 0)
    {
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
        fix_command_args();

        // printf("Executing [%s]\n", input);
        if (execvp(*args, args) < 0)
        {
            error(0, errno, "Failed run command.");
            //unix_err(errno);
        }
        exit(0);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        printf("Exit status [%s]: %i\n", args_str, status);
        return;
    }
}

void fix_command_args()
{

    for (int i = 0; i < MAX_ARGS; i++)
    {
        if (parsed[i] == NULL || parsed[i] == "\0")
        {
            continue;
        }

        args[i] = parsed[i];
    }

    // for (int i = 0; i < MAX_ARGS; i++) {
    //       printf("%s \n", args[i]);
    // }
}

int ioredirection()
{

    int pid = fork();

    if (pid == 0)
    {

        int in = -1, out = -1;
        int count = 0;

        char i_location[128];
        char o_location[128];

        // for (int i = 0; i < MAX_ARGS; i++) {
        //     printf("%s \n", parsed[i]);
        // }

        while (parsed[count] != NULL)
        {
            // printf("%s\n", parsed[count]);
            if (strcmp(parsed[count], "<") == 0)
            {
                in = count;

                parsed[count] = "\0";

                strcpy(i_location, parsed[count + 1]);

                // Vi må muligens ha med denne også.
                parsed[count + 1] = "\0";
            }
            if (strcmp(parsed[count], ">") == 0)
            {
                out = count;
                parsed[count] = "\0";

                strcpy(o_location, parsed[count + 1]);
                // Vi må muligens ha med denne også.
                parsed[count + 1] = "\0";
            }
            count++;
        }
        // Gjør sånn at args-arrayet KUN har kommandoen som skal bli utført
        fix_command_args();

        if (in < 0 && out < 0)
        {
            return 1;
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

        // TODO execute kommando etter io redirection.
        if (execvp(*args, args) < 0)
        {
            error(0, errno, "Failed run command.");
            //unix_err(errno);
        }
        exit(0);
        // sende inn riktig parsede argumenter.

        // exite
    }

    else
    {
        int status;
        waitpid(pid, &status, 0);
        printf("Exit status [%s]: %i\n", args_str, status);
        return status;
    }
}

int main(int argc, char const *argv[])
{
    init_shell();
    // stdout_redirection();
    while (1)
    {
        printDir();
        // exec_command();
        process_input();
    }

    return 0;
}