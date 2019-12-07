#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


void* sending(void* sock)
{
  char* response;     
  int sockfd = *(int*)sock;
  size_t response_size = 4096;
  while(1)
  {
    bzero(&response, response_size);  //Empty the buffer
    //Send to server
    getline(&response, &response_size, stdin);
    if(write(sockfd, response,response_size) < 0){
      perror("write");
      exit(1);
    }
    bzero(response, response_size);
  }
}

int main(int argc, char* argv[]){

  //Check if correct number of arguments are passed
  if (argc < 4)
  {
    printf("Please follow this format:./client.c hostaddress port clientname.\n");
    exit(1);
  }
  char* hostname =argv[1];    //the hostname we are looking up
  short port=atoi(argv[2]);      //the port we are connecting on
  char* client_name = argv[3];
  struct addrinfo *result;    //to store results
  struct addrinfo hints;      //to indicate information we want
  int sock, s;                      //socket file descriptor
  char response[4096];  
  size_t response_size = 4096;

  memset(&hints,0,sizeof(struct addrinfo));  //zero out hints
  hints.ai_family = AF_INET; //we only want IPv4 addresses
  //Convert the hostname to an address
  if( (s = getaddrinfo(hostname, NULL, &hints, &result)) != 0){
    fprintf(stderr, "getaddrinfo: %s\n",gai_strerror(s));
    exit(1);
  }
  struct sockaddr_in* saddr_in = (struct sockaddr_in *)result->ai_addr;
  saddr_in->sin_port = htons(port);

  //open a socket
  if( (sock = socket(AF_INET, SOCK_STREAM, 0))  < 0){
    perror("socket");
    exit(1);
  }
  
  //connect to the server
  if(connect(sock, (struct sockaddr *)saddr_in, sizeof(*saddr_in)) < 0){
    perror("connect");
    exit(1);
  }
  send(sock, client_name, strlen(client_name), 0);

  pthread_t t1, t2;
  pthread_create(&t1, NULL,  sending, (void*)&sock); 
  int n;
  while(1)
  {
    if (n = read(sock, response, sizeof(response)) > 0)
    {
      printf("%s\n", response);
      bzero(response,response_size);
    }
    else if (n = read(sock, response, sizeof(response)) == 0){
      break;
    }
  }

  if (read(sock, response, sizeof(response)) == 0){
    printf("Closing client... \n");
    close(sock); //close the socket
    return 0;
  }
  pthread_join(t1, NULL);

  return 0;
}