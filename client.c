#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ev.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<wiringPi.h>
#define PORT 8888
#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];
void send_cb(struct ev_loop *loop, struct ev_io *watcher, int revent);
int main()
{
		int sd;
		wiringPiSetup();
    pinMode(7,OUTPUT);
		struct sockaddr_in addr;
    struct ev_loop *loop = ev_default_loop(0);
    struct ev_io connect_watcher;
		//int addr_len=sizeof(addr);
		if((sd=socket(AF_INET,SOCK_STREAM,0))<0)
		{
				perror("socket error");
				return -1;
		}
		bzero(&addr,sizeof(addr));

		addr.sin_family=AF_INET;
		addr.sin_port=htons(PORT);
		addr.sin_addr.s_addr=inet_addr("115.159.64.71");

		if(connect(sd,(struct sockaddr *)&addr,sizeof(addr))<0)
		{
				perror("connect error");
				return -1;
		}
    ev_io_init(&connect_watcher,send_cb, sd, EV_WRITE);
    ev_io_start(loop, &connect_watcher);
		while(1) {
        ev_loop(loop, 0);
		}
		return 0;
}
void send_cb(struct ev_loop *loop, struct ev_io *watcher, int revent)
{
    int send_num;
    int read;
    if(EV_ERROR & revent) {
        printf("error event in read");
        return;
    }
    //printf("please input message : \n");
    //scanf("%s",buffer);
    strcpy(buffer,"\0");
    system("sleep 0.5");
    send_num = send(watcher->fd, buffer, BUFFER_SIZE, 0);
    if(send_num == 0) {
        ev_io_stop(loop,watcher);
        free(watcher);
        perror("peer might closing");
        return;
    } else {
        buffer[send_num] = '\0';
        send_num = (int)strlen(buffer);
        printf("send the message: %s\n",buffer);
    }
    read = recv(watcher->fd, buffer, BUFFER_SIZE, 0);
    if(read == 0) {
        ev_io_stop(loop,watcher);
        perror("peer might closing");
        return;
    } else {
        int i =0;
        char signal[10];
        buffer[read] = '\0';
        for(i=0;i<4;i++) {
           signal[i]=buffer[i];
        }
        signal[4]='\0';
        if(!strcmp(signal,"1111")){
	    digitalWrite(7,LOW);
	}else if(!strcmp(signal,"0000")){
	    digitalWrite(7,HIGH);
	} else {
	   printf("error command\n");
	}
        printf("get message :%s\n",signal);
    }
}
