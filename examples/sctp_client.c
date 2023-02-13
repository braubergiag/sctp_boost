#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>

static void die(const char *s) {
        perror(s);
        exit(1);
}
static void client(unsigned short port) {
        int conn_fd, ret;
        const char *msg = "Hello, Server!";
        struct sockaddr_in servaddr;
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        conn_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
        if (conn_fd < 0)
                die("socket()");

        ret = connect(conn_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
        if (ret < 0)
                die("connect()");

        ret = sctp_sendmsg(conn_fd, (void *) msg, strlen(msg) + 1, NULL, 0, 0, 0, 0, 0, 0 );
        if (ret < 0)
                die("sctp_sendmsg");

        close(conn_fd);
}

int main(int argc, char **argv) {

    if (argc < 2){
        die("<Port>");
    }
    unsigned short port = atoi(argv[1]);
    client(port);

return 0;
}