#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h>

#define PORT 9051
#define MAXDATASIZE 100

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in their_addr;

    if ((he=gethostbyname("localhost")) == NULL) { 
        herror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;      /* host byte order */
    their_addr.sin_port = htons(PORT);    /* short, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(their_addr.sin_zero), 8);     /* zero the rest of the struct */

    if (connect(sockfd, (struct sockaddr *)&their_addr, \
        sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(1);
    }

    const char *msg1 = "authenticate \"\"\n";
    if (send(sockfd, msg1, strlen(msg1), 0) == -1){
        perror("send");
        exit (1);
    }
    printf("Msg sent: %s\n", msg1);
    sleep(1);

    if ((numbytes=recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';
    printf("Received in pid=%d, text=: %s \n",getpid(), buf);
    sleep(1);

    const char *msg2 = "SIGNAL SHUTDOWN\n";
    if (send(sockfd, msg2, strlen(msg2), 0) == -1){
        perror("send");
        exit (1);
    }
    printf("Msg sent: %s\n", msg2);
    sleep(1);

    if ((numbytes=recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';
    printf("Received in pid=%d, text=: %s \n",getpid(), buf);
    sleep(1);

    close(sockfd);

    return 0;
}

