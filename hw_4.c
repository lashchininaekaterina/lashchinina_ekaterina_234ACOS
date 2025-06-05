#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

void run_fork() {
    pid_t child_pid = fork();
    if (child_pid == 0) {
        printf("Hello from Child (PID: %d)\n", getpid());
        pid_t grandchild_pid = fork();
        if (grandchild_pid == 0) {
            printf("Hello from Grandchild (PID: %d)\n", getpid());
        } else {
            wait(NULL);
        }
    } else {
        wait(NULL);
        printf("Hello from Parent (PID: %d)\n", getpid());
    }
}

void run_outredir(char *cmd, char *outfile) {
    int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fork() == 0) {
        dup2(fd, STDOUT_FILENO);
        close(fd);
        execlp(cmd, cmd, NULL);
        perror("exec failed");
        exit(1);
    } else {
        close(fd);
        wait(NULL);
    }
}

void run_allredir(char *cmd, char *infile, char *outfile) {
    int in_fd = open(infile, O_RDONLY);
    int out_fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    pid_t pid = fork();
    if (pid == 0) {
        dup2(in_fd, STDIN_FILENO);
        dup2(out_fd, STDOUT_FILENO);
        close(in_fd);
        close(out_fd);
        execlp(cmd, cmd, NULL);
        perror("exec failed");
        exit(1);
    } else {
        int status;
        wait(&status);
        printf("WEXITSTATUS: %d\n", WEXITSTATUS(status));
        close(in_fd);
        close(out_fd);
    }
}

void run_argredir(int argc, char *argv[]) {
    int in_fd = open(argv[1], O_RDONLY);
    int out_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    pid_t pid = fork();
    if (pid == 0) {
        dup2(in_fd, STDIN_FILENO);
        dup2(out_fd, STDOUT_FILENO);
        close(in_fd);
        close(out_fd);
        execvp(argv[3], &argv[3]);
        perror("execvp failed");
        exit(1);
    } else {
        int status;
        wait(&status);
        printf("WEXITSTATUS: %d\n", WEXITSTATUS(status));
        close(in_fd);
        close(out_fd);
    }
}

void run_piperedir(int argc, char *argv[]) {
    int pipefd[2];
    pipe(pipefd);
    pid_t pid1 = fork();
    if (pid1 == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execlp(argv[1], argv[1], NULL);
        perror("exec1 failed");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execvp(argv[2], &argv[2]);
        perror("exec2 failed");
        exit(1);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    wait(NULL);
    wait(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s fork\n", argv[0]);
        fprintf(stderr, "  %s outredir <command> <outfile>\n", argv[0]);
        fprintf(stderr, "  %s allredir <command> <infile> <outfile>\n", argv[0]);
        fprintf(stderr, "  %s argredir <infile> <outfile> <command> [args...]\n", argv[0]);
        fprintf(stderr, "  %s piperedir <command1> <command2> [args...]\n", argv[0]);
        exit(1);
    }

    if (strcmp(argv[1], "fork") == 0) {
        run_fork();
    } else if (strcmp(argv[1], "outredir") == 0 && argc == 4) {
        run_outredir(argv[2], argv[3]);
    } else if (strcmp(argv[1], "allredir") == 0 && argc == 5) {
        run_allredir(argv[2], argv[3], argv[4]);
    } else if (strcmp(argv[1], "argredir") == 0 && argc >= 5) {
        run_argredir(argc - 1, &argv[1]);
    } else if (strcmp(argv[1], "piperedir") == 0 && argc >= 4) {
        run_piperedir(argc - 1, &argv[1]);
    } else {
        fprintf(stderr, "Invalid usage\n");
        exit(1);
    }

    return 0;
}




// argredir.c

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: %s infile outfile command [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int in_fd = open(argv[1], O_RDONLY);
    if (in_fd < 0) {
        perror("open infile");
        exit(EXIT_FAILURE);
    }

    int out_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd < 0) {
        perror("open outfile");
        close(in_fd);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        close(in_fd);
        close(out_fd);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        if (dup2(in_fd, STDIN_FILENO) < 0) {
            perror("dup2 infile");
            exit(EXIT_FAILURE);
        }
        if (dup2(out_fd, STDOUT_FILENO) < 0) {
            perror("dup2 outfile");
            exit(EXIT_FAILURE);
        }
        close(in_fd);
        close(out_fd);

        execvp(argv[3], &argv[3]);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }

    close(in_fd);
    close(out_fd);

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid failed");
        exit(EXIT_FAILURE);
    }

    if (WIFEXITED(status)) {
        printf("WEXITSTATUS: %d\n", WEXITSTATUS(status));
    } else {
        printf("Child terminated abnormally\n");
    }

    return 0;
}




// pip2redir.c

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s command1 command2 [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork 1");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        if (dup2(pipefd[1], STDOUT_FILENO) < 0) {
            perror("dup2 pipe write");
            exit(EXIT_FAILURE);
        }
        close(pipefd[0]);
        close(pipefd[1]);
        execlp(argv[1], argv[1], NULL);
        perror("exec1 failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork 2");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        if (dup2(pipefd[0], STDIN_FILENO) < 0) {
            perror("dup2 pipe read");
            exit(EXIT_FAILURE);
        }
        close(pipefd[0]);
        close(pipefd[1]);
        execvp(argv[2], &argv[2]);
        perror("exec2 failed");
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    int status;
    int exited = 0;

    while (1) {
        pid_t w = wait(&status);
        if (w == -1) {
            if (errno == ECHILD) break;
            perror("wait");
            exit(EXIT_FAILURE);
        }
        if (WIFEXITED(status))
            printf("Child PID %d exited with status %d\n", w, WEXITSTATUS(status));
        else
            printf("Child PID %d terminated abnormally\n", w);
        exited++;
    }

    return 0;
}
