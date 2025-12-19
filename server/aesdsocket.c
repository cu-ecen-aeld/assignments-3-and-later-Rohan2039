#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <syslog.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h> 
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT "9000"
#define MAXDATASIZE 100
int listenfd;
FILE *fd;
struct addrinfo *res;
static void signal_handler ( int signal_number )
{
    

    if ( signal_number == SIGINT || signal_number == SIGTERM ) {
        syslog(LOG_INFO, "Caught signal, exiting");
        close(listenfd);
        freeaddrinfo(res);
        fclose(fd);
        remove("/var/tmp/aesdsocketdata");
        closelog();
    }

}

int main(){

    struct addrinfo hints;
    struct sockaddr_storage client_addr;
    struct sockaddr_in6 *addr_in6;
    struct sockaddr_in *addr_in;
    socklen_t addr_size;
    int listenfd, connfd;
    char ip6str[INET6_ADDRSTRLEN];

    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct sigaction new_action;
    memset(&new_action,0,sizeof(struct sigaction));
    new_action.sa_handler=signal_handler;

    if( sigaction(SIGTERM, &new_action, NULL) != 0 ) {
        return -1;
    }
    if( sigaction(SIGINT, &new_action, NULL) ) {
        return -1;
    }

    if(getaddrinfo(NULL,PORT, &hints, &res)!= 0)
    {
        return -1;
    }
    fd = fopen("/var/tmp/aesdsocketdata", "a+");
    openlog("aesdsocket", LOG_PID | LOG_CONS, LOG_USER);

    listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(listenfd < 0)
    {
        freeaddrinfo(res);
        return -1;
    }
    if(bind(listenfd,res->ai_addr,res->ai_addrlen) < 0)
    {
        close(listenfd);
        freeaddrinfo(res);
        return -1;
    }

    if(listen(listenfd, 5) < 0)
    {
        close(listenfd);
        freeaddrinfo(res);
        return -1;
    }

    addr_size = sizeof(client_addr);
    connfd = accept(listenfd, (struct sockaddr *)&client_addr, &addr_size);
    if(connfd < 0)
    {
        close(listenfd);
        freeaddrinfo(res);
        return -1;
    }

    if(client_addr.ss_family==AF_INET){
        // Connection established with an IPv4 client
        addr_in = (struct sockaddr_in *)&client_addr;
        syslog(LOG_INFO, "Accepted connection from %s", inet_ntoa(addr_in->sin_addr));
    }
    else if(client_addr.ss_family==AF_INET6){
        // Connection established with an IPv6 client
        addr_in6 = (struct sockaddr_in6 *)&client_addr;
        inet_ntop(AF_INET6, &addr_in6->sin6_addr, ip6str, sizeof(ip6str));
        syslog(LOG_INFO, "Accepted connection from %s", ip6str);
    }

    while(1){
        do{
            char buf[MAXDATASIZE],ret_fwrite,ret_fread;
            int numbytes = recv(connfd,buf,MAXDATASIZE-1,0);
            if(numbytes < 0){

                close(listenfd);
                close(connfd);
                freeaddrinfo(res);
                return -1;
            }
            else if(numbytes > 0){
                buf[numbytes] = '\0';
                strcat(buf,"\n");
                ret_fwrite = fwrite(buf,sizeof(char),numbytes,fd);
                
                if(ret_fwrite < strlen(buf)){
                    close(listenfd);
                    close(connfd);
                    freeaddrinfo(res);
                    return -1;
                }
                fseek(fd, 0, SEEK_END);
                long size = ftell(fd);
                char *file_content = malloc(size + 1);
                if(file_content == NULL){
                    close(listenfd);
                    close(connfd);
                    freeaddrinfo(res);
                    return -1;
                }
                ret_fread = fread(file_content, sizeof(char), size, fd);
                file_content[size] = '\0';
                if(send(connfd,file_content,size,0) < 0){
                    close(listenfd);
                    close(connfd);
                    freeaddrinfo(res);
                    return -1;
                }
                free(file_content);
            }
        }while(1);

        close(connfd);
        if(client_addr.ss_family==AF_INET){
            syslog(LOG_INFO, "Closed connection from %s", inet_ntoa(addr_in->sin_addr));
        }
        else if(client_addr.ss_family==AF_INET6){

            inet_ntop(AF_INET6, &addr_in6->sin6_addr, ip6str, sizeof(ip6str));
            syslog(LOG_INFO, "Closed connection from %s", ip6str);
        }
    }


    return 0;
}