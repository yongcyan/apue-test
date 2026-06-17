#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    int fd, ret;
    pid_t pid;
    pid = fork();
    switch (pid) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
        fflush(stdout);
        //_exit(EXIT_SUCCESS);
        puts("Child sleeping.");
        sleep(5);

        puts("Child creating new seesion.");
        pid = setsid();
        if (pid == -1)
            perror("setsid");
        ret = chdir("/tmp");
        if (ret != 0)
            perror("chdir");
        umask(0002);
        fd = open("/tmp/daemon-file", O_RDONLY|O_CREAT);
        close(fd);

        // 0/1/2 --> /dev/null
        close(STDIN_FILENO);
        fd = open("/dev/null", O_RDWR);
        if (fd == -1)
            perror("open /dev/null");
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);

        for (;;);
    default:
        printf("Child is PID %d\n", pid);
        puts("Parent exiting.");
        exit(EXIT_SUCCESS);
    }
}
