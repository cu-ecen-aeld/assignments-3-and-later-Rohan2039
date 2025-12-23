#include "aesdsocket.h"


#define PORT "9000"
#define MAXDATASIZE 25000
int listenfd;
FILE *fp;
struct addrinfo *res;
int listenfd, connfd;
char ip6str[INET6_ADDRSTRLEN];

typedef enum{
    CL_LISTEN_FD,
    CL_CONN_FD
}close_fd;

extern int siganl_init();

void get_IP_ADDR_info(struct sockaddr_storage client_addr);
int close_socket(close_fd close_fd_lcl, char *funcname);
int send_client(int connfd, FILE *fp);
void demonise_process();

void get_IP_ADDR_info(struct sockaddr_storage client_addr){
    
    if(client_addr.ss_family==AF_INET){
        struct sockaddr_in *addr_in;
        // Connection established with an IPv4 client
        addr_in = (struct sockaddr_in *)&client_addr;
        strcpy(ip6str, inet_ntoa(addr_in->sin_addr));
        
    }
    else if(client_addr.ss_family==AF_INET6){
        struct sockaddr_in6 *addr_in6;
        // Connection established with an IPv6 client
        addr_in6 = (struct sockaddr_in6 *)&client_addr;
        inet_ntop(AF_INET6, &addr_in6->sin6_addr, ip6str, sizeof(ip6str));
        
    }
}

int close_socket(close_fd close_fd_lcl, char *funcname){

    perror(funcname);
    if(close_fd_lcl == CL_LISTEN_FD){
        close(listenfd);
    }
    else if(close_fd_lcl == CL_CONN_FD){
        close(listenfd);
        close(connfd);
    }
    freeaddrinfo(res);
}

int send_client(int connfd, FILE *fp){
    
    char line[25000];
    fflush(fp);          
    rewind(fp);
    while(fgets(line,sizeof(line),fp)!=NULL)
    {
        if(send(connfd,line,strlen(line),0) < 0)
        {
            close_socket(CL_CONN_FD,"send");
            return -1;
        }
    }
}

void demonise_process(){
    pid_t pid = fork();

    if(pid<0){
        exit(EXIT_FAILURE);
    }
    else if(pid>0){
        exit(EXIT_SUCCESS);
    }
    
    if(setsid()<0){
        exit(EXIT_FAILURE);
    }

    chdir("/");
    umask(0);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}


int main(int argc ,char *argv[]){

    struct addrinfo hints;
    struct sockaddr_storage client_addr;
    
    socklen_t addr_size;

    if(argc == 2 && strcmp(argv[1], "-d")==0){
        printf("Demonising the process\n");
        demonise_process();
    }

    siganl_init();
    fp = fopen("/var/tmp/aesdsocketdata", "a+");
    //journalctl | grep aesdsocket use this to check syslog messages
    openlog("aesdsocket10", LOG_PID | LOG_CONS, LOG_USER);

    //get the address information by calling getaddrinfo
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if(getaddrinfo(NULL,PORT, &hints, &res)!= 0)
    {
        perror("getaddrinfo");
        return -1;
    }
    
    listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(listenfd < 0)
    {
        close_socket(CL_LISTEN_FD,"socket");
        return -1;
    }

    if(bind(listenfd,res->ai_addr,res->ai_addrlen) < 0)
    {
        close_socket(CL_LISTEN_FD,"bind");
        return -1;
    }

    if(listen(listenfd, 5) < 0)
    {
        close_socket(CL_LISTEN_FD,"listen");
        return -1;
    }

    addr_size = sizeof(client_addr);
    while(1){
	    connfd = accept(listenfd, (struct sockaddr *)&client_addr, &addr_size);
        if(connfd < 0)
        {
            close_socket(CL_LISTEN_FD,"accept");
            return -1;
        }

        get_IP_ADDR_info(client_addr);
        syslog(LOG_INFO, "Accepted connection from %s", ip6str);
    
        char buf[MAXDATASIZE];
        size_t ret_fwrite,ret_fread;
        int numbytes;

        while ((numbytes = recv(connfd, buf, sizeof(buf), 0)) > 0) {
            syslog(LOG_INFO, "The string %s", buf);
            fwrite(buf, 1, numbytes, fp);
            fflush(fp);
            if(numbytes == strlen(buf)-1){
                close_socket(CL_CONN_FD,"fwrite");
                return -1;
            }
            if(buf[numbytes-1]=='\n'){  
                send_client(connfd,fp);
            }
        }
        /* normalize record boundary */
        if(numbytes < 0){
            close_socket(CL_CONN_FD,"recv");
            return -1;
        }

        
        syslog(LOG_DEBUG,"Exitting.....");
        close(connfd);

        syslog(LOG_INFO, "Closed connection from %s", ip6str);
        syslog(LOG_DEBUG,"Listeing to other clients.......");
    }
    return 0;
}
