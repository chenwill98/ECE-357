//William Chen
//Professor Hakner
//ECE-357 Operating Systems

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>

char err_msg[1024];
int files_proc = 0;
long bytes_proc = 0;

void handler(int s) { //writes the total files and bytes processed before exiting
        fprintf(stderr, "Files Processed: %i\nBytes Processed: %l\n", files_proc, bytes_proc);
        exit(0);
}

int pipeIO(int pipe[], const int fd_to_redirect) {
        /* connect stding to pipe and close the pipe's file descriptors */
        if (fd_to_redirect == STDIN_FILENO) {
                if (dup2(pipe[0], STDIN_FILENO) == -1) {
                        fprintf(stderr, "Error redirecting input: %s\n", strerror(errno));
                        return -1;
                }
        }
        else {
                if (dup2(pipe[1], STDOUT_FILENO) == -1) {
                        fprintf(stderr, "Error redirecting output: %s\n", strerror(errno));
                        return -1;
                }
        }
        return 0;
}

int main(int argc, char *argv[]) {
        struct sigaction sa;
        int fd_in = 0, buf_size = 4096;
        int bytes_r = 1, bytes_w = 1;
        char *pattern, *buf;

        sa.sa_handler=handler; //handles the signal properly
        sa.sa_flags=0;
        sigemptyset(&sa.sa_mask);
        if (sigaction(SIGINT, &sa, 0) == -1) {
                perror("Error changing the signal disposition:");
                return -1;
        }

        if (argc < 3) { //if there are fewer than 3 arguments catgrepmore is being used wrong
                fprintf(stderr, "Correct format: catgrepmore pattern infline1 [...infile2...]\n");
                return -1;
        }
        pattern = argv[optind++];

        if (!(buf = malloc(buf_size * sizeof(char)))) { //checks to see if memory allocation succeeded
                perror("Error allocating memory for default buffer size");
                return -1;
        }

        for (; optind < argc; optind++) { //loops until the end of the argument to take in all the arguments
                int p1[2], p2[2], pid1, pid2, status1, status2;
                if (pipe(p1) < 0 || pipe(p2) < 0) { //p1 is from cat to grep and p2 is for grep to more
                        perror("Error creating pipes");
                        return -1;
                }

                pid1 = fork();
                if (pid1 < 0) {
                        perror("Error forking child process");
                        return -1;
                }
                if (pid1 == 0) { //child process for grep
                        if (pipeIO(p1, STDIN_FILENO) < 0 || pipeIO(p2, STDOUT_FILENO) < 0)
                                return -1;

                        if (close(p1[0]) < 0 || close(p1[1]) < 0) {
                                perror("Error closing the file descriptors of pipe between cat and grep");
                                return -1;
                        }
                        if (close(p2[0]) < 0 || close(p2[1]) < 0) {
                                perror("Error closing the file descriptors of pipe between grep and more");
                                return -1;
                        }

                        if (execlp("grep", "grep", pattern, (char *)0) == -1) {
                                fprintf(stderr, "Error running 'grep' program: %s\n", strerror(errno));
                                return -1;
                        }



                } else {
                        pid2 = fork();
                        if (pid2 < 0) {
                                perror("Error forking child process");
                                return -1;
                        }
                        if (pid2 == 0) { //child process for more
                                if (pipeIO(p2, STDIN_FILENO) < 0)
                                        return -1;

                                if (close(p1[0]) < 0 || close(p1[1]) < 0) {
                                        perror("Error closing the file descriptors of pipe between cat and grep");
                                        return -1;
                                }
                                if (close(p2[0]) < 0 || close(p2[1]) < 0) {
                                        perror("Error closing the file descriptors of pipe between grep and more");
                                        return -1;
                                }
                                if (execlp("more", "more", (char *)0) == -1) {
                                        fprintf(stderr, "Error running 'more' program: %s\n", strerror(errno));
                                        return -1;
                                }


                        } else { //parent process for catgrepmore
                                fd_in = open(argv[optind], O_RDONLY);
                                if (fd_in < 0) {
                                        snprintf(err_msg, sizeof(err_msg), "Error opening input file %s", argv[optind]);
                                        perror(err_msg);
                                        return -1;
                                }
                                files_proc++;

                                while (bytes_r != 0) {
                                        bytes_r = read(fd_in, buf, sizeof(buf));
                                        if (bytes_r < 0) {
                                                snprintf(err_msg, sizeof(err_msg), "Error reading input file %s", argv[optind]);
                                                perror(err_msg);
                                                return -1;
                                        }
                                        bytes_proc += bytes_r;

                                        bytes_w = write(p1[1], buf, bytes_r);
                                        if (bytes_w < 0) {
                                                perror("Error writing to pipe between cat and grep");
                                                return -1;
                                        }
                                        if (bytes_w - bytes_r < 0) {
                                                perror("Partial write occured");
                                                return -1;
                                        }
                                }
                                if (close(p1[0]) < 0 || close(p1[1]) < 0) {
                                        perror("Error closing the file descriptors of pipe between cat and grep");
                                        return -1;
                                }
                                if (close(p2[0]) < 0 || close(p2[1]) < 0) {
                                        perror("Error closing the file descriptors of pipe between grep and more");
                                        return -1;
                                }
                                if (waitpid(pid1, &status1, 0) < 0 || waitpid(pid2, &status2, 0) < 0) {
                                        return -1;
                                }
                                if (WIFEXITED(status1) == 0 || WIFEXITED(status2) == 0) {
                                        perror("Error exiting grep or more normally");
                                        return -1;
                                }
                        }
                }
        }
}
