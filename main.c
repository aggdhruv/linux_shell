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
#include <fcntl.h>

void printShellLine() {
    printf("> ");
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
    chdir(homeDirectory);
  }

  else {
    chdir(args[1]);
  }
}

void pwdExecute() {
  char * cwd;
  cwd = malloc(sizeof(char) * 1024);
  printf("%s \n", getcwd(cwd, 1024));
}

void waitExecute() {
  while(wait(NULL) != -1) {}
}

void exitExecute() {
  exit(0);
}

void helpExecute() {
  printf("The built in commands in the shell are:\n");
  printf("cd \npwd \nwait \nexit \nhelp\n");
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
    printf("Non built in command\n");
  }
}

int main( int argc, char ** argv ) {
  
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
  return 0;
}
