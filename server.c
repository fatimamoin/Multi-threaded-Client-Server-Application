#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "linked_list.h"

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* server_work(void* sock)
{
  size_t BUF_SIZE = 4096;
  char response[BUF_SIZE];           //read in 4096 byte chunks
  int sockfd = *(int*)sock;
  bzero(&response, BUF_SIZE);
  char response2[BUF_SIZE]; 
  //Read from client
  if((read(sockfd, response, BUF_SIZE)) < 0){
    perror("read");
    exit(1);
  }
  
  pthread_mutex_lock(&lock);
  if(push(sockfd, response)==NULL)
  { 
    write(sockfd, "This client name is already taken.", 50);
    close(sockfd);
  };
  else
  {
    printf("Added %s\n.", response);
  }
  pthread_mutex_unlock(&lock);
  
  int n;

  while(1)
  {
 
  while(n = read(sockfd,response2,BUF_SIZE) > 0 )  
  {
    if(n <0)
    {
        printf("Error reading from socket ");
    }

    if(strncmp(response2,"/list",5)==0)
    {
     pthread_mutex_lock(&lock);
      node_t* temp = head;
      while (temp != NULL)
      {
         write(sockfd,temp->name, strlen(temp->name));
         temp = temp -> next;
         write(sockfd, " ",1);
      }
      pthread_mutex_unlock(&lock);
    }
    else if (strncmp(response2,"/msg",4) == 0)
    {
      int count = 0;
      char* client_name = NULL;
      char* recieve[4096] ={0};
      char* split = strtok(response2, " ");
       int fd;
       while (split)
       {
        recieve[count++] = split;
        split = strtok (NULL," ");
       }
       recieve[count] = '\0';
       if (recieve[1] != 0)
       {
         pthread_mutex_lock(&lock);
          node_t* temp = head;
          while (temp != NULL)
          {
            if(strncmp(temp->name,recieve[1], sizeof(recieve[1])) == 0)
            {
              client_name = temp-> name;
              fd = temp-> fd;
               break;
            }
            temp = temp -> next;
          }
        pthread_mutex_unlock(&lock);
          if (client_name == NULL)
          {
             write(sockfd, "Invalid client name. Please try again.", 100);
          }
          else
          { 
            int count=2;    
            char msg[BUF_SIZE];    
            while(recieve[count]!=NULL)
            { 
              strcat(msg,recieve[count]);
              strcat(msg," ");
              count++;
            }
            write(fd, msg , sizeof(msg));
            bzero(msg, sizeof(msg));
          }

       }
     else
     {
      write(sockfd, "Please follow this format: /msg client_name your_message", 23);
     }
    }
    else if (strncmp(response2,"/quit",5) == 0){
      printf("Closing %s from server... \n", response);
      pthread_mutex_lock(&lock);
      delete_node(sockfd);
      pthread_mutex_unlock(&lock);
      close(sockfd);
    }
    else
    {
      write(sockfd, "Invalid command.", 30);
    }
    bzero(response2, BUF_SIZE);  
  }

}
  printf("ENDING THREAD\n");
}
 
int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    printf("No port provided. Please try again.\n");
    exit(1);
  }
  short port=atoi(argv[1]);      //the port we are to bind to
  struct sockaddr_in saddr_in, client_saddr_in;  //socket interent address of server
  int server_sock, client_sock;                     //socket file descriptor

  //set up the address information
  saddr_in.sin_family = AF_INET;
  saddr_in.sin_port = htons(port);

  //open a socket
  if( (server_sock = socket(AF_INET, SOCK_STREAM, 0))  < 0){
    perror("socket");
    exit(1);
  }
  //bind the socket
  if(bind(server_sock, (struct sockaddr *) &saddr_in, sizeof(saddr_in)) < 0){
    perror("bind");
    exit(1);
  }
  //ready to listen, queue up to 5 pending connectinos
  if(listen(server_sock, 5)  < 0){
    perror("listen");
    exit(1);
  }
  printf("Listening On: %s:%d\n", inet_ntoa(saddr_in.sin_addr), ntohs(saddr_in.sin_port));
  
  socklen_t saddr_len = sizeof(struct sockaddr_in); //length of address
  //accept incoming connections
pthread_t t1;
  while(client_sock = accept(server_sock, (struct sockaddr *)&client_saddr_in, &saddr_len))
  {
   
    printf("Connection From: %s:%d (%d)\n", inet_ntoa(client_saddr_in.sin_addr), //address as dotted quad
         ntohs(client_saddr_in.sin_port),     //the port in host order
         client_sock);                        //the file descriptor number
    
    if(client_sock < 0)
    {      
      perror("accept");
        exit(1);
    };  
    
    if (pthread_create(&t1, NULL,  server_work, (void*)&client_sock))
    {
      perror("Thread could not be created");

    };

  }
   pthread_join(t1, NULL);
  printf("Closing socket\n\n");
  close(server_sock); //close the socket

  return 0; //success
}
