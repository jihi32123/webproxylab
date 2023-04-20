/* $begin sharing */
#include "csapp.h"
#define N 2
void *thread(int vargp, int a);

char **ptr;  /* Global variable */ //line:conc:sharing:ptrdec

int main() 
{
    int i;  
    pthread_t tid;
    char *msgs[N] = {
	"Hello from foo",  
	"Hello from bar"   
    };

    ptr = msgs; 
    for (i = 0; i < N; i++)  
        Pthread_create(&tid, NULL, thread, i, 2); 
    Pthread_exit(NULL); 
}

void *thread(int vargp, int b) 
{
    int myid = vargp;
    printf("%d\n", b);
    static int cnt = 0; //line:conc:sharing:cntdec
    printf("[%d]: %s (cnt=%d)\n", myid, ptr[myid], ++cnt); //line:conc:sharing:stack
    return NULL;
}
/* $end sharing */