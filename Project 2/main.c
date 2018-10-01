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
      snprintf(err_msg, sizeof(err_msg), "Error executing readdir(dirp) in %s", direc);
      perror(err_msg);
      return -1;
    }

    if (dirp) {
      snprintf(file, sizeof(file), "%s/%s", direc, dir->d_name);
      struct stat file_meta;
      lstat(file, &file_meta);
      if (errno) {
        snprintf(err_msg, sizeof(err_msg), "Error executing lstat(%s, &file_meta) in %s", file, direc);
        perror(err_msg);
        return -1;
      }
      if (strcmp(dir->d_name, ".") == 0) {
        print_out(file, file_meta);
      }
      if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){
        if ((file_meta.st_mode & S_IFMT) == S_IFDIR) {
          print_out(file, file_meta);
          dir_read(file);
        } else {
          print_out(file, file_meta);
        }
      }

    } else {
      if (closedir(dirp) < 0) {
        snprintf(err_msg, sizeof(err_msg), "Error executing closedir(dirp) in %s", direc);
        perror(err_msg);
        return -1;
      }
      return 0;
    }
  }
}

int print_out(char * file, struct stat file_meta) {
  int errno = 0;
  int sym = S_ISLNK(file_meta.st_mode);
  char sym_buffer[257];
  struct passwd * pw;
  struct group * gr;
  printf("%ld", file_meta.st_ino);
  printf("\t%ld\t", file_meta.st_blocks/2); //this is so  the block size is consistent with the find command
  mode_t mode = (file_meta.st_mode & ~S_IFMT);
  if (sym) {
    readlink(file, sym_buffer, sizeof(sym_buffer));
    if (errno) {
      snprintf(err_msg, sizeof(err_msg), "Error executing readlink(%s, sym_buffer, %ld) in %s", file, sizeof(sym_buffer), file);
      perror(err_msg);
      return -1;
    }
    printf("l");
  } else {
    (S_ISDIR(file_meta.st_mode)) ? printf("d") : printf("-");
  }

  (mode & S_IRUSR) ? printf("r") : printf("-");
  (mode & S_IWUSR) ? printf("w") : printf("-");
  (mode & S_IXUSR) ? printf("x") : printf("-");
  (mode & S_IRGRP) ? printf("r") : printf("-");
  (mode & S_IWGRP) ? printf("w") : printf("-");
  (mode & S_IXGRP) ? printf("x") : printf("-");
  (mode & S_IROTH) ? printf("r") : printf("-");
  (mode & S_IWOTH) ? printf("w") : printf("-");
  (mode & S_IXOTH) ? printf("x") : printf("-");
  printf("\t%ld", file_meta.st_nlink);
  pw = getpwuid(file_meta.st_uid);
  if (pw) {
    printf("\t%s", pw->pw_name);
  } else {
    printf("\t%d", file_meta.st_uid);
  }
  gr = getgrgid(file_meta.st_gid);
  if (gr) {
    printf("\t%s", gr->gr_name);
  } else {
    printf("\t%d", file_meta.st_gid);
  }
  printf("\t%ld", file_meta.st_size);
  printf("\t%s ", ctime(&file_meta.st_mtime));
  if (sym) {
    printf("\t%s -> %s", file, sym_buffer);
  } else {
    printf("\t%s", file);
  }
  printf("\n");
}
