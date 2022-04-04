/* main.c
 * ----------------------------------------------------------
 *  CS350
 *  Midterm Programming Assignment
 *
 *  Purpose:  - Use Linux programming environment.
 *            - Review process creation and management
 * ----------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>	
// #include <fcntl.h>

void printShellLine() {
    printf("> ");
}

void handleError() {
  char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}

int getArgsLength(char ** args) {
  int i=0;
  while(args[i]!=NULL) {
    i++;
  }
  return i;
}

char * getCommand() {
        int pos = 0;
        char * command = (char *)malloc(sizeof(char) * 512);
        char c= getchar();
        while (c != '\n' && c != EOF){
                command[pos] = c;
                pos++;
                c = getchar();
        }
        return command;
}

void cdExecute(char ** args) {

  if(args[1] == NULL) {
    char * homeDirectory;
    homeDirectory = getenv("HOME");
    int status = chdir(homeDirectory);
    if (status == -1) {
      handleError();
    }
  }

  else {
    int status = chdir(args[1]);
    if (status == -1) {
      handleError();
    }
  }
}

void pwdExecute() {
  char cwd[512];
  getcwd(cwd, sizeof(cwd));
  char *cwd_w_newline = malloc(strlen(cwd) + 2);
  char n = '\n';
  strcpy(cwd_w_newline, cwd);
  cwd_w_newline[strlen(cwd)] = n;
  cwd_w_newline[strlen(cwd) + 1] = '\0';
  write(STDOUT_FILENO, cwd_w_newline, strlen(cwd_w_newline));
  free(cwd_w_newline);
}

void waitExecute() {
  while(wait(NULL) != -1) {}
}

void exitExecute() {
  exit(0);
}

void helpExecute() {
  printf("cd\npwd\nwait\nexit\nhelp\n");
}

char ** processString(char* command) {
    int pos = 0;
    int num = 64;
    char ** tokens = malloc(sizeof(char *) * num);
    char lim[2] = " ";
    char * token = strtok(command, lim);
    while (token != NULL){
      tokens[pos] = token;
      token = strtok(NULL, lim);
      pos++;
    }
    tokens[pos] = NULL;
    return tokens;
}

int builtInCommand(char ** args) {
  int numBuiltInCommands = 5;
  char * listOfBuiltInCommands[numBuiltInCommands];
  int commandCase = -1;

  listOfBuiltInCommands[0] = "cd";
  listOfBuiltInCommands[1] = "pwd";
  listOfBuiltInCommands[2] = "wait";
  listOfBuiltInCommands[3] = "exit";
  listOfBuiltInCommands[4] = "help";

  for (int i=0; i< numBuiltInCommands; i++) {
    if (strcmp(listOfBuiltInCommands[i], args[0]) == 0) {
      commandCase = i;
      break;
    }
  }

  switch(commandCase) {
    case 0: cdExecute(args);
            return 1;
            break;
    case 1: pwdExecute();
            return 1;
            break;
    case 2: waitExecute();
            return 1;
            break;
    case 3: exitExecute();
            return 1;
            break;
    case 4: helpExecute();
            return 1;
            break;
    default: return 0;
            break;

  }

}

void startProcess(char ** args, int childStatus) {

    if (childStatus == 1) {
      pid_t p_child = fork();

      if(p_child == 0) {
        int status = execvp(args[0], args);
        if (status == -1) {
          handleError();
        }
        exit(0);
      }
      else if (p_child == -1) {
        handleError();
      }
      else {
        return;
      }
    }
    else {
      pid_t p_pid = fork(); 
      if (p_pid == 0) {
        int status = execvp(args[0], args);
        if (status == -1) {
          handleError();
        }
        // exit(0);
      }

      else if (p_pid == -1) {
        handleError();
        return;
      } 

      else {
        wait(NULL); 
        return;
      }
  }
}

void executeCommand(char ** args) {

  if (args[0] == NULL) {
    return;
  }

  if(builtInCommand(args)) {
    return;
  }

  else {
    int arr = getArgsLength(args);
    if(strcmp("&", args[arr-1])==0) {
      args[arr-1] = '\0';
      startProcess(args, 1);
    }
    else {
      startProcess(args, 0);
    }
  }
}

void batchMode(char ** argv) {
  FILE *file;
  char *filename = argv[1];
  file = fopen(filename, "r");

  if (file==NULL) {
    handleError();
    return;
  }

  char * command;
  command = (char *)malloc(sizeof(char) * 1024);
  while (fscanf(file, "%[^\n] ", command) != EOF) {
      char ** tokens = processString(command);
      executeCommand(tokens);
    }
}

int main( int argc, char ** argv ) {

  if (argc == 2) {
    batchMode(argv);
  }

  else {
  
    char * command;
    while(1) {
      printShellLine();
      command = getCommand();

      if( strcmp(command, "") == 0) {
        continue;
      }

      char ** tokens = processString(command);
      executeCommand(tokens);

    }

  }
  return 0;
}
