#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <map>
#include <set>

#define MAXLINE 4096					
#define LISTENQ 1024	

using namespace std;

bool doEcho = false;
bool doBroadcast = false;
set<int> userSd;                    //set of user socket descriptors
set<int> idPool;                    //set of availabe user IDs
mutex userSd_lock;
mutex idPool_lock;

void usage(void){
    puts("syntax : echo-server <port> [-e[-b]]");
    puts("sample : echo-server 1234 -e -b");
}

void printErrExit(const char* msg){
	perror(msg);
	exit(0);
}

void clientThread(int userId, int sd){
    char recvBuf[MAXLINE+1];
    char msgBuf[MAXLINE+32];
    int msgLen;

    while(true){
        int res = recv(sd, recvBuf, MAXLINE, 0);
        if (res == 0 || res == -1) {
			printf("Received %d\n",res);
			break;
		}

        recvBuf[res] = 0;
        if(doEcho){
            sprintf(msgBuf, "[Echo] %s\n", recvBuf);
            msgLen = strlen(msgBuf);	
            if(send(sd, msgBuf, msgLen, 0)!=msgLen){
                printErrExit("Echo error");
            }
        }
        if(doBroadcast){
            sprintf(msgBuf, "[Broadcast: User%d] %s\n", userId, recvBuf);
            msgLen = strlen(msgBuf);

            userSd_lock.lock();
            for(int targetSd : userSd){
                if(send(targetSd, msgBuf, msgLen, 0)!=msgLen){
                    printErrExit("Broadcast error");
                }
            }	
            userSd_lock.unlock();
        }
    }

    printf("Connection with [User%d] closed\n",userId);

    idPool_lock.lock();
    idPool.insert(userId);          //return user ID resource
    idPool_lock.unlock();

    userSd_lock.lock();
    userSd.erase(sd);
    userSd_lock.unlock();
    close(sd);
} 

int main(int argc, char* argv[]){

    if((argc<2) || (argc>4)){
        usage();
        return -1;
    } 

    uint16_t port = atoi(argv[1]);
    if(port<0 || port > 65535){
		printErrExit("Invalid port number");
    }
    for(int i=2;i<argc;i++){
        if (strcmp(argv[i], "-e") == 0) {
				doEcho = true;
				continue;
			}
			if (strcmp(argv[i], "-b") == 0) {
				doBroadcast = true;
				continue;
			}
    } 

    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd < 0){
        printErrExit("Socket error");
    }

    struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

    if(bind(sd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
		printErrExit("Bind error");
    }
	if(listen(sd, LISTENQ) < 0){
		printErrExit("Listen error");
    }

    for(int i=0;i<LISTENQ;i++){         //serve at most 1024 users
        idPool.insert(i);
    }

    while(idPool.empty() == false){
        struct sockaddr_in cliAddr;
		socklen_t len = sizeof(cliAddr);
        puts("Waiting for Connection Request");
		int cliSd = accept(sd, (struct sockaddr *)&cliAddr, &len);
        if(cliSd == -1){
            printErrExit("Accept error");
        }

        idPool_lock.lock();
        int userId = *idPool.begin();
        idPool.erase(userId);
        idPool_lock.unlock();

        userSd_lock.lock();
        userSd.insert(cliSd);
        userSd_lock.unlock();
        
        new thread(clientThread, userId, cliSd);

        char ip[16];
        inet_ntop(AF_INET, &cliAddr.sin_addr, ip, 16);
        printf("Connected with [User%d] at %s\n", userId, ip);
    }

    puts("idPool exhauseted, terminating server");
    close(sd);
    return 0;
}