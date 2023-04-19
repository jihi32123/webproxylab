#include <stdio.h>
#include "csapp.h"
#include "cache.h"
/* Recommended max cache and object sizes */

#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";
static cache *proxy_cache;
void read_header(int fd, char*host, char* port, char* url);
void read_requesthdrs(rio_t *rp);
void doit(int fd);
void send_request(char* host, char* port, char* uri, int connfd);
void *thread(void *varagp);

int main(int argc, char **argv) 
{
  int listenfd, *connfd;
  char hostname[MAXLINE], port[MAXLINE];
  proxy_cache = new_cache();
  char buf[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  pthread_t tid;

  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  listenfd = Open_listenfd(argv[1]);
  while (1) {
      clientlen = sizeof(clientaddr);
      connfd = Malloc(sizeof(int));
      *connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); //line:netp:tiny:accept
      Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, 
                port, MAXLINE, 0);
      printf("Accepted connection from (%s, %s)\n", hostname, port);
      Pthread_create(&tid, NULL, thread, connfd);
    }
}
void *thread(void *vargp){
    int connfd = *((int *)vargp);
    printf("connect\n");
    Pthread_detach(pthread_self());
    Free(vargp);
    doit(connfd);  
    Close(connfd);   
    return NULL;
}
/* 주요 로직 */
void doit(int connfd){
  /* 1. 클라이언트한테 받은 요청의 헤더를 읽어서 저장하자  */   
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE]; // 헤더 정보 저장할 변수들
  rio_t rio;

  Rio_readinitb(&rio, connfd);                                    // Rio I/O 초기화
  if (!Rio_readlineb(&rio, buf, MAXLINE))                         // Rio I/O 못 읽으면 반환 
      return;
  printf("%s", buf);                                              // 헤더 전부 출력
  sscanf(buf, "%s %s %s", method, uri, version);                  // 헤더 읽기                                       
  read_requesthdrs(&rio);                                         // 남은 헤더 처리하기 

  /* 2. 받은 헤더 URI를 파싱하자 */
  char *end_host[MAXLINE], *end_port[MAXLINE], *end_uri[MAXLINE];   // 파싱해서 저장할 정보들
  parse_uri(uri, end_host, end_port);                        // 파싱하기

  /* 3. 파싱한 URI를 재구성해서 서버에 전송하고 바로 전달하자 */
  send_request(end_host, end_port, uri, connfd);
}

/* 헤더 처리하는 함수 */
void read_requesthdrs(rio_t *rp) 
{
  char buf[MAXLINE];

  Rio_readlineb(rp, buf, MAXLINE);
  printf("%s", buf);
  while(strcmp(buf, "\r\n")) {          //line:netp:readhdrs:checkterm
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
  }
  return;
}

/* $begin parse_uri */
int parse_uri(char *uri, char *host, char *port) 
{
  // char *ptr;
  char *token;
  
  // host 찾기
  token = strtok(uri, "/"); // "//" 뒤 문자열은 호스트 명
  token = strtok(NULL, ":");
  strcpy(host,++token);
  
  // port 찾기
  token = strtok(NULL, "/"); // ":"로 구분된 포트 찾기
  if(token != NULL){  
    strcpy(port,token);
  }
  else{
    strcpy(port, "80");  // 포트가 없는 경우 기본값 80으로 설정
    printf("token = %s\n", token);
  }
  // uri 경로 찾기
  token = strtok(NULL, "");
  if (token != NULL) {
    strcpy(uri, token);
  } else {
    strcpy(uri, "/");  // uri 경로가 없는 경우 "/"로 설정
  }
}

/* tiny 서버에 요청하는 함수*/
void send_request(char *host, char *port, char *uri, int fd) 
{
  
  int clientfd; // 소켓 식별자
  char buf[MAXLINE]; // 값 저장할 애들

  // static char data[MAXLINE] = ""; //home.html 저장할 공간 
  // static int check = 0;
  char data[MAX_OBJECT_SIZE] = ""; //home.html 저장할 공간 
  printf("host = <%s>, port =<%s>, url =<%s>\n", host, port, uri);
  
  int c;
  if(c = find_cache(proxy_cache,uri,data)){
    printf("uri = %s\n", uri);
    printf("result find = %d", c);
    printf("=======캐시!===========\n");
    Rio_writen(fd, data, MAXLINE); // 캐시 버퍼 그대로 보내주기 
  }
  else{
    /* Print the HTTP response headers */
    clientfd = Open_clientfd(host, port);
    sprintf(buf, "GET /%s HTTP/1.1\r\n\r\n", uri);
    Rio_writen(clientfd, buf, strlen(buf));
    
    rio_t rio;
    size_t n;
    Rio_readinitb(&rio, clientfd);                   
    // 이미지 파일의 경우 strlen())을 사용하면 안됨 그래서 파일을 따로 받고 호출함)
    int cache_check = 1;
    while ((n = Rio_readlineb(&rio, buf, MAXLINE))!= 0) {
      printf("%s \n", buf);
      Rio_writen(fd, buf, n);
      
      if(strlen(data) + strlen(buf) < MAX_OBJECT_SIZE)
        strcat(data, buf); // 저장할 데이터
      else
        cache_check = 0;
    }
    Close(clientfd); //line:netp:echoclient:close
    if(cache_check == 1) 
      insert_cache(proxy_cache, uri, data);
  }

}
/* $end proxy */