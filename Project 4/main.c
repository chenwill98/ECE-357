#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
void check_error(int val, char *obj, char c);

int main(int argc, char *argv[]) {
        int c = 0, fd_out = 0, fd_in = 0, buf_size = 8192;
        int bytes_r = 1, bytes_w = 1;
        char *oname = "standard output";
        while ((c = getopt(argc, argv, "b:o:")) != -1)
                switch (c) {
                case 'b':
                        buf_size = atoi(optarg);
                        break;
                case 'o':
                        oname = optarg;
                        fd_out = open(oname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        check_error(fd_out, oname, 'o');
                        break;
                case '?':
                        return -1;
                        break;
                }
        char buf[buf_size];
        if (optind < argc) {// if there are infiles, it loops until the end of the argument to take in all the arguments
                for (; optind < argc; optind++) {
                        bytes_r = 1;
                        if (strcmp(argv[optind], "-") != 0) {
                                fd_in = open(argv[optind], O_RDONLY);
                                check_error(fd_in, argv[optind], 'o');
                                while (bytes_r != 0) {
                                        bytes_r = read(fd_in, buf, sizeof(buf));
                                        check_error(bytes_r, argv[optind], 'r');
                                        bytes_w = write(fd_out, buf, bytes_r);
                                        check_error(bytes_w, oname, 'w');
                                        check_error(bytes_w - bytes_r, oname, 'p');
                                }
                        } else {
                                while (bytes_r != 0) {
                                        bytes_r = read(0, buf, sizeof(buf));
                                        check_error(bytes_r, argv[optind], 'r');
                                        bytes_w = write(fd_out, buf, bytes_r);
                                        check_error(bytes_w, oname, 'w');
                                        check_error(bytes_w - bytes_r, oname, 'p');
                                }
                        }
                }
        }
        else { // if there aren't any infiles, it just pulls from standard input
                while (bytes_r != 0) {
                        bytes_r = read(0, buf, sizeof(buf));
                        check_error(bytes_r, argv[optind], 'r');
                        bytes_w = write(fd_out, buf, bytes_r);
                        check_error(bytes_w, oname, 'w');
                        check_error(bytes_w - bytes_r, oname, 'p');
                }
        }
        check_error(close(fd_in), argv[optind], 'c');
        check_error(close(fd_out), oname, 'c');
}

void check_error(int val, char *obj, char c) { //4 cases: error opening, reading, writing, and closing
        char err_msg[1024];
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
