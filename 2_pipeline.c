
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int pipefd[2];
    int sep = -1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], ":") == 0) {
            sep = i;
            break;
        }
    }

    if (sep <= 1 || sep >= argc - 1) {
        fprintf(stderr,
                "사용법: %s 명령1 인자... : 명령2 인자...\n",
                argv[0]);
        return 1;
    }

    argv[sep] = NULL;
    char **cmd1 = &argv[1];
    char **cmd2 = &argv[sep + 1];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid1 = fork();

    if (pid1 < 0) {
        perror("fork");
        return 1;
    }

    if (pid1 == 0) {
        close(pipefd[0]);

        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }

        close(pipefd[1]);
        execvp(cmd1[0], cmd1);
        perror("execvp 첫 번째 명령");
        exit(127);
    }

    pid_t pid2 = fork();

    if (pid2 < 0) {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        waitpid(pid1, NULL, 0);
        return 1;
    }

    if (pid2 == 0) {
        close(pipefd[1]);

        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }

        close(pipefd[0]);
        execvp(cmd2[0], cmd2);
        perror("execvp 두 번째 명령");
        exit(127);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    if (!WIFEXITED(status1) || WEXITSTATUS(status1) != 0 ||
        !WIFEXITED(status2) || WEXITSTATUS(status2) != 0) {
        return 1;
    }

    return 0;
}
