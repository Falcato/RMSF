#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include "cam_control1.c"

#define max(A,B) ((A)>=(B)?(A):(B))

void accao(char * comando){

	if(strcmp(comando,"PAN_LEFT")==0){ controlos('D');printf("Move Left\n");}
	else if(strcmp(comando,"PAN_RIGHT")==0){ controlos('C');printf("Move Right\n");}
	else if(strcmp(comando,"TILT_DOWN")==0){ controlos('B');printf("Move Down\n");}
	else if(strcmp(comando,"TILT_UP")==0){ controlos('A');printf("Move Up\n");}
	else if(strcmp(comando,"RESET_PAN")==0){ controlos('P');printf("Pan Reset\n");}
	else if(strcmp(comando,"RESET_TILT")==0){ controlos('T');printf("Tilt Reset\n");}
	else printf("Invalid\n");

}

int main(){
	
	int fd, newfd;
	struct sockaddr_in addr;
	int n;
	char buffer[128];
	
	
	
	if ((fd=socket(AF_INET, SOCK_STREAM, 0))==-1) exit(1);
	
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(9000);
	if(bind(fd, (struct sockaddr*)&addr, sizeof(addr))==-1)
		exit(1);
		
	if(listen(fd, 5)==-1) exit(1);
	
	//VARIAVEIS SELECT
	fd_set rfds;
	int maxfd;
	newfd=0;
	
	//LOOP PRINCIPAL
	while(1){

		FD_ZERO(&rfds);
		FD_SET(fd,&rfds);
		maxfd=max(maxfd,fd);
		
		if (newfd > 0){
			FD_SET(newfd,&rfds);
			maxfd=max(maxfd,newfd);
		}
		
		
		//SELECT
		if(select((maxfd+1),&rfds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval*)NULL)==-1) exit(1);
		
		//RECEBEU LIGAÇAO
		if(FD_ISSET(fd, &rfds)){

			socklen_t addrlen=sizeof(addr);
		
			//ACEITA A NOVA LIGAÇAO
			newfd=accept(fd,(struct sockaddr*)&addr,&addrlen);
			//if(newfd==-1) exit(1);
			
			//LE DA NOVA LIGAÇAO
			bzero(buffer,128);
			n=read(newfd,buffer,128);
			buffer[n-1] = '\0';
			//if(n<=0) exit(1);
			printf("Recebi do listen:%s\n",buffer);
			
			accao(buffer);
		}
		//RECEBE DA LIGAÇAO JA EXISTENTE
		if(FD_ISSET(newfd, &rfds)){

			bzero(buffer,128);
			n=read(newfd,buffer,128);
			buffer[n-1] = '\0';
			if(n<=0){newfd=-1;printf("cliente foi embora\n");continue;}
			printf("Recebi do newfd:%s\n",buffer);
			
			accao(buffer);
		}
						
			
	}
	close(fd);
	exit(0);
}
