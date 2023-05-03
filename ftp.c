 
#include <stdio.h>
#include<string.h>
 #include <stdlib.h>
 #include <memory.h>
 #include <sys/socket.h> // for socket
 #include <sys/types.h>
 #include <netinet/in.h> // sockaddr_in
 #include <unistd.h> // for close
 #include <arpa/inet.h> // for inet_aton
 #include <netdb.h> // for gethostbyname
 int hx_connect_v4(const char * ip, int port);

 
 int hx_connect_v4(const char * ip, int port)
 {
 int sock, res;
 struct sockaddr_in addr;
 struct hostent *host = NULL;

 // fill structure for input
 memset(&addr, 0, sizeof(addr));
 host = gethostbyname(ip);
 if (host == NULL) {
 return -3;
 }
 // Write resolved IP address of a server to the address structure
 memcpy(&(addr.sin_addr.s_addr), host->h_addr_list[0], 4);
 addr.sin_port = htons(port);
 addr.sin_family = AF_INET;

 // create socket for TCP connection
 sock = socket(AF_INET, SOCK_STREAM, 0);
 if (sock < 0) {
 return -1;
 }

 // connect socket with the remote address
 res = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
 if ( res < 0 ) {
 close(sock);
 return -2;
 }

 return sock;
 }

 int main(void) {
 char t[4][1000];
 // connect to 127.0.0.1 and port 1234
 int sock = hx_connect_v4("exam.1434.ru", 63821);
 if (sock == -1 ) {
 printf("Can't create socket\n");
 return -1;
 } else if (sock == -2 ) {
 printf("Can't connect to requested host\n");
 return -1;
 } else if (sock == -3 ) {
 printf("IP address or Domain name is wrong\n");
 return -1;
 }
 printf("Connection successfull\n");
 

 // send and recv
    recv(sock, t[0], 1000, 0);
    puts(t[0]);
 send(sock, "USER anonymous\r\n", 16, 0);
  recv(sock, t[1], 1000, 0);
    puts(t[1]);
 send(sock, "PASV\r\n", 6, 0);
 recv(sock, t[2], 1000, 0);
    puts(t[2]);
 close(sock);
 printf("%c%c%c%c%c%c", t[2][40],t[2][41],t[2][42],t[2][44],t[2][45], t[2][46]);
//  int sock1 = hx_connect_v4("exam.1434.ru",t[]);
//  if (sock1 == -1 ) {
//  printf("Can't create socket\n");
//  return -1;
//  } else if (sock1 == -2 ) {
//  printf("Can't connect to requested host\n");
//  return -1;
//  } else if (sock1 == -3 ) {
//  printf("IP address or Domain name is wrong\n");
//  return -1;
//  }
//  printf("Connection successfull\n");
//         send(sock1, "RETR c4NkQW.dat\r\n", 17 ,0);
//     recv(sock1, t[3], 1000, 0);
//     puts(t[3]);
//  close(sock1);
 return 0;
 }
