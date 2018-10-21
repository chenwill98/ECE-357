//William Chen
//ECE-357 Operating Systems Program #3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#define GREEN "\x1b[32m"
#define RESET "\x1b[0m"

char err_msg[1024];

int process(FILE *file);
void process_line(char **args, char **redirs, char *line);
int run(char **args, char **redirs);

int main(int argc, char **argv) {
  FILE *file;
  if (argc == 1) {
    file = stdin;
    process(file);
  } else if (argc != 0) {
    if ((file = fopen(argv[1], "r")) != NULL) {
      process(file);
    } else {
      snprintf(err_msg, sizeof(err_msg), "Error reading %s", argv[1]);
      perror(err_msg);
      return -1;
    }
  }
  return 0;
}

int process(FILE *file) {
  char * line = NULL;
  char ** args = malloc(BUFSIZ);
  char ** redirs = malloc(BUFSIZ);
  ssize_t buff = 0;
  int status = 1;

  while (status > 0) {
    printf(GREEN "> " RESET);
    getline(&line, &buff, file);
    process_line(args, redirs, line);
    printf("%s\n", redirs[0]);
    status = run(args, redirs);
  }
  free(args);
  free(redirs);
}

void process_line(char ** args, char ** redirs, char * line) {
  int i = 0;
  char * token = strtok(line, " \r\n");

  while (token != NULL) {
    printf("%c\n", token[0]);
    if (token[0] == '>' || token[0] == '<' || (token[0] == '2' && token[1] == '>')) {
      redirs[i] = token;
    } else {
      args[i] = token;
    }
    token = strtok(NULL, " \r\n");
    i++;
  }
}

int run(char **args, char **redirs) {
  return 0;
}
