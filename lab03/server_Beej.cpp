/***********************************************************************
* Program:
*    Lab RockSrv, Rock/Paper/Scissors with Sockets - Server Code
*    Brother Jones, CS 460
* Author:
*    Trevor Adams
* Summary:
*    Server code for the RPS game over TCP using berkley sockets
************************************************************************/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
//  #include <signal.h>
using namespace std;

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold
#define MAXBUF 99

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
   if (sa->sa_family == AF_INET)
   {
      return &(((struct sockaddr_in*)sa)->sin_addr);
   }

   return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
   int sockfd;      // listen on sock_fd
   int p1_fd;      // new connection on new_fd
   int p2_fd;      // new connection on new_fd
   struct addrinfo hints;
   struct addrinfo *servinfo;
   struct addrinfo *p;
   struct sockaddr_storage p1_addr; // connector's address information
   struct sockaddr_storage p2_addr; // connector's address information
   socklen_t sin_size;
   socklen_t sin_size2;
   //struct sigaction sa;
   int yes=1;
   char s[INET6_ADDRSTRLEN];
   int rv;

   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE; // use my IP

   if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
   {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      return 1;
   }

   // loop through all the results and bind to the first we can
   for(p = servinfo; p != NULL; p = p->ai_next)
   {
      if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1)
      {
         perror("server: socket");
         continue;
      }

      if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
            sizeof(int)) == -1)
      {
         perror("setsockopt");
         exit(1);
      }

      if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
      {
         close(sockfd);
         perror("server: bind");
         continue;
      }

      break;
   }

   freeaddrinfo(servinfo); // all done with this structure

   if (p == NULL)
   {
      fprintf(stderr, "server: failed to bind\n");
      exit(1);
   }

   if (listen(sockfd, BACKLOG) == -1)
   {
      perror("listen");
      exit(1);
   }

   
   while(1)    // main accept() loop
   {
      printf("server: waiting for connections...\n");
      //Player 1
      sin_size = sizeof p1_addr;
      p1_fd = accept(sockfd, (struct sockaddr *)&p1_addr, &sin_size);
      if (p1_fd == -1)
      {
         perror("accept");
         continue;
      }

      inet_ntop(p1_addr.ss_family,
         get_in_addr((struct sockaddr *)&p1_addr),
         s, sizeof s);
      printf("server: got connection from %s\n", s);
      
      //Player 2.........
      sin_size2 = sizeof p2_addr;
      p2_fd = accept(sockfd, (struct sockaddr *)&p2_addr, &sin_size2);
      if (p2_fd == -1)
      {
         perror("accept");
         continue;
      }

      inet_ntop(p2_addr.ss_family,
         get_in_addr((struct sockaddr *)&p2_addr),
         s, sizeof s);
      printf("server: got connection from %s\n", s);
      // end of player 2.........
      

      send(p1_fd, "Hello, Player1!", 15, 0);
      send(p2_fd, "Hello, Player2!", 15, 0);
      
      // Set up buffer and int for use in while loop
      char buf[MAXBUF];
      char buf2[MAXBUF];
      int numbytes;
      // Loop until they quit
      while(true) {
         if ((numbytes = recv(p1_fd, buf, MAXBUF-1, 0)) == -1){
            perror("send");
            exit(1);
         } 
         buf[numbytes] = '\0';
         printf("server received: '%s'\n", buf);
         
         if ((numbytes = recv(p2_fd, buf2, MAXBUF-1, 0)) == -1){
            perror("send");
            exit(1);
         } 
         buf2[numbytes] = '\0';
         
         //send commands to players
         send(p1_fd, buf2, 1, 0);
         send(p2_fd, buf, 1, 0);
         
         buf[numbytes] = '\0';
         
         printf("server received: '%s'\n", buf2);
         
         if (buf2[0] == 'q' || buf[0] == 'q')
            break;
      }
         
      close(p1_fd);
      close(p2_fd);
   }

   return 0;
}

