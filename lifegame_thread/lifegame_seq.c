/*
* The Life Game
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NLOOP	(10000)
#define N	(8192/4)
#define M	(8192/4)
#define ALIVE	(1)
#define DEAD	(0)


typedef int Grid[N + 2][M + 2];

void *func(void *arg);
#define T 16
struct Pint{
	const Grid * pCur;
	Grid * pNext;
	int n;
};

void computeNextGen(const Grid * pCur, Grid * pNext)
{
	pthread_t th[T];
	int i;
	struct Pint pin[T];
	for(i = 0; i < T; i++){
		int r=0;
		pin[i].pCur=pCur;
		pin[i].pNext=pNext;
		pin[i].n=N/T*i;
		r=pthread_create(&th[i], NULL, func, (void *) &pin[i]);
		if(r != 0)
		perror("new thread");
	}

	for(i = 0; i < T; i++)
		pthread_join(th[i], NULL);

}

void *func(void *arg){
	struct Pint * p = (struct Pint *) arg;
	int i, j;
	for (i = 1 + p->n; i <= N/T + p->n; ++i) {
		for (j = 1; j <= M; ++j) {
			int count = 0;

			/* NW neighbor */
			if ((*p->pCur)[i - 1][j - 1] == ALIVE)
			count++;
			/* N neighbor */
			if ((*p->pCur)[i - 1][j] == ALIVE)
			count++;
			/* NE neighbor */
			if ((*p->pCur)[i - 1][j + 1] == ALIVE)
			count++;
			/* W neighbor */
			if ((*p->pCur)[i][j - 1] == ALIVE)
			count++;
			/* E neighbor */
			if ((*p->pCur)[i][j + 1] == ALIVE)
			count++;
			/* SW neighbor */
			if ((*p->pCur)[i + 1][j - 1] == ALIVE)
			count++;
			/* S neighbor */
			if ((*p->pCur)[i + 1][j] == ALIVE)
			count++;
			/* SE neighbor */
			if ((*p->pCur)[i + 1][j + 1] == ALIVE)
			count++;

			if (count <= 1 || count >= 4)
			(*p->pNext)[i][j] = DEAD;
			else if ((*p->pCur)[i][j] == ALIVE &&
			(count == 2 || count == 3))
			(*p->pNext)[i][j] = ALIVE;
			else if ((*p->pCur)[i][j] == DEAD && count == 3)
			(*p->pNext)[i][j] = ALIVE;
			else
			(*p->pNext)[i][j] = DEAD;
		}
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	int cur = 0;
	static Grid g[2];
	int i, j, n;
	srand(time(NULL));

	printf("\033[2J");	/* clear screen */
	for (i = 0; i <= N + 1; ++i) {
		for (j = 0; j <= M + 1; ++j) {
			g[0][i][j] = rand() & 1;
		}
		printf("Initializing g[%6d]...\r", i);
	}

	for (n = 0; n < NLOOP; n++) {
		printf("\033[2J");	/* clear screen */
		printf("n = %d\n", n);
		for (i = 1; i <= N >> 5 ; ++i) {
			for (j = 1; j <= M >>3 ; ++j) {
				printf("%c", g[cur][i][j] == ALIVE ? '@' : ' ');
			}
			printf("\n");
		}
		computeNextGen(&g[cur], &g[(cur + 1) & 1]);
		cur = (cur + 1) & 1;
	}

	return 0;
}
