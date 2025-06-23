#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

static int get_signal_number(const char *name) {
    if (strcasecmp(name, "INT")  == 0) return SIGINT;
    if (strcasecmp(name, "TERM") == 0) return SIGTERM;
    if (strcasecmp(name, "ABRT") == 0) return SIGABRT;
    if (strcasecmp(name, "SEGV") == 0) return SIGSEGV;
    if (strcasecmp(name, "HUP")  == 0) return SIGHUP;
    if (strcasecmp(name, "ILL")  == 0) return SIGILL;
    if (strcasecmp(name, "KILL") == 0) return SIGKILL;
    return -1;
}

static volatile sig_atomic_t quit_counter = 0;
static int termination_signal = 0;

static void signal_handler(int signo) {
    printf("[Caught %d]\n", signo);
    if (signo == termination_signal) {
        if (++quit_counter >= 3) {
            printf("Got QUIT signal %d three times, exiting\n", signo);
            _exit(0);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr,
            "Usage: %s <mode> [...args...]\n"
            "Modes:\n"
            "  endless\n"
            "  proc <timeout>\n"
            "  killn <pid> <SIG>\n"
            "  catchsig <timeout> <SIG1> [SIG2...]\n"
            "  childctl <timeout> <QUIT_SIG> <SIG1> [SIG2...]\n",
            argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "endless") == 0) {
        while (1) ;
    }

    if (strcmp(argv[1], "proc") == 0) {
        int interval = atoi(argv[2]);
        pid_t process_id = getpid();
        unsigned long counter = 0;
        while (1) {
            printf("%d: %lu\n", process_id, counter++);
            sleep(interval);
        }
    }

    if (strcmp(argv[1], "killn") == 0) {
        pid_t target_pid = (pid_t)atoi(argv[2]);
        int sig_num = get_signal_number(argv[3]);
        if (sig_num < 0) {
            fprintf(stderr, "Unknown signal %s\n", argv[3]);
            return 1;
        }
        kill(target_pid, sig_num);
        return 0;
    }

    if (strcmp(argv[1], "catchsig") == 0) {
        int interval = atoi(argv[2]);
        struct sigaction action = { .sa_handler = signal_handler };
        for (int idx = 3; idx < argc; idx++) {
            int sig_num = get_signal_number(argv[idx]);
            if (sig_num > 0) sigaction(sig_num, &action, NULL);
        }
        pid_t process_id = getpid();
        unsigned long counter = 0;
        while (1) {
            printf("%d: %lu\n", process_id, counter++);
            sleep(interval);
        }
    }

    if (strcmp(argv[1], "childctl") == 0) {
        int interval = atoi(argv[2]);
        termination_signal = get_signal_number(argv[3]);
        struct sigaction action = { .sa_handler = signal_handler };
        sigaction(termination_signal, &action, NULL);
        for (int idx = 4; idx < argc; idx++) {
            int sig_num = get_signal_number(argv[idx]);
            if (sig_num > 0) sigaction(sig_num, &action, NULL);
        }
        pid_t process_id = getpid();
        unsigned long counter = 0;
        while (1) {
            printf("%d: %lu\n", process_id, counter++);
            sleep(interval);
        }
    }

    fprintf(stderr, "Unknown mode: %s\n", argv[1]);
    return 1;
}
