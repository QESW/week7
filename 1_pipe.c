
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int p2c[2];  
    int c2p[2];  
    pid_t pid;
    char buffer[1024];

    if (argc != 2) {
        fprintf(stderr, "사용법: %s \"문자열\"\n", argv[0]);
        return 1;
    }

    if (pipe(p2c) == -1 || pipe(c2p) == -1) {
        perror("pipe");
        return 1;
    }

    pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        close(p2c[1]);
        close(c2p[0]);

        ssize_t n = read(p2c[0], buffer, sizeof(buffer) - 1);
        if (n < 0) {
            perror("read");
            exit(1);
        }

        buffer[n] = '\0';

        for (int i = 0; i < n; i++) {
            buffer[i] = (char)toupper((unsigned char)buffer[i]);
        }

        printf("자식: 대문자로 변환 완료\n");

        if (write(c2p[1], buffer, n) != n) {
            perror("write");
            exit(1);
        }

        close(p2c[0]);
        close(c2p[1]);
        exit(0);
    } else {
        close(p2c[0]);
        close(c2p[1]);

        size_t len = strlen(argv[1]);

        if (write(p2c[1], argv[1], len) != (ssize_t)len) {
            perror("write");
            return 1;
        }

        close(p2c[1]);

        ssize_t n = read(c2p[0], buffer, sizeof(buffer) - 1);
        if (n < 0) {
            perror("read");
            return 1;
        }

        buffer[n] = '\0';
        printf("부모가 받은 결과: %s\n", buffer);

        close(c2p[0]);
        waitpid(pid, NULL, 0);
    }

    return 0;
}
