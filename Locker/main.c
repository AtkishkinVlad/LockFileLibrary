#include<stdio.h>

#include<stdlib.h>

#include<string.h>

#include<sys/stat.h>

#include<fcntl.h>

#include<unistd.h>

#include<signal.h>

#include<errno.h>

// Файл, доступ к которому блокируют/освобождают процессы
#define WORK_FILENAME "work.txt"
// Файл со статистикой группы процессов
#define LOG_FILENAME "log.txt"

int haveWorkLock = 0;
int locksNum = 0;
int unlocksNum = 0;
int errorFlag = 0;

int isExistsRegFile(char * filename) {
    struct stat s;

    if (stat(filename, & s) == -1) {
        if (errno == ENOENT) {
            return 0;
        }
        perror("Error with 'stat' system call");
    }

    if (S_ISREG(s.st_mode)) {
        return 1;
    }

    fprintf(stderr, "File '%s' is not a regular file\n", filename);
    exit(1);
}

// fatal указывает не необходимость завершить программу без создания записи в логе
void handleError(char * errorMessage, int fatal) {
    errorFlag = 1;
    fprintf(stderr, errorMessage);

    if (fatal) {
        exit(1);
    } else {
        kill(getpid(), SIGINT);
    }

    // Никогда сюда не попадем, но vscode extension просит ... :)
    exit(0);
}

void lockFile(char * filename, int * haveLock, int fatal) {
    char buf[1024];
    char out_buf[1024];
    sprintf(buf, "%s.lck", filename);

    while (isExistsRegFile(buf));
    int fd_lock = open(buf, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (fd_lock == -1) {
        perror("Cannot create lock file\n");
        exit(1);
    }

    * haveLock = 1;
    pid_t curPid = getpid();

    sprintf(out_buf, "%d\n", curPid);
    ssize_t len = strlen(out_buf);

    if (write(fd_lock, out_buf, strlen(out_buf)) != len) {
        handleError("I/O error while writing to lock file\n", fatal);
    }

    if (close(fd_lock) == -1) {
        perror("Cannot close file\n");
        exit(1);
    }
}

void unlockFile(char * filename, int * haveLock, int fatal) {
    char buf[1024];
    sprintf(buf, "%s.lck", filename);

    if (!isExistsRegFile(buf)) {
        handleError("unlocking: lock file does not exist\n", fatal);
    }

    int fd_lock = open(buf, O_RDONLY);

    if (fd_lock == -1) {
        handleError("unlocking: cannot open lock file for reading\n", fatal);
    }

    char in_buf[1024];
    ssize_t len = read(fd_lock, in_buf, 1023);

    if (len == -1) {
        handleError("unlocking: cannot read lock file\n", fatal);
    }

    in_buf[len] = '\0';

    pid_t curPid = getpid();
    pid_t lockPid;

    if (sscanf(in_buf, "%d", & lockPid) != 1) {
        handleError("unlocking: bad format of lock-file\n", fatal);
    }

    if (curPid != lockPid) {
        handleError("unlocking: lock file contains bad PID\n", fatal);
    }

    if (close(fd_lock) == -1) {
        perror("Cannot close file\n");
        exit(1);
    }

    remove(buf);
    * haveLock = 0;
}

void signalHandler(int sig) {
    int haveLogLock = 0;

    lockFile(LOG_FILENAME, & haveLogLock, 1);

    char out_buf[1024];
    sprintf(out_buf, "Process with PID %d made %d locks and %d unlocks. %s\n",
        getpid(), locksNum, unlocksNum,
        errorFlag ? "Exited with erorr" : "Exited without error");

    ssize_t len = strlen(out_buf);
    int fd_lock = open(LOG_FILENAME, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (write(fd_lock, out_buf, strlen(out_buf)) != len) {
        fprintf(stderr, "I/O error while writing to log file\n");
        exit(1);
    }

    if (close(fd_lock) == -1) {
        perror("Cannot close log file\n");
        exit(1);
    }

    unlockFile(LOG_FILENAME, & haveLogLock, 1);
    exit(0);
}

int main() {
    if (signal(SIGINT, & signalHandler) == SIG_ERR) {
        perror("Cannot set signal handler for SIGINT\n");
        exit(1);
    }

    while (1) {
        lockFile(WORK_FILENAME, & haveWorkLock, 0);
        locksNum++;
        sleep(1);
        unlockFile(WORK_FILENAME, & haveWorkLock, 0);
        unlocksNum++;
    }

    return 0;
}