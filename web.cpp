#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sys/socket.h> // for socket
#include <netinet/in.h> // sockaddr_in
#include <unistd.h> // for close
#include <netdb.h> // for gethostbyname
#include <iostream>
#include <vector>
#include <pthread.h>




    int glob_s;




 
void send_404(int sock) {
  char recvbuf[1024];
  sprintf(recvbuf, 
  "HTTP/1.0 404 Not found\r\n"
  "Connection: close\r\n"
  "Content-type: text/html\r\n"
  "Content-length: 14\r\n"
  "\r\n"
  "Page not found"
  ); 
  send(sock, recvbuf, strlen(recvbuf), 0);
  close(sock);
}

void process_html(const char* l_file, int socket){

    
  FILE* f=fopen(l_file,"r");
	if (f==NULL){
		std::cout<<"file not opened"<<std::endl;
		send_404(socket);
		close(socket);
	}

	char recvbuf[50000];
  int res;
  fseek(f, 0L, SEEK_END); //указатель на конец файла
  int sz = ftell(f);
  fseek(f, 0L, SEEK_SET);  //указатель на начало файла
  char * htmlpage = new char[sz+1];
  fread(htmlpage, 1, sz, f);
  fclose(f);
  htmlpage[sz] = '\0';
  
  sprintf(recvbuf, 
      "HTTP/1.0 200 OK\r\n"
      "Content-Type: text/html; charset=utf-8\r\n"
      "Content-length: %d\r\n"
      "Connection: close\r\n"
      "\r\n",sz);
          
   //printf("\n%s\n", l_file);
   
   
   int header_length = strlen(recvbuf);
   memcpy(recvbuf+header_length, htmlpage, sz);
   res = send(socket, recvbuf, header_length+sz, 0);
      
   delete [] htmlpage;
      
   printf("[+] Data sent: %d out of %d\n", res, header_length+sz);
    
}

void process_png(const char* localfile, int socket){
    FILE* f=fopen(localfile, "rb");
    if (!(f)) {
        send_404(socket);
        return ;
    }
    char recvbuf[50000];
    int res = 0, sz = 0, i = 0, flag = 0, j = 0, k = 0;
    
    fseek(f, 0L, SEEK_END);//указатель на конец файла
    sz = ftell(f);  
    sprintf(recvbuf, 
		  "HTTP/1.0 200 OK\r\n"
		  "Content-Type: image/png\r\n"
		  "Content-length: %d\r\n"
		  "Connection: close\r\n"
		  "\r\n",
    sz
    );
    printf("\nfile: %s\n", localfile); 
    k = strlen(recvbuf);
    res = send(socket, recvbuf, k, 0);
    if(res < k) {
      printf("[-] Can't send data to remote peer\n");
      return;
    } 
    
    fseek(f, 0L, SEEK_SET);
    while(!feof(f)){
        memset(recvbuf, 0, sizeof(recvbuf));
        for (i = 0; i < 1024; i++){
            if (feof(f)) {
                flag = 1;
                recvbuf[i]='\0';
                break;
            }
            fscanf(f,"%c", &recvbuf[i]);
        } 
        res += send(socket, recvbuf, 1024, 0);
        j += 1024;
        if(res < k + j) {
          printf("[-] Can't send data from png_file\n");
          return;
        } 
    }
    k += j;
    if (flag){
    	res += send(socket, recvbuf, strlen(recvbuf), 0); 
      k += strlen(recvbuf);
    }
    if(res < k) {
      printf("[-] Can't send data from png_file\n");
      return;
    }   
    printf("[+] Data sent from png_file %d out of %d\n",res, k);
    fclose(f);
    return ;
}



void *work(void* sock) {
   char recbuf[10000]; 
  int r=0;
  
  int* so=(int*)sock;
  r = recv(*so, recbuf, sizeof(recbuf)-1, 0);
  if(r <= 0) {
      printf("[-] Can't receive data\n");
      send_404(*so); 
      close(*so);
      
  } 
  recbuf[r] = '\0';
  printf("[+] Received %d bytes: %s\n", r, recbuf);
  
  
  //отделение названия файла от пришедшего GET-запроса
	char * p = strchr(recbuf, ' ');
	if(!p) {
		std::cout<<"HERE 1 !"<<std::endl;
		send_404(*so); 
		close(*so);
	}
	while (*p==' ') p++;
	char * s = strchr(p, ' ');
	if(!s) {
		std::cout<<"HERE 2 !"<<std::endl;
		send_404(*so);
		close(*so);
	}
	//send_404(sock);
	//send(sock, "Sorry, page not found\r\n", strlen("Sorry, page not found\r\n"), 0);
	//std::cout<<"HERE!"<<std::/home/student/Desktop/evm_4/web.cpp;
	
	char filename [256];
	strncpy (filename, p, (int)(s-p));
	//std::cout<<filename<<std::endl;
	filename [(int)(s-p)] = '\0';
	//std::cout<<filename<<std::endl;
	printf("REQUESTED: %s\n", filename) ;
	if(strcmp(filename, "/") == 0) {
		std::cout<<"HERE!"<<std::endl;
		strcpy(filename,"/file.html");
	}
	
	char l_file[256];
	sprintf(l_file,"%s",filename+1);
	//std::cout<<l_file<<std::endl;
	

	if(strstr(l_file, ".html")!=NULL){ 
								std::cout<<"OK, I'm in HTML processing.. !"<<std::endl;
                process_html(l_file, *so);
              return 0;
            }
            if(strstr(l_file, ".png")!=NULL){ 
            		std::cout<<"OK, I'm in PNG processing.. !"<<std::endl;
                process_png(l_file, *so);
               return 0;
            }
				
	
	return 0;
}



int main(int , char **) {
	int w=0;
  const unsigned short port = 8071;
  int sock, sock_peer, ip_h;
  struct sockaddr_in addr, peer_addr;
  struct linger linger_opt = { 1, 0 };
 
  int count = 0;
  int* pts;

  std::vector <int> itervector;
  socklen_t peer_addr_size = sizeof(peer_addr);
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    printf("[-] Can't create socket\n");
    return -1;
  }
  pts = (int*) malloc (1 * sizeof(glob_s));
  
  memset(&addr, 0, sizeof(addr));
  addr.sin_addr.s_addr = htonl( INADDR_ANY );
  addr.sin_port = htons( port );
  addr.sin_family = AF_INET;
  if (bind (sock, (const struct sockaddr *)&addr, sizeof(addr) ) != 0) {
    printf("[-] Can't bind socket to port\n");
    free(pts);
    close(sock);
    return -1;
  }
  if (setsockopt(sock, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt)) != 0) {
    printf("[-] Can't linger socket\n");
    free(pts);
    close(sock);
    return -1;
  }
  if (listen( sock, 2 ) != 0) {
    printf("[-] Can't set listen queue for socket\n");
    free(pts);
    close(sock);
    return -1;
  }
  printf("[+] Server listening on port %d\n", port);

  pthread_t thread;
  while(1) {
				sock_peer = accept(sock, (struct sockaddr*) &peer_addr, &peer_addr_size);
				if (sock_peer < 0) {
				  printf("[-] Can't accept new connection\n");
				  free(pts);
				  close(sock);
				  return -1;
				} 
				ip_h = ntohl(peer_addr.sin_addr.s_addr);

				printf("\n[+] New connection accepted from %d.%d.%d.%d on port %d\n",  (ip_h >> 24) & 0xff, (ip_h >> 16) & 0xff, (ip_h >> 8) & 0xff, ip_h & 0xff,       htons(peer_addr.sin_port));
				 *pts = sock_peer;
				
				 
			
				if (count == 0){
						std::cout<<"HERE 0!"<<std::endl;
						itervector.push_back(1);
						count++;
						w=pthread_create(&thread, 0, work, pts+0);
						if (w){
							printf("[-] Can't create thread!\n");
							close(sock) ; 
						}
						w=pthread_detach (thread);
						if (w){
							  printf("[-] Can't detach\n");
						    close(sock) ;}
					}
				else{
				for (int i = 0; i < count; i++){
					std::cout<<"HERE!"<<std::endl;

				       //количество подключений check
				
							if (itervector[i] > 30)
								printf("Too many connections\n");
							else 
							{
								w=pthread_create(&thread, 0, work, pts+0);
								if (w)
								{
									printf("[-] Can't create thread!\n");
									close(sock) ;
								}
								w=pthread_detach (thread);
								if (w){ 
											printf("[-] Can't detach\n");
								     close(sock) ;
								}
								itervector[i] += 1;

							}
						}
			
				}
    }
 
    //process_http(sock_peer);

   
   free(pts);
  close(sock);
  return 0;
} 

