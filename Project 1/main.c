#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
void error();

int main(int argc, char *argv[]) {

        int c = 0, fd_out = 0, fd_in = 0, buf_size = 8192;
        int bytes_r = 1, bytes_w = 1;
        while ((c = getopt(argc, argv, "b:o:")) != -1)
                switch (c) {
                case 'b':
                        buf_size = atoi(optarg);
                        break;
                case 'o':
                        fd_out = open(optarg, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd_out < 0) {
                                error(1, "creating or reading the", optarg, "null");
                                return -1;
                        }
                        break;
                case '?':
                        return -1;
                        break;
                }
        char buf[buf_size];
        if (optind < argc) // if there are infiles, it loops until the end of the argument to take in all the arguments
                for (; optind < argc; optind++) {
                        bytes_r = 1;
                        if (strcmp(argv[optind], "-") != 0) {
                                fd_in = open(argv[optind], O_RDONLY);
                                while (bytes_r != 0) {
                                        bytes_r = read(fd_in, buf, sizeof(buf));
                                        bytes_w = write(fd_out, buf, bytes_r);
                                }
                                if (fd_in < 0) {
                                        error(0, "opening the file", argv[optind], "File does not exist");
                                        return -1;
                                }
                        } else {
                                while (bytes_r != 0) {
                                        bytes_r = read(0, buf, sizeof(buf));
                                        bytes_w = write(fd_out, buf, bytes_r);
                                }
                                if (bytes_w < 0) {
                                        error(1, "writing to ", "standard output", "null");
                                        return -1;
                                }
                        }
                }
        else { // if there aren't any infiles, it just pulls from standard input
                while (bytes_r != 0) {
                        bytes_r = read(0, buf, sizeof(buf));
                        bytes_w = write(fd_out, buf, bytes_r);
                }
                if (bytes_w < 0) {
                        error(1, "writing to ", "standard output", "null");
                        return -1;
                }
        }
}

void error(int p_err, char *action, char *subject, char *custom_error) {
        char err_msg[1024];
        snprintf(err_msg, sizeof(err_msg), "Error %s %s", action, subject);
        if (p_err == 1)
                perror(err_msg);
        else
                printf("%s: %s", err_msg, custom_error);
}
