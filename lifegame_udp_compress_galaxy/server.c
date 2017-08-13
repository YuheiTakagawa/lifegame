#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int
main()
{
  int sock;
  struct sockaddr_in addr;

  char buf[10];
  char *num;
  int i = 0;

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  printf("sock");
  addr.sin_family = AF_INET;
  addr.sin_port = htons(12345);
  addr.sin_addr.s_addr = INADDR_ANY;


  bind(sock, (struct sockaddr *)&addr, sizeof(addr));

  memset(buf, 0, sizeof(buf));
  printf("Binding...\n");
  while(1){
    recv(sock, buf, sizeof(buf), 0);
    if(buf[0]=='@'||buf[0]=='.'){
      num = &buf[1];
      for(i = 0; i < atoi(num); i++)
      printf("%c",buf[0]);
    }else{
      printf("%s", buf);
    }
  }
  close(sock);
  return 0;
}
