#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char *inputs[10];

void parse_input();
void error();
void minicat();

int main()
{
    parse_input();
    /*int x = 0;
    while (inputs[x] != '\0') {
        printf("%s\n", inputs[x++]);
    }*/
    printf(&inputs[0][0]);
    if (inputs[0] == "minicat") {
        minicat();
    } else {
        //error();
        return -1;
    }

    return 0;
}

void parse_input() {
    int size;
    int fd = 0, index = 0;
    char *c = (char *) calloc(100, sizeof(char));
    size = read(fd, c, 100);
    char *pch;
    pch = strtok (c," ,.-");
    while (pch != NULL) {
        inputs[index++] = pch;
        pch = strtok (NULL, " ,.-");
    }
}

void error(char op[], char ob[], int message, char err[]) {

}

void minicat() {

}
