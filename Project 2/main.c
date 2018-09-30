//William Chen
//ECE-357 Operating Systems Program #2
//Credit to Rayhan for helping me

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <getopt.h>
#include <stdint.h>
#include <grp.h>
#include <time.h>
#include <pwd.h>

extern int optind;
char err_msg[1024];

int dir_read(char * dir);
int print_out(char * filename, struct stat file_meta);

int main(int argc, char * argv[]) {
  char* dir;
  if (argc == 1) {
    dir = ".";
  } else if (argc == 2) {
    dir = argv[optind];
  } else {
    fprintf(stderr, "Error: too many arguments\n");
    printf("Proper format: ./search [directory]");
    return -1;
  }
  dir_read(dir);
  return 0;
}

int dir_read(char * direc) {
  DIR * dirp = opendir(direc);
  struct dirent * dir;
  if (!dirp) {
    snprintf(err_msg, sizeof(err_msg), "Error executing opendir(%s)", direc);
    perror(err_msg);
    return -1;
  }
  while (dir = readdir(dirp)) {
    int errno = 0;
    char file[257];

    if (errno) {
      snprintf(err_msg, sizeof(err_msg), "Error executing readdir() in %s", direc);
      perror(err_msg);
      return -1;
    }

    if (dirp) {
      snprintf(file, sizeof(file), "%s/%s", direc, dir->d_name);
      struct stat file_meta;
      lstat(file, &file_meta);
      if (errno) {
        snprintf(err_msg, sizeof(err_msg), "Error executing lstat() in %s", direc);
        perror(err_msg);
        return -1;
      }
      switch(file_meta.st_mode & S_IFMT){
          case S_IFREG:
              printf("%s\n", dir->d_name);
              //print_out(file, file_meta);
              break;
          case S_IFDIR:
              if (dir->d_name != "." || dir->d_name != ".."){
                //print_out(file, file_meta);
              } else {
                printf("%s\n", dir->d_name);
                //print_out(file, file_meta);
                dir_read(file);
              }
              break;
      }
    } else {
      if (closedir(dirp) < 0) {
        snprintf(err_msg, sizeof(err_msg), "Error executing closedir() in %s", direc);
        perror(err_msg);
        return -1;
      }
      return 0;
    }
  }
}

int print_out(char * file, struct stat file_meta) {


  mode_t mode = (file_meta.st_mode & ~S_IFMT);
  (mode & S_IRUSR) ? printf("r") : printf("-");
  (mode & S_IWUSR) ? printf("w") : printf("-");
  (mode & S_IXUSR) ? printf("x") : printf("-");
  (mode & S_IRGRP) ? printf("r") : printf("-");
  (mode & S_IWGRP) ? printf("w") : printf("-");
  (mode & S_IXGRP) ? printf("x") : printf("-");
  (mode & S_IROTH) ? printf("r") : printf("-");
  (mode & S_IWOTH) ? printf("w") : printf("-");
  (mode & S_IXOTH) ? printf("x") : printf("-");
  printf("\t%lds", file_meta.st_nlink);
  printf("\t%d", file_meta.st_uid);
  printf("\t%d", file_meta.st_gid);
  printf("\t%ld", file_meta.st_size);
  printf("\t%ld", file_meta.st_nlink);
  printf("%s", file);
  printf("\n");
}
