#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

#define MAXLINE 4096					

int sd;                 //socket descriptor

void usage(void){
    puts("syntax : echo-client <ip> <port>");
    puts("sample : echo-client 192.168.10.2 1234");
}

void printErrExit(const char* msg){
	fprintf(stderr, "%s\n", msg);
	exit(0);
}

void sendThread(void){
	char sendBuf[MAXLINE+1]={0,};
    int msgLen;

    while(true){
        printf("Enter your message: ");
        fgets(sendBuf, MAXLINE, stdin);
        msgLen = strlen(sendBuf)-1;
        sendBuf[msgLen] = 0;

        int res = send(sd, sendBuf, msgLen, 0);
        if (res == 0 || res == -1) {
			printf("Received %d\n",res);
            close(sd);
            exit(0);
		}
    }
} 

void recvThread(void){
    char recvBuf[MAXLINE+1]={0,};

    while(true){
        ssize_t res = recv(sd, recvBuf, MAXLINE, 0);
        recvBuf[res] = 0;
        printf("%s",recvBuf);
    }
} 

int main(int argc, char* argv[]){

    if(argc != 3){
        usage();
        return -1;
    }

    struct in_addr ip;
    int res = inet_pton(AF_INET, argv[1], &ip);
	switch (res) {
		case 1: break;
		case 0: puts("invalid network address"); return -1;
		case -1: perror("inet_pton"); return -1;
	}

    uint16_t port = atoi(argv[2]);
    if(port<0 || port > 65535){
		printErrExit("Invalid port number");
    }

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd < 0){
        printErrExit("Socket error");
    }

    struct sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	servAddr.sin_addr = ip;
	memset(&servAddr.sin_zero, 0, sizeof(servAddr.sin_zero));

    if(connect(sd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0){
        printErrExit("Connection error");
    }
    puts("Connection success");
    puts("Press Enter to Exit");
    std::thread t1(recvThread);
    std::thread t2(sendThread);

    while(true);
}


