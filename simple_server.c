#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define SERV_PORT 9527

int main(int argc, char *argv[])
{
    int sfd, cfd, ret;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_len;
    char buf[BUFSIZ];

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
        perror("socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ret = bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret != 0)
        perror("bind");

    ret = listen(sfd, 128);
    if (ret != 0)
        perror("listen");

    clnt_addr_len = sizeof(clnt_addr);
    cfd = accept(sfd, (struct sockaddr *)&clnt_addr, &clnt_addr_len);
    if (cfd == -1)
        perror("accept");

    printf("connected from client addr = %s, port = %d\n",
            inet_ntop(AF_INET, &clnt_addr.sin_addr.s_addr, buf, sizeof(buf)),
            ntohs(clnt_addr.sin_port));

    while (1) {
        ret = read(cfd, buf, sizeof(buf));
        if (ret == -1)
            perror("read");

        for (int i = 0; i < ret; i++)
            buf[i] = toupper(buf[i]);
        write(cfd, buf, ret);
    }

    close(sfd);
    close(cfd);

    return 0;
}
