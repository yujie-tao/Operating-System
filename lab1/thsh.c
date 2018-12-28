/* Written by: Yujie Tao
 * Onyen: yujiet
 *
 * This is shell, supports changing directories, redirection, scripting
 * To compile code: gcc ex2.c -o ex2 -g
 *   
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>


#include <sys/wait.h>
#include <sys/types.h>

#define MAX_INPUT 1024

//Count the size of the cmd after parsing
int countSize(char **cmdIn){
  int size = 0;
  while(cmdIn[size] != NULL){
    size++;
  }
  return size;
}

//Parse space, return value size equals to 1 if there is no space
char **spaceParser(char cmd[]){
  char *splitCmd;
  char **cmdIn = malloc(MAX_INPUT*sizeof(char*));

  splitCmd = strtok(cmd, " '\n");
  int num = 0;

  while(splitCmd!=NULL){
    cmdIn[num] = splitCmd;
    splitCmd = strtok(NULL, " '\n");
    num++;
  }
  return cmdIn;
}

//Parse '|', return value size equals to 1 if there is no '|'
char **pipeParser(char cmd[]){
  char *splitCmd;
  char **cmdIn = malloc(MAX_INPUT*sizeof(char*));

  splitCmd = strtok(cmd,"|");
  int num = 0;

  while(splitCmd != NULL){
    cmdIn[num] = splitCmd;
    splitCmd = strtok(NULL, "|\n");
    num++;
  }

  return cmdIn;
}

//Parse '>', return value size equals to 1 if there is no '>'
char **rredirectParser(char cmd[]){
  char *splitCmd;
  char ** cmdIn = malloc(MAX_INPUT*sizeof(char*));

  splitCmd = strtok(cmd, ">");
  int num = 0;

  while(splitCmd!=NULL){
    cmdIn[num] = splitCmd;
    splitCmd = strtok(NULL,"> \n");
    num ++;
  }
  return cmdIn;
}

//Parse '<', return value size equals to 1 if there is no '<'
char **lredirectParser(char cmd[]){
  char *splitCmd;
  char ** cmdIn = malloc(MAX_INPUT*sizeof(char*));

  splitCmd = strtok(cmd, "<");
  int num = 0;

  while(splitCmd!=NULL){
    cmdIn[num] = splitCmd;
    splitCmd = strtok(NULL,"< \n");
    num ++;
  }
  return cmdIn;
}

//Right redirection parser for command with in pipe
char **rredirectParserForPipe(char *cmd[]){
  char ** cmdIn = malloc(MAX_INPUT*sizeof(char*));
  int i = 0;
  int k = 0;
  while(cmd[i]!=NULL){
    if(strcmp(cmd[i],">")!=0){
      cmdIn[k] = cmd[i];
      i++;
      k++;
    }else{
      i++;
    }
  }
  return cmdIn;
}

//Left redirection parser for command with in pipe
char **lredirectParserForPipe(char *cmd[]){
  char ** cmdIn = malloc(MAX_INPUT*sizeof(char*));
  int i = 0;
  int k = 0;
  while(cmd[i]!=NULL){
    if(strcmp(cmd[i],"<")!=0){
      cmdIn[k] = cmd[i];
      i++;
      k++;
    }else{
      i++;
    }
  }

  return cmdIn;
}

//Print out current directory
void showDir(){
     char cwd[MAX_INPUT];
     if (getcwd(cwd, sizeof(cwd)) != NULL) {
      char *splitDir;
      char *curDir[MAX_INPUT];
      splitDir = strtok (cwd,"/");

      int num2 = 0;
  
      while (splitDir != NULL){
        curDir[num2] = splitDir;
        splitDir = strtok (NULL, "/");
        num2++;
      }

      write(0, "[", 1);
      write(0, curDir[num2-1], strlen(curDir[num2-1]));
      write(0, "] ",2);
    } 
}

//Show debug message when the first command is '-d'
void debugMsg(char args[], int status, int exitStatus){
  if(status == 1){
    write(0,"RUNNING: ",9);
    write(0, args, strlen(args)-1);
    write(0,"\n", 1); 
  }

  if(status == 0){
    write(0,"ENDED: ",7);
    write(0, args, strlen(args)-1);
    write(0,"(ret=", 5);
    printf("%d", exitStatus);
    fflush(stdout);
    write(0,")",1);
    write(0,"\n", 1); 
  }
}

//Implement change directory, 'cd' only goes to the home directory
int cdCmd(char *args[],int num){
  int exit_status = 0;

  if(num == 1){
    exit_status = chdir(getenv("HOME"));
  }else{
    exit_status = chdir(args[1]);
  }

  return exit_status;
}

//Execute one single command, handle the case when command not found
int executeCmd(char *args[],int num){
    int exit_status = 0;
    pid_t pid;
    int status;

    pid = fork();

    if(pid < 0){
      //unsuccessful creation of the children
      write(1, "unsuccessul creation of children", 30);
      exit(EXIT_FAILURE);
    }else if (pid == 0){
      //newly created child process 
      char *argv[num+1];
      for(int i = 0; i<num; i++){
        argv[i] = args[i];
      }

      argv[num] = NULL;
      exit_status = execvp(argv[0], argv);
      if(exit_status != 0){
        printf("%s\n","Command Not Found");
        return exit_status;
      }

    }else{
      //return to parent process, value process ID of newly created child process
      waitpid(pid, &status, 0);
    }

    return exit_status;
  }

//Implement redirection for pipe command
int executeRRedirForPipe(char** cmd){
  char* file = cmd[countSize(cmd)-1];
  int fd = open(file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  
  dup2(fd, 1);
  dup2(fd, 2);

  close(fd);
  close(fd);

  char *cmdOut[countSize(cmd)];
  for(int i = 0; i < countSize(cmd)-1;i++){
    cmdOut[i] = cmd[i];
  }
  
  cmdOut[countSize(cmd)-1] = NULL;

  execvp(cmdOut[0],cmdOut);
  
  return 0;
}

//Implement redirection for pipe command
int executeLRedirForPipe(char** cmd){
   char* file = cmd[countSize(cmd)-1];
   int fd = open(file, O_RDONLY,0);
   if(fd < 0){
      printf("%s\n","File not found");
      return (EXIT_FAILURE);
    }

    dup2(fd,0);
    close(fd);

    char** noSpace = spaceParser(cmd[0]);
    int num = countSize(noSpace);
    char *cmdOut[num+1];

    for(int i =0; i<num; i++){
      cmdOut[i] = noSpace[i];
    }

    cmdOut[num] = NULL;

    execvp(cmdOut[0],cmdOut);
    return 0;
}

//For redirection command without pipe, do fork, wait
int executeLRedir(char** cmd){
  pid_t pid = fork();
  char* file = cmd[countSize(cmd)-1];
  int status;

  if(pid < 0){
    write(1, "unsuccessul creation of children", 30);
    return (EXIT_FAILURE);
  }else if(pid == 0){
    int fd = open(file, O_RDONLY,0);
    if(fd < 0){
      printf("%s\n","File not found");
      return (EXIT_FAILURE);
    }
    dup2(fd,0);
    close(fd);

    char** noSpace = spaceParser(cmd[0]);
    int num = countSize(noSpace);
    char *cmdOut[num+1];

    for(int i =0; i<num; i++){
      cmdOut[i] = noSpace[i];
    }

    cmdOut[num] = NULL;

    execvp(cmdOut[0],cmdOut);

  }else{
    waitpid(pid, &status, 0);
  }

  return 0;
}

//For redirection command without pipe, do fork, wait
int executeRRedir(char** cmd){
  pid_t pid = fork();
  char* file = cmd[1];
  int status;

  if(pid < 0){
  //unsuccessful creation of the children
    write(1, "unsuccessul creation of children", 30);
    exit(EXIT_FAILURE);
  } else if(pid == 0){

    int fd = open(file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    dup2(fd, 1);
    dup2(fd, 2);

    close(fd);
    close(fd);

    char** noLRedirect = lredirectParser(cmd[0]);
    if(countSize(noLRedirect)>1){
      executeLRedirForPipe(noLRedirect);
    }else{
      char** noSpace = spaceParser(cmd[0]);
      int num = countSize(noSpace);
      char *cmdOut[num+1];

      for(int i =0; i<num; i++){
        cmdOut[i] = noSpace[i];
      }
      cmdOut[num] = NULL;
      execvp(cmdOut[0],cmdOut);

    }

  }else{
    waitpid(pid, &status, 0);
  }

   return 0;
}

//Execute command in one pipe, do fork, wait
int executeOnePipe(int input, int output, char *cmd[],int debug){
  int i = 0;
  int rdir = 0;
  int ldir = 0;
  char** noRRedirect = NULL;
  char** noLRedirect = NULL;

  while(cmd[i]!= NULL){
    if(strcmp(cmd[i],">") == 0){
      rdir = 1;
    }
    if(strcmp(cmd[i],"<") == 0){
      ldir = 1;
    }
    i++;
  }

  if(rdir == 1){
    noRRedirect = rredirectParserForPipe(cmd);
  }
  
  if(ldir == 1){
    noLRedirect = lredirectParserForPipe(cmd);
  }


  pid_t pid = fork();
  int status;

  if(pid < 0){
    write(1, "unsuccessul creation of children", 30);
    exit(EXIT_FAILURE);
  }else if(pid == 0){
    if(input != 0){
      dup2(input,STDIN_FILENO);
      close(input);
    }
    if(output != 0){
      dup2(output,STDOUT_FILENO);
      close(output);
    }

    if(rdir == 1 || ldir == 1){
      if(ldir == 1){
        executeLRedirForPipe(noLRedirect);
      }
      if(rdir == 1){   
        executeRRedirForPipe(noRRedirect);
      }
    }else{
      execvp(cmd[0],cmd);
    }

  }else{
    waitpid(pid, &status, 0);
  }

  return 0;
}

//Execute the whole pipe, put the previous output as input for the folloing one
int executePipe(char *cmd[][MAX_INPUT], int size, int debug){
  int i = 0;
  int rdir = 0;
  int ldir = 0;
  char** noRRedirect = NULL;
  char** noLRedirect = NULL;


  while(cmd[size-1][i]!= NULL){
    if(strcmp(cmd[size-1][i],">") == 0){
      rdir = 1;
    }
    if(strcmp(cmd[size-1][i],"<") == 0){
      ldir = 1;
    }
    i++;
  }

  if(rdir == 1){
    noRRedirect = rredirectParserForPipe(cmd[size-1]);
  }
  
  if(ldir == 1){
    noLRedirect = lredirectParserForPipe(cmd[size-1]);
  }

  pid_t pid;
  pid = fork();
  int status;

  if(pid < 0){
    write(1, "unsuccessul creation of children", 30);
    exit(EXIT_FAILURE);
  }else if(pid == 0){
    int input, fd[2];

    input = 0;
    for(int i = 0; i < size-1; i++){
      pipe(fd);
      executeOnePipe(input, fd[1], cmd[i],debug);
      close(fd[1]);
      input = fd[0];
    }

    if(input != 0){
      dup2(input, 0);
      close(input);
    }


    if(rdir == 1 || ldir == 1){
      if(ldir == 1){
        executeLRedirForPipe(noLRedirect);
      }
      if(rdir == 1){   
        executeRRedirForPipe(noRRedirect);
      }
    }else{
       execvp(cmd[size-1][0], cmd[size-1]);
    }

  }else{
    waitpid(pid, &status, 0);
  }

  return 0;
}

//Support scripting
int executeScript(char* file){
  int fd = open(file, O_RDONLY);
  if(fd < 0){
    printf("%s\n","error");
    exit(1);
  }
  pid_t pid;
  pid = fork();
  int status;

  if(pid < 0){
    write(1, "unsuccessul creation of children", 30);
    exit(EXIT_FAILURE);
  }else if (pid == 0){
    dup2(fd, 0);
    close(fd);
  }else{
    waitpid(pid, &status,0);
  }

  FILE *fp;
  int lines = 0;
  char c;
  fp = fopen(file,"r");
  for(c = getc(fp); c != EOF; c = getc(fp)){
    if(c=='\n'){
      lines++;
    }
  }

  return lines;
}

//Main function
int main (int argc, char ** argv, char **envp) {

  int finished = 0;
  char *prompt = "thsh> ";
  char cmd[MAX_INPUT];
  char cmddup[MAX_INPUT];
  int debug = 0;
  int first = 1;
  int interactive = 1;
  int file_lines;

  if(argv[1]!=NULL){
    close(0);
    file_lines = executeScript(argv[1]);
    if(file_lines== 0){
      return 0;
    }
    interactive = 0;
  }


  while (!finished) {
    char *cursor;
    char last_char;
    int rv;
    int count;

    if(interactive == 1){
       showDir();
       write(1, prompt, strlen(prompt));
    }

    // read and parse the input
    for(rv = 1, count = 0, cursor = cmd, last_char = 1;
      rv && (++count < (MAX_INPUT-1)) && (last_char != '\n');
      cursor++) { 

      rv = read(0, cursor, 1);
      last_char = *cursor;
    } 

    *cursor = '\0';

    if (!rv) { 
      finished = 1;
      break;
    }

    if(cmd[0] == '#'){
      continue;
    }

    for(int i=0; i<MAX_INPUT;i++){
      cmddup[i] = cmd[i];
    }

    //Check whether there is pipe in the command
    char** noPipe = pipeParser(cmd);
    if(countSize(noPipe) == 1){

      //If it is pipe command, check whether there is redirection
      char** noRRedirect = rredirectParser(cmd);
      if(countSize(noRRedirect) > 1){
        if(debug == 1){
          debugMsg(cmddup,1,0);
        }
        int exit_status = executeRRedir(noRRedirect);
        if(debug == 1){
          debugMsg(cmddup,0,exit_status);
        }
        continue;
      }

      char** noLRedirect = lredirectParser(cmd);
      if(countSize(noLRedirect) > 1){
        if(debug == 1){
          debugMsg(cmddup,1,0);
        }
        int exit_status = executeLRedir(noLRedirect);
        if(debug == 1){
          debugMsg(cmddup,0,exit_status);
        }
        continue;
      }

      //Parse all space
      char** noSpace = spaceParser(cmd);
      char *cmdIn[MAX_INPUT];

      int size = countSize(noSpace);

      for(int i=0;i < size; i++){
        cmdIn[i] = noSpace[i];
      }

      //Check for whether need to set to debuging mode
      if(strcmp(cmdIn[0],"-d") == 0 && first == 1){
          debug = 1;
        }else{

          //Special command 'goheels'
          if(strcmp(cmdIn[0],"goheels")==0 && size ==1){
            if(debug == 1){
              debugMsg(cmddup,1,0);
            }
            printf("      __  _       _  _     _  \n");
            printf("     /__ / \\ |_| |_ |_ |  (_  \n");
            printf("     \\_| \\_/ | | |_ |_ |_ __) \n"); 
            if(debug == 1){
              debugMsg(cmddup,0,0);
            }
            continue;                          
          }

          //Support 'exit'
          if(strcmp(cmdIn[0],"exit") == 0 && size ==1){
            if(debug == 1){
              debugMsg(cmddup,1,0);
              debugMsg(cmddup,0,0);
            }
            return 0;
          }else if(strcmp(cmdIn[0], "cd") == 0){
            if(debug == 1){
              debugMsg(cmddup,1,0);
            }
            int exit_status = cdCmd(cmdIn, size);
            if(debug == 1){
              debugMsg(cmddup,0,exit_status);
            }
          }else{
            if(debug == 1){
              debugMsg(cmddup,1,0);
            }
            int exit_status = executeCmd(cmdIn, size);
            if(debug == 1){
              debugMsg(cmddup,0,exit_status);
            }
          } 
        }

    }else{

      if(debug == 1){
          debugMsg(cmddup,1,0);
        }

      //If it is pipe command, restructure the command into 2D array
      char *pipCommand[countSize(noPipe)][MAX_INPUT];
      
      for(int i = 0; i < countSize(noPipe); i++){
        char** noPipeSpace = spaceParser(noPipe[i]);

        for(int k = 0; k < countSize(noPipeSpace); k++){
          pipCommand[i][k] = noPipeSpace[k];
        }
      }

      int exit_status = executePipe(pipCommand,countSize(noPipe),debug);

      if(debug == 1){
        debugMsg(cmddup,0,exit_status);
      }

   }

 }

  return 0;
}
