#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdio.h>

void handle_child(int signo)
{
    pid_t wpid;
    int status;
    // see signal(7) "Queueing and delivery semantics for standard signals"
    while ((wpid = waitpid(-1, &status, 0)) != -1)
        if (WIFEXITED(status))
            printf("Release resources of child pid = %d, ret = %d\n", wpid, WEXITSTATUS(status));
    return;
}

int main (int argc, char *argv[])
{
    pid_t pid;
    int i, ret;

    sigset_t set;
    ret = sigemptyset(&set);
    if (ret != 0)
        perror("sigemptyset");
    ret = sigaddset(&set, SIGCHLD);
    if (ret != 0)
        perror("sigaddset");
    sigprocmask(SIG_BLOCK, &set, NULL);
    if (ret != 0)
        perror("sigprocmask SIG_BLOCK");

    for (i=0; i<5; i++) {
        // loop creating child
        if ((pid = fork()) == 0)
            break;
    }
    // parent
    if (i == 5) {
        printf("I'm parent with pid = %d\n", getpid());
        //waitpid(-1, NULL, 0);

         /*struct sigaction {
               void     (*sa_handler)(int);
               void     (*sa_sigaction)(int, siginfo_t *, void *);
               sigset_t   sa_mask;
               int        sa_flags;
               void     (*sa_restorer)(void);
           };
        */
        struct sigaction act;
        act.sa_handler = handle_child;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGCHLD, &act, NULL);
        if (ret != 0)
            perror("sigaction");

        sigprocmask(SIG_UNBLOCK, &set, NULL);
        if (ret != 0)
            perror("sigprocmask SIG_UNBLOCK");

        while(1);

    // children
    } else {
        printf("I'm child %d with pid = %d\n", i+1, getpid());
        return i+1;
    }
}
