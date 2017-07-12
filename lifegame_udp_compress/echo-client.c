#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "lifegame_seq.c"
int
main()
{
  int sock;
  int cur = 0;
  static Grid g[2];
  int i, j, n;
  char bnu[16]={'\0'};
  char buf[10] = {'\0'};
  int ko = 1;
  srand(time(NULL));
  for(i = 0; i <= N + 1; ++i){
    for(j = 0; j <= M + 1; ++j){
      g[0][i][j] = rand() & 1;
    }
  }
  struct sockaddr_in addr;

  sock = socket(AF_INET, SOCK_DGRAM, 0);

  addr.sin_family = AF_INET;
  addr.sin_port = htons(12345);
  addr.sin_addr.s_addr = inet_addr("192.168.10.90");

  for (n = 0; n <NLOOP; n++){
    sendto(sock, "\033[2J", sizeof("\033[2J"), 0, (struct sockaddr *)&addr, sizeof(addr));
    sprintf(bnu, "n = %d\n", n);
    sendto(sock, bnu, sizeof(bnu), 0, (struct sockaddr *)&addr, sizeof(addr));
    for (i = 1; i <= N >> 7; ++i) {
      for (j = 1; j <= M >>5 ; ++j) {
        if((g[cur][i][j] == ALIVE && g[cur][i][j+1] == ALIVE)||
        (g[cur][i][j] == DEAD && g[cur][i][j+1] == DEAD)){
          ko++;
        }else{
          if(g[cur][i][j] == ALIVE){
            sprintf(buf, "@%d", ko);
          }else{
            sprintf(buf, ".%d", ko);
          }
          sendto(sock, buf, sizeof(buf), 0, (struct sockaddr *)&addr, sizeof(addr));
          ko = 1;
        }
      }
      if(g[cur][i][j] == ALIVE){
        sprintf(buf, "@%d", ko);
      }else{
        sprintf(buf, ".%d", ko);
      }
      ko=1;
      sendto(sock, buf, sizeof(buf), 0, (struct sockaddr *)&addr, sizeof(addr));
      sendto(sock, "\n", sizeof("\n"), 0, (struct sockaddr *)&addr, sizeof(addr));
    }
    computeNextGen(&g[cur], &g[(cur + 1) & 1]);
    cur = (cur + 1) & 1;
  }

  close(sock);

  return 0;
}
