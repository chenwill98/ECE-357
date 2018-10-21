//William Chen
//ECE-357 Operating Systems Program #3
//Credit to Joey Bentivegna for helping me

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#define GREEN "\x1b[32m"
#define RESET "\x1b[0m"

char err_msg[1024];

int process(FILE *file);
void process_line(char **args, char **redirs, char *line);
int run(char **args, char **redirs);
void sh_pwd();
void sh_cd(char * arg);
int execute(char ** args, char ** redirs);
void redirect(char **red);
int io(char *path, int fd_new, int flags, mode_t mode);

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
    status = run(args, redirs);
  }
  free(args);
  free(redirs);
}

void process_line(char ** args, char ** redirs, char * line) {
  int r_index = 0, a_index = 0;
  char * token = strtok(line, " \r\n");

  while (token != NULL) {
    if (token[0] == '>' || token[0] == '<' || (token[0] == '2' && token[1] == '>')) {
      redirs[r_index] = token;
      r_index++;
    } else {
      args[a_index] = token;
      a_index++;
    }
    token = strtok(NULL, " \r\n");
  }
}

int run(char ** args, char ** redirs) {
  int status;
  if (args[0] == NULL) {
    fprintf(stderr, "Error: missing command\n");
  } else if (strstr(args[0], "#") || args[0] == NULL) {
    return 1;
  } else if (!strcmp(args[0], "pwd")) {
    sh_pwd();
    return 1;
  } else if (!strcmp(args[0], "cd")) {
    sh_cd(args[1]);
    return 1;
  } else if (!strcmp(args[0], "exit")) {
    exit(atoi(args[1]));
  } else {
    status = execute(args, redirs);
    return status;
  }
}

//Taken from the Open Group Base Specification
void sh_pwd() {
  long size;
  char * buff;
  char * val;

  size = pathconf(".", _PC_PATH_MAX);
  if ((buff = (char *)malloc((size_t)size)) != NULL)
    val = getcwd(buff, (size_t)size);

  if (val != NULL)
    printf("Current directory: %s\n", buff);
  else {
    snprintf(err_msg, sizeof(err_msg), "Error finding current directory");
    perror(err_msg);
  }
}

void sh_cd(char * arg) {
  if (arg == NULL) {
    fprintf(stderr, "Error: missing target directory\n");
  } else {
    if (chdir(arg) < 0) {
      snprintf(err_msg, sizeof(err_msg), "Error changing directory");
      perror(err_msg);
    }
  }
}

//Thanks to MIT for showing how to fork/exec
int execute(char ** args, char ** redirs) {
  pid_t pid;
  int status;
  printf("%s\n", args[0]);

  if (pid = fork() < 0) {
    snprintf(err_msg, sizeof(err_msg), "Error forking the process");
    perror(err_msg);
  } else if (pid == 0) {
    redirect(redirs);
    if (execvp(*args, args) < 0) {
      snprintf(err_msg, sizeof(err_msg), "Error executing %s", args[0]);
      perror(err_msg);
    }
    exit(1);
  } else {
    while (wait(&status) != pid);
  }
  return 1;
}

void redirect(char ** redirs) {
  int i = 0;
  char * red = redirs[0];
  if (redirs[1] == NULL) {

    if (red[0] == '2' && red[1] == '>' && red[2] == '>') {
      if (io(red + 3, 2, O_RDWR|O_APPEND|O_CREAT, 0666)) exit(1);
    }
    if (red[0] == '>' && red[1] == '>') {
      if (io(red + 2, 1, O_RDWR|O_APPEND|O_CREAT, 0666)) exit(1);
    }
    if (red[0] == '2' && red[1] == '>') {
      if (io(red + 2, 2, O_RDWR|O_TRUNC|O_CREAT, 0666)) exit(1);
    }
    if (red[0] == '>') {
      if (io(red + 1, 1, O_RDWR|O_TRUNC|O_CREAT, 0666)) exit(1);
    }
    if (red[0] == '<') {
      if (io(red + 1, 0, O_RDONLY, 0666)) exit(1);
    }
  } else {
    fprintf(stderr, "Error: cannot redirect file descriptor more than once\n");
  }
}

int io(char *path, int fd_new, int flags, mode_t mode) {
  int fd = open(path, flags, mode);
  if (fd < 0) {
    snprintf(err_msg, sizeof(err_msg), "Error opening %s", path);
    perror(err_msg);
    return 1;
  }
  if (dup2(fd, fd_new) < 0) {
    snprintf(err_msg, sizeof(err_msg), "Error duping %s", fd);
    perror(err_msg);
    return 1;
  }
  if (close(fd) < 0) {
    snprintf(err_msg, sizeof(err_msg), "Error closing %s", path);
    perror(err_msg);
    return 1;
  }
  return 0;
}
