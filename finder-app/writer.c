#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>  
#include <string.h>
#include <libgen.h> 


int writer(char *writefile, char *writestr){

    FILE *file;
    size_t ret_fwrite;
    if(writefile==NULL || writestr==NULL){
        syslog(LOG_ERR,"Program exited with the error message ERROR : either dir or serchstring is not provided!! Please provide inputs properly");
        
        return 1;
    }

    file = fopen(writefile , "w");
    openlog("writer", LOG_CONS|LOG_PERROR|LOG_PID , LOG_USER);
    if(file==NULL){
        syslog(LOG_ERR,"Program exited with the error message %s",strerror(errno));
        return 1;
    }
    syslog(LOG_DEBUG,"Writing %s to %s",writestr,basename(writefile));
    ret_fwrite=fwrite(writestr, sizeof(char), strlen(writestr), file);
    
    if(ret_fwrite!=strlen(writestr)){
        syslog(LOG_ERR,"Program exited with the error message %s",strerror(errno));
        return 1;
    }

    return 0;

}

int main(int argc, char *argv[]){
    
    return writer(argv[1],argv[2]);
    
}