#include "aesdsocket.h"

extern int listenfd, connfd;
extern struct addrinfo *res;
extern FILE *fp;
static void signal_handler ( int signal_number )
{
    
    if ( signal_number == SIGINT || signal_number == SIGTERM ) {
        syslog(LOG_INFO, "Caught signal, exiting");
        close(listenfd);
        freeaddrinfo(res);
        fclose(fp);
        remove("/var/tmp/aesdsocketdata");
        closelog();
	    exit(EXIT_FAILURE); 
    }

}

int siganl_init(){

    struct sigaction new_action;
    memset(&new_action,0,sizeof(struct sigaction));
    new_action.sa_handler=signal_handler;

    if( sigaction(SIGTERM, &new_action, NULL) != 0 ) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if( sigaction(SIGINT, &new_action, NULL)!= 0 ) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}