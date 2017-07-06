#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "lifegame_seq.c"

extern int echo_client_one(char *server, int portno);
extern int echo_send_request(FILE *out);
extern int echo_receive_reply(FILE *in, char buf[], int size);
extern int tcp_connect(char *server, int porno);
extern int fdopen_sock(int sock, FILE **inp, FILE **outp);

int main(int argc, char *argv[])
{
	char *server;
	int portno;
	char *message;
	int err;
	if(argc != 3)
	{
		fprintf(stderr,"Usage: %s host port\n",argv[0]);
		exit(-1);
	}
	server = argv[1];
	portno = strtol(argv[2], 0, 10);
	err = echo_client_one(server, portno);
	return (err);
}

#define BUFFERSIZE 1024
int echo_client_one(char *server, int portno)
{
	int sock;
	FILE *in, *out;
	char rbuf[BUFFERSIZE];
	int res;
	sock = tcp_connect(server, portno);
	if(sock < 0)
	return (1);
	if(fdopen_sock(sock, &in, &out) < 0)
	{
		fprintf(stderr, "fdopen()\n");
		close(sock);
		return (1);
	}
	res = echo_send_request(out);
	if(res < 0)
	{
		fprintf(stderr, "fprintf()\n");
		fclose(in);
		fclose(out);
		return (1);
	}
	fclose(in);
	fclose(out);
	return (0);
}

int echo_send_request(FILE *out)
{
	int res;
	int cur = 0;
	static Grid g[2];
	int i, j, n;
	char bnu[16]={'\0'};
	srand(time(NULL));
	for (i = 0; i <= N + 1; ++i) {
		for (j = 0; j <= M + 1; ++j) {
			g[0][i][j] = rand() & 1;
		}
	}
	for (n = 0; n < NLOOP; n++) {
		fprintf(out,"\033[2J");
		sprintf(bnu, "n = %d\n", n);
		fprintf(out, bnu, sizeof(bnu));
		for (i = 1; i <= N >> 7; ++i) {
			for (j = 1; j <= M >>5 ; ++j) {
			fprintf(out, g[cur][i][j] == ALIVE ? "@" : ".");
			}
			fprintf(out, "\n");
		}
		computeNextGen(&g[cur], &g[(cur + 1) & 1]);
		cur = (cur + 1) & 1;
	}
	return (0);
}

int echo_receive_reply(FILE *in, char buf[], int size)
{
	char *res;
	res = fgets(buf, size, in);
	if(res)
	return (strlen(buf));
	else
	return (-1);
}

int fdopen_sock(int sock, FILE **inp, FILE **outp)
{
	int sock2;
	if((sock2=dup(sock)) < 0)
	{
		return (-1);
	}
	if((*inp = fdopen(sock2, "r")) == NULL)
	{
		close(sock2);
		return (-1);
	}
	if((*outp = fdopen(sock, "w")) == NULL)
	{
		fclose(*inp);
		*inp = 0;
		return (-1);
	}
	setvbuf(*outp, (char *)NULL, _IONBF, 0);
	return (0);
}








#define PORTNO_BUFSIZE 16
int tcp_connect( char *server, int portno)
{
	struct addrinfo hints, *ai;
	char portno_str[PORTNO_BUFSIZE];
	int s, err;
	snprintf(portno_str, sizeof(portno_str), "%d", portno);
	memset(&hints, 0 ,sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	if((err = getaddrinfo(server, portno_str, &hints, &ai)))
	{
		fprintf(stderr,"unknown server %s (%s)\n",server, gai_strerror(err));
		goto error0;
	}
	if((s = socket(ai->ai_family, ai->ai_socktype,ai->ai_protocol)) < 0)
	{
		perror("socket");
		goto error1;
	}
	if(connect(s, ai->ai_addr, ai->ai_addrlen) < 0)
	{
		perror(server);
		goto error2;
	}
	freeaddrinfo(ai);
	return (s);
	error2:
	close(s);
	error1:
	freeaddrinfo(ai);
	error0:
	return (-1);
}
