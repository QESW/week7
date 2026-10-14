
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define FIFO_PATH "/tmp/week7_fifo"

int main(void) {
    char buffer[1024];

    if (mkfifo(FIFO_PATH, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }

    printf("FIFO 수신 대기 중...\n");
    fflush(stdout);

    int fd = open(FIFO_PATH, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    if (n < 0) {
        perror("read");
        close(fd);
        return 1;
    }

    buffer[n] = '\0';
    printf("받은 메시지: %s\n", buffer);

    close(fd);

    if (unlink(FIFO_PATH) == -1) {
        perror("unlink");
        return 1;
    }

    printf("FIFO 삭제 완료\n");
    return 0;
}
