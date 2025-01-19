#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>

#define clear() printf("\033[H\033[J") // clear the terminal window

int Prompt();
void TakeInput(char*buffer, size_t size);
int ExecuteCommand(const char *command);
void handleRedirectionAndPiping(const char *command); // > < , | , ;
void handleRedirection(const char *command);
void handlePiping(const char * command);

void Prompt()
{
    char *username = getenv("USER");
    char hostname[256];
    char *current_directory = getcwd(NULL, 0);

    gethostname(hostname, sizeof(hostname));
    printf("%s@%s:%s$ ", username, hostname, current_directory);

    free(current_directory);
}

void TakeInput(char* buffer, size_t size)
{
    printf("TEAM5$:");
    fflush(stdout);

    fgets(buffer, size, stdin); 
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove the newline character \0
}

int ExecuteCommand(const char *command) // ls&&cat file
{
    if(strchr(command, ';')) // handles the ; command // ls > result.txt 
    {
        char *cmd = strdup(command);
        char *separator = strchr(cmd, ';');  //get the index of the separator ; and store it in separator // ls\0 cat file.txt\0

        *separator = '\0';
        separator++;

        ExecuteCommand(cmd); // Execute the first command before ;
        ExecuteCommand(separator); // Execute the second command after ;
        return 0;
    }

    if (strchr(command, '>') || strchr(command, '<') || strchr(command, '|'))
    {
        handleRedirectionAndPiping(command); // Call a helper function to handle this
        return 0;
    }

        char *args[100]; // pointer to array of string => arguments to send to exec function
        char *token = strtok(strdup(command), " "); // split to command to array of string (split using spaces) ls -l 
        
        
        int i = 0;

        while (token != NULL && i < 99) // adding each token to the args array
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // making sure the last token is null to be end marker // args = [ls,-l]

        // skipping fork if it is a built-in command (cd)
        if (strcmp(args[0], "cd") == 0)
        {
            change_dir(args);
            return;
        }
    int pid = fork(); 

    if( pid < 0 )
    {
        return -1; //Forking Failed
    }

    if( pid == 0 )
    {
        // child proccess executing the command
        if (execvp(args[0], args) < 0) // exec()
        {
            perror("Execution failed");
            _exit(1);
        }
    }

    wait(NULL); // Parent Proccess waiting for child to terminate
}

void handlePiping(const char *command)
{
    char *cmd = strdup(command); // makes a duplicate for the command in cmd
    char *pipeCmd = strchr(cmd, '|'); // return the pointer of index of '|' char if found 

    if (pipeCmd) // if index of '|' was found 
    {
        *pipeCmd = '\0';  // this splits the command from firstcommand | secondcommand => firstcommand \0 secondcommand
        pipeCmd++;        // Move to the second command // ls \0 grep

        // Create a pipe
        int pipefd[2];
        if (pipe(pipefd) < 0)
        {
            perror("Pipe creation failed");
            _exit(1);
        }

        // First process writes to the pipe = firstcommand 
        if (fork() == 0)
        {
            close(pipefd[0]);             // Close unused read end 
            dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
            close(pipefd[1]);

            ExecuteCommand(cmd); // Execute the first command start pointer = cmd whic
            _exit(0); 
        }


        // Second process reads from the pipe = secondcommand
        if (fork() == 0)
        {
            close(pipefd[1]);             // Close unused write end
            dup2(pipefd[0], STDIN_FILENO); // Redirect stdin from pipe
            close(pipefd[0]);

            ExecuteCommand(pipeCmd); // Execute the second command
            _exit(0);
        }

        close(pipefd[0]);
        close(pipefd[1]);

        // Wait for both child processes
        wait(NULL);
        wait(NULL);
    }
}

void handleRedirection(const char *command)
{
    char *cmd = strdup(command); // makes a duplicate for the command in cmd
    char *redirection = strchr(cmd, '>');  // return the pointer of index of '>' char if found ls >> file
    char *errorcheck = redirection;

    if(fork() == 0) 
    {

        if (redirection) // if index of '>' was found 
        {
            int fd;
            if(*(redirection + 1) == '>')
            {
                *redirection = '\0';  // this splits the command from firstcommand > file => firstcommand \0 file
                redirection++;
                redirection++; 
                redirection++;  

                fd = open(redirection, O_WRONLY | O_CREAT | O_APPEND, 0777);
            }
            else
            {
                *redirection = '\0';  // this splits the command from firstcommand > file => firstcommand \0 file
                redirection++;
                redirection++;  
                fd = open(redirection, O_WRONLY | O_CREAT | O_TRUNC, 0777);

            }

            // Create a file

            if (fd < 0)
            {
                perror("File open failed");
                return;
            }
            if(*(errorcheck - 1) == '2')
            {
                // Redirect stderror to the file
                dup2(fd, STDERR_FILENO);
                close(fd);
            }
            else
            {
                // Redirect stdout to the file
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            

            ExecuteCommand(cmd); // Execute the first command
            exit(0);
        }

    }


}

void handleRedirectionAndPiping(const char *command)

{
    if (strchr(command, '|'))
    {
        handlePiping(command);
    }
    else
    {
        handleRedirection(command);
    }
}

int main(int argc, char *argv[])
{
    

    while(1) // Main Loop
    {
        prompt();
        char command[200]; // string

        TakeInput(command, sizeof(command));

        if(strcmp(command, "exit") == 0) // stop looping and terminate the shell when the the inputis "exit"
        {
            printf("Thank You For Using Our Shell \n");
            sleep(1);
            return 0;
        }

        if(strlen(command) > 0)
        {
            ExecuteCommand(command);
        } 
        
    } 
    return 0;
}