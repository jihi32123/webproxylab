#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";


void read_header(int fd, char*host, char* port, char* url);
void read_requesthdrs(rio_t *rp);
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg);
void doit(int fd);
void send_request(char* host, char* port, char* uri, int connfd);

void sigchld_handler(int sig) //line:conc:echoserverp:handlerstart
{
    while (waitpid(-1, 0, WNOHANG) > 0)
	;
    return;
} //line:conc:echoserverp:handlerend


int main(int argc, char **argv) 
{
  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  char buf[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  Signal(SIGCHLD, sigchld_handler);
  listenfd = Open_listenfd(argv[1]);

  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); //line:netp:tiny:accept
    printf("connect\n");
    Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, 
                port, MAXLINE, 0);
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    if (Fork() == 0) { 
	    Close(listenfd); /* Child closes its listening socket */
      doit(connfd); 
	    Close(connfd);   /* Child closes connection with client */ //line:conc:echoserverp:childclose
	    exit(0);         /* Child exits */
	  }
    Close(connfd);     
  }
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

// /* tiny 서버에 요청하는 함수*/
// void send_request(char *host, char *port, char *uri, int client_fd) 
// {
//     int server_fd; // 소켓 식별자(서버랑 연결됨)
//     char client_buf[MAXLINE],server_buf[MAXLINE]; // 값 저장할 애들
//     rio_t client_rio, server_rio;
//     Rio_readinitb(&server_rio, server_fd); // 서버 I/O 초기화
//     sprintf(server_buf, "GET /%s HTTP/1.1\r\n\r\n", uri);

//     server_fd = Open_clientfd(host, port);
//     Rio_writen(server_fd, server_buf, strlen(server_buf)); // 서버한테 헤더 보내기
    
//     size_t n; 
//     while ((n = Rio_readlineb(&server_rio, client_buf, MAXLINE))!= 0) {
//       printf("%s \n", client_buf);
//       Rio_writen(client_fd, client_buf, n); // 이미지 파일의 경우 strlen())을 사용하면 안됨 그래서 파일을 따로 받고 호출함)
//     } 
//     Close(server_fd); //line:netp:echoclient:close             
// }

void send_header(int fd, char *uri, int connfd) 
{
  char buf[MAXLINE];

  /* Print the HTTP response headers */
  sprintf(buf, "GET /%s HTTP/1.1\r\n\r\n", uri);
  Rio_writen(fd, buf, strlen(buf));
  
  rio_t rio;

  size_t n;
  Rio_readinitb(&rio, fd);                   
  // 이미지 파일의 경우 strlen())을 사용하면 안됨 그래서 파일을 따로 받고 호출함)
  while ((n = Rio_readlineb(&rio, buf, MAXLINE))!= 0) {
    printf("%s \n", buf);
    Rio_writen(connfd, buf, n);
  }           

}


/* tiny 서버에 요청하는 함수*/
void send_request(char *host, char *port, char *uri, int fd) 
{
    int clientfd; // 소켓 식별자
    char buf[MAXLINE]; // 값 저장할 애들
    rio_t rio;
    // port = strtok(end_host , "/");
    printf("host = <%s>, port =<%s>, url =<%s>\n", host, port, uri);
    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);
    send_header(clientfd,uri, fd);
    Close(clientfd); //line:netp:echoclient:close
}


/* $end proxy */
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE];

    /* Print the HTTP response headers */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Print the HTTP response body */
    sprintf(buf, "<html><title>Tiny Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor=""ffffff"">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>The Tiny Web server</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}
/* $end clienterror */