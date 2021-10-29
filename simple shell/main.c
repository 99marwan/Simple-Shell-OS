#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>


//this is the id of the log file.
int fd;

//the input of user.
char* s;
char d;

//array of arguments after splitting the input.
char *op[500];
//flag of waiting on child process.
bool flag = false;
//id of process
pid_t pid;


//function that signal "SIGCHLD" call.
void proc_exit()
{
    //this message will be written in the log file.
    char msg[]="Child process was terminated\n";
    //check if file is exist or not
    if(fd != 1)
    {
        //writing message in log file.
        write(fd,msg,strlen(msg));
    }

    //this part prevent the process of being zombie.
    int wstat;
    pid_t	pid;
    pid = wait3 (&wstat, WNOHANG, (struct rusage *)NULL );
    if (pid == 0)
        return;
    else if (pid == -1)
        return;
}

void splitInput()
{
    int i =0;
    //the first pointer
    char *token = strtok(s," \n");

    //iterate on the splitted word
    while (token != NULL)
    {
        //if there is & make waiting flag equal true and remove it.
        if (!strcmp(token,"&"))
        {
            flag= true;
            token = '\0';
            //save arguments in pointer array
            op[i++]= token ;

        }
        else
            op[i++]=token;
        //get next pointer.
        token = strtok(NULL, " ");
    }
    //saving null at at the end.
    op[i] = token;
}

void forkProcess()
{
    pid_t pid = fork();

    //fork failed
    if (pid == -1)
    {
        printf("\nForking a child is failed...\n");

    }
    //child created
    else if (pid == 0)
    {
        //execute command and arguments.
        int x = execvp(op[0], op);

        //if execution fail
        if (x < 0)
        {
            printf("\nThis command can't be executed...\n");
            //exit with problem
            exit(x);
        }
        //exit normally
        exit(0);
    }
    //if id is belongs to parent
    else
    {
        // waiting for child to terminate if there is no &
        if(!flag)
            waitpid(pid, NULL, 0);

        if(flag)
                flag=false;
    }
}


int main ()
{
    //signal that call function of writing in log file after
    //termination of process
    signal (SIGCHLD, proc_exit);

    //open file if exist and create it if it is not exist.
    fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);


    while(1)
    {
        //scanning input.

        scanf("%m[^\n]",&s);
        scanf("%c",&d);

        //if user press enter without type anything.
        if(!s)
            continue;

        //if input is exit close log file and exit console.
        if(!strcmp(s,"exit"))
        {
            close(fd);
            exit(0);
        }

        //split input and save it in array of pointers.
        splitInput();

        //fork a process to execute command.
        forkProcess();
    }
}


