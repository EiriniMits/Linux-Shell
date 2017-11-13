#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <sys/time.h>
#include <signal.h>

#define MAX_BUFFER 1000
#define MAX_ARGS 64
#define SEPARATORS " \t\n"
#define FALSE 0
#define TRUE !FALSE

# Eirini Mitsopoulou

extern  int alphasort();
char pathname[MAXPATHLEN];
int pids[MAX_BUFFER];


int main (int argc, char ** argv)
{
    char linebuf[MAX_BUFFER];
    char cmndbuf[MAX_BUFFER];
    char * args[MAX_ARGS];
    char prompt[MAX_BUFFER];
    char exp[MAX_BUFFER];
    char exp2[MAX_BUFFER];
    char exp3[MAX_BUFFER];
    int backround=0;
    struct direct **files;

    if (getcwd(pathname,sizeof(pathname)) == NULL )
    {
        printf("Error getting path\n");
        exit(0);
    }
    strcpy(prompt, "MYSHELL:");
    strcat(prompt,pathname);
    strcat(prompt,"$ ");
    fputs (prompt, stdout);
    fflush(stdout);
    while (!feof(stdin)) {

    // get command line from input

    if (fgets(linebuf, MAX_BUFFER, stdin )) { // read a line

        char *token;         //split command into separate strings
        token = strtok(linebuf,SEPARATORS);
        int i=0;
        while(token!=NULL){
            args[i]=token;
            token = strtok(NULL,SEPARATORS);
            i++;
        }
        args[i]=NULL;
        if(strcmp(args[i-1],"&")==0){
            backround=1;
            args[i-1]=NULL;
        }

        if (args[0]) {
            cmndbuf[0] = 0;
            if (!strcmp(args[0],"clear")) {  // "clear" command
                strcpy(cmndbuf, "clear");
                if (cmndbuf[0])
                  system(cmndbuf);
                fputs (prompt, stdout);
                fflush(stdout);
            } else
            if (!strcmp(args[0],"cd"))
            {
                int ret = chdir(args[1]);
                if(args[1]==NULL){// if user write cd
                        chdir(getenv("HOME"));
                        strcpy(prompt, "MYSHELL:~");
                        strcat(prompt,"$ ");
                        fputs (prompt, stdout);
                        fflush(stdout);
                }
                else if(ret==0){
                if (getcwd(pathname,sizeof(pathname)) == NULL )
                { printf("Error getting path\n");
								exit(0);
                }
                strcpy(prompt, "MYSHELL:");
                strcat(prompt,pathname);
                strcat(prompt,"$ ");
                fputs (prompt, stdout);
                fflush(stdout);}
                else{
                    perror("Command failed");
                    fputs (prompt, stdout);
                    fflush(stdout);
                }
            }
            else if (!strcmp(args[0],"exit")) {   // "exit" command
                break;
            }
            else {
                 pid_t pid, wpid;
                  int status;

                  pid = fork();
                  if (pid == 0) {
                    // Child process
                    int i;
                    char input[64],output[64];
                    for(i=0;args[i]!='\0';i++)
                    {
                        if(strcmp(args[i],"<")==0)
                        {
                            strcpy(input,args[i+1]);
                            int fd0;
                            if ((fd0 = open(input, O_RDONLY, 0666)) < 0) {
                                perror("Couldn't open input file");
                                exit(0);
                            }
                            dup2(fd0, 0);
                            close(fd0);
                            args[i]=NULL;
                            break;
                        }

                        if(strcmp(args[i],">")==0)
                        {
                            strcpy(output,args[i+1]);
                            int fd1 ;
                            if ((fd1 = creat(output , 0644)) < 0) {
                                perror("Couldn't open the output file");
                                exit(0);
                            }
                            dup2(fd1, 1);
                            close(fd1);
                            args[i]=NULL;
                            break;
                        }

                        if(strcmp(args[i],">>")==0)
                        {
                            strcpy(output,args[i+1]);
                            int fd2 ;
                            if ((fd2 = open(output , O_WRONLY|O_APPEND|O_CREAT, 0666)) < 0) {
                                perror("Couldn't open the output file");
                                exit(0);
                            }
                            dup2(fd2, 1);
                            close(fd2);
                            args[i]=NULL;
                            break;
                        }

                        if(strcmp(args[i],"|")==0)
                        {
                            char *cmd1[10];
                            char *cmd2[10];
                                int j,k=0;
                                for(j=0;j<10;j++){
                                    if(strcmp(args[j],"|")==0){
                                       break;}
                                    else{
                                        cmd1[j] = args[j];}
                                }
                                cmd1[j] = 0;


                                for(j=i+1;args[j]!='\0';j++){
                                    cmd2[k] = args[j];
                                    k++; }
                                cmd2[k] = 0;


                            int pid1, status;
                            int pipefd[2];
                            switch ((pid1 = fork())) {
                            case -1:
                                perror("fork");
                                break;
                            case 0:
                                if (pipe(pipefd)) {
                                perror("pipe");
                                exit(127);
                                }
                                // since we have TWO commands to exec we need to have two processes
                                switch (fork()) {
                                case -1:
                                    perror("fork");
                                    exit(127);
                                case 0:
                                    close(pipefd[0]);
                                    dup2(pipefd[1], 1);
                                    close(pipefd[1]);
                                    execvp(cmd1[0],cmd1);
                                    perror("exec");
                                    exit(126);
                                default:
                                    close(pipefd[1]);
                                    dup2(pipefd[0], 0);
                                    close(pipefd[0]);
                                    execvp(cmd2[0],cmd2);
                                    perror("exec");
                                    exit(125);
                                }
                                break;
                            default:

                                pid1 = wait(&status);

                        }
                        return(0);
                    }
                    if(strstr(args[i],"*.")!=NULL)
                    {
                            int count,r,status,bool = 1;
                            if(args[i][0]!='*') break; // make sure that the argument start with *
                            count =scandir(pathname, &files, 0, alphasort);
                            strcpy(exp, args[i]);
                            char *ps = exp;
                            memmove (exp, exp+1, strlen (exp+1) + 1);
                            int len = strlen(ps);
                            ps[len] = '*';
                            ps[len+1] = '\0';
                            for (r=1;r<count+1;++r){
                                     strcpy(exp2, files[r-1]->d_name);
                                     strcpy(exp3, exp2);
                                     int len = strlen(exp3);
                                        exp3[len] = '*';
                                        exp3[len+1] = '\0';
                                     if(strstr(exp3,ps)!=NULL){
                                            bool = 0;
                                            int pid2= fork();
                                            if(pid2==0){
                                                strcpy(input,exp2);
                                                int fd0;
                                                if ((fd0 = open(input, O_RDONLY, 0666)) < 0) {
                                                    perror("Couldn't open input file");
                                                    exit(0);
                                                }
                                                dup2(fd0, 0);
                                                close(fd0);
                                                args[i]=NULL;
                                                execvp(args[0], args);
                                            }
                                            else if (pid2 < 0) {
                                             // Error forking
                                               perror("lsh");
                                            }
                                            else{
                                                pid2 = wait(&status);
                                            }
                                    }
                            }
                            if(bool==1){
                                perror("lsh");
                            }
                           return 0;

                        }
                    }

                    if (execvp(args[0], args) == -1) {
                      printf("%s: command not found\n",args[0]);
                    }
                    exit(EXIT_FAILURE);
                  }
                  else if (pid < 0) {
                    // Error forking
                    perror("lsh");
                  }
                  else {
                    do {
                      wpid = waitpid(pid, &status, WUNTRACED);
                    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
                    if(backround==1){
                        int k;
                        for(k=0;k<1000;k++){
                            if(pids[k]==0){
                                    pids[k]=pid;
                                    break;
                            }
                        }
                        backround=0;
                     }
                  }

                 fputs (prompt, stdout);
                 fflush(stdout);               // pass command on to OS shell
                }
            }
        }
    }
    return 0;
}

