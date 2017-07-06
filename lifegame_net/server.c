#define PORTNO_BUFSIZE  16
#define BUFFERSIZE 5000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <math.h>


extern int tcp_acc_connect(int portno, char *du);
extern int connect_server(int portno, char *du);
extern int fdopen_sock(int sock, FILE **inp, FILE **outp);
extern int http_receive_request(FILE *in, char buf[], int size);

int main(int argc, char *argv[])
{
  int portno=8080;
  int err;
  err = connect_server(portno, argv[1]);
  return (err);
}

int tcp_acc_port(int portno)
{
  struct addrinfo hints, *ai;
  char portno_str[PORTNO_BUFSIZE];
  int err, s;

  snprintf(portno_str, sizeof(portno_str), "%d", portno);
  memset(&hints, 0, sizeof(hints));
  ai = NULL;
  hints.ai_family = PF_INET;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_socktype = SOCK_STREAM;
  if((err = getaddrinfo(NULL, portno_str, &hints, &ai))){
    fprintf(stderr, "bad portno %d? (%s)\n", portno, gai_strerror(err));
    goto error0;
  }
  if((s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0){
    perror("socket");
    goto error1;
  }
  if(bind(s, ai->ai_addr, ai->ai_addrlen) < 0){
    perror("bind");
    fprintf(stderr, "Port number %d\n", portno);
    goto error2;
  }
  if(listen(s, 5) < 0){
    perror("listen");
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

int connect_server(int portno, char *du)
{
  //unsigned char key[3]="ac";
  char rbuf[BUFFERSIZE];
  int acc, com;
  int key;
  acc = tcp_acc_port(portno);
  if(acc < 0)
  exit(-1);

  while(1){
    FILE *in, *out;
    printf("Accepting incoming connections...\n");
    if((com = accept(acc, 0, 0)) <0){
      perror("accept");
      exit(-1);
    }else{
      printf("%d\n",com);
    }

    if(fdopen_sock(com,&in, &out) <0){
      perror("fdopen()");
      return (-1);
    }

    http_receive_request(in,rbuf,BUFFERSIZE);
    fclose(in);
    fclose(out);
  }
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

int http_receive_request(FILE *in, char buf[], int size)
{
  while(buf != NULL){
  fgets(buf, size, in);

  printf("%s", buf);
}
  return 0;
}
