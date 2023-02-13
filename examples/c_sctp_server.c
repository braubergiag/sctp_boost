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


static void server(unsigned short port){
    
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);


    struct sctp_initmsg initmsg;
    initmsg.sinit_max_attempts = 4;
    initmsg.sinit_num_ostreams = 5;
    initmsg.sinit_num_ostreams = 5;


    int listen_fd = socket(AF_INET, SOCK_STREAM,IPPROTO_SCTP);
    if (listen_fd < 0)
        die("socket()");

    if ( bind(listen_fd, (struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
        die("bind()");

    if ( setsockopt(listen_fd,IPPROTO_SCTP,SCTP_INITMSG,&initmsg,sizeof(initmsg)) < 0)
        die("setsockopt()");

    if ( listen(listen_fd,initmsg.sinit_max_instreams) < 0)
        die("listen()");

    for (;;){
        char buffer[1024];
        fputs("Waiting for connection\n",stdout);
        
        int connect_fd = accept(listen_fd, (struct sockaddr *)NULL, NULL);
        if (connect_fd < 0)
            die("accept()");

        fputs("New client connected\n",stdout);
         struct sctp_sndrcvinfo sndrcvinfo;
         int flags;
        size_t bytesRcvd = sctp_recvmsg(connect_fd,buffer,sizeof(buffer),NULL,0,&sndrcvinfo,&flags);
        if (bytesRcvd > 0){
            printf("Received data : %s\n", buffer);
        }
        close(connect_fd);
    }

    

}



int main(int argc, char * argv[]){
    if (argc < 2)
        die("Usage ./c_sctp_server <Port>");

    unsigned short port = atoi(argv[1]);
    server(port);
    return 0;
}