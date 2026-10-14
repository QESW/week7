
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define FIFO_PATH "/tmp/week7_fifo"

int main(int argc, char *argv[]) {
    const char *message =
        (argc >= 2) ? argv[1] : "Hello from FIFO writer!";

    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    size_t len = strlen(message);
    size_t sent = 0;

    while (sent < len) {
        ssize_t n = write(fd, message + sent, len - sent);

        if (n <= 0) {
            perror("write");
            close(fd);
            return 1;
        }

        sent += (size_t)n;
    }

    printf("메시지 전송 완료: %s\n", message);

    close(fd);
    return 0;
}
