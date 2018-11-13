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

char err_msg[1024];
int files_proc = 0;
long bytes_proc = 0;

void handler(int s) { //writes the total files and bytes processed before exiting
        fprintf(stderr, "Files Processed: %i, Bytes Processed: %l.\n", files_proc, bytes_proc);
        exit(0);
}

void check_error(int val, char *obj, char c);

int main(int argc, char *argv[]) {
        struct sigaction sa;
        int fd_out = 0, fd_in = 0, buf_size = 4096;
        int bytes_r = 1, bytes_w = 1;
        char *pattern, *buf;
        char *oname = "standard output";

        sa.sa_handler=handler; //handles the signal properly
        sa.sa_flags=0;
        sigemptyset(&sa.sa_mask);
        if (sigaction(SIGINT, &sa, 0) == -1) {
                snprintf(err_msg, sizeof(err_msg), "Error changing the signal disposition:");
                perror(err_msg);
                return -1;
        }

        if (argc < 3) { //if there are fewer than 3 arguments catgrepmore is being used wrong
                fprintf(stderr, "Correct format: catgrepmore pattern infline1 [...infile2...]\n");
                return -1;
        }
        pattern = argv[optind++];

        if (!(buf = malloc(buf_size * sizeof(char)))) { //checks to see if memory allocation succeeded
                snprintf(err_msg, sizeof(err_msg), "Error allocating memory for default buffer size:");
                perror(err_msg);
                return -1;
        }

        if (optind < argc) { //loops until the end of the argument to take in all the arguments
                for (; optind < argc; optind++) {
                        bytes_r = 1;
                        fd_in = open(argv[optind], O_RDONLY);
                        check_error(fd_in, argv[optind], 'o');
                        while (bytes_r != 0) {
                                bytes_r = read(fd_in, buf, sizeof(buf));
                                check_error(bytes_r, argv[optind], 'r');
                                bytes_w = write(fd_out, buf, bytes_r);
                                check_error(bytes_w, oname, 'w');
                                check_error(bytes_w - bytes_r, oname, 'p');
                        }
                        check_error(close(fd_out), oname, 'c');
                }
        }
}

void check_error(int val, char *obj, char c) { //4 cases: error opening, reading, writing, and closing

        if (val < 0) {
                switch (c) {
                case 'o':
                        snprintf(err_msg, sizeof(err_msg), "Error opening file %s", obj);
                        break;
                case 'r':
                        snprintf(err_msg, sizeof(err_msg), "Error reading file %s", obj);
                        break;
                case 'w':
                        snprintf(err_msg, sizeof(err_msg), "Error writing to file %s", obj);
                        break;
                case 'c':
                        snprintf(err_msg, sizeof(err_msg), "Error closing file %s", obj);
                        break;
                case 'p':
                        snprintf(err_msg, sizeof(err_msg), "Error with partial write to file %s", obj);
                }
                perror(err_msg);
                exit(-1);
        }
}
