#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
extern int errno;

void error();
int main(int argc, char *argv[]) {

        int c = 0, err = 0, fd_out = 0, fd_in = 0, buf_val = 0;
        int bflag = 0, oflag = 0, bytes_r = 0, bytes_w = 0;
        char *bname, *oname;
        char *buf = (char *) malloc(100*sizeof(char));
        while ((c = getopt(argc, argv, "b:o:")) != -1)
                switch (c) {
                case 'b':
                        bflag = 1;
                        bname = optarg;
                        break;
                case 'o':
                        oflag = 1;
                        oname = optarg;
                        break;
                case '?':
                        err = 1;
                        break;
                }
        if (err == 1)
                return -1;

        if (oflag == 1) {
                fd_out = open(oname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out < 0) {
                        error(1, "creating or reading the", oname, "null");
                        return -1;
                }
        }
        if (bflag == 1) {
                buf_val = atoi(bname);
                if (buf_val == 0) {
                        error(0, "converting string to integer with the", "buffer value", "This string is not a number");
                        return -1;
                }
                buf = (char *) malloc(atoi(bname)*sizeof(char));
        }

        if (optind < argc) // if there are infiles, it loops
                for (; optind < argc; optind++) {
                        printf("%s\n", argv[optind]);
                        if (argv[optind] != "-") {
                                printf("heyo");
                                fd_in = open(argv[optind], O_RDONLY);
                                bytes_w = write(fd_out, buf, read(fd_in, buf, 100));
                                if (fd_in < 0) {
                                        error(0, "opening the file", argv[optind], "File does not exist");
                                        return -1;
                                }
                        } else {
                                printf("heyo-");
                                //bytes_w = write(fd_out, buf, read(0, buf, 100));
                        }
                }
        else { // if there aren't any infiles, it just pulls from standard input
                bytes_w = write(fd_out, buf, read(0, buf, 100));
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
