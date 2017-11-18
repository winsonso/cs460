/***********************************************************************
 * Program:
 *    Lab RockClient, Rock/Paper/Scissors with Sockets - Server Code
 *    Brother Jones, CS 460
 * Author:
 *    Cesar de Leon
 * Summary:
 *    This is the server code for the RPS game. This takes messages from
 *    two different clients and computes the results of the game, sending
 *    each client if they won or lost.
 ************************************************************************/

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
#include <iostream>
#include <string>
//  #include <signal.h>
void computeRes(std::string &player1, std::string &player2);

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold

#define MAXDATASIZE 100 // max number of bytes we can get at once

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
  int numbytes;
  char buf[MAXDATASIZE];
   int sockfd;      // listen on sock_fd
   int p1_fd;      // new connection on new_fd
   int p2_fd;      // new connection on new_fd
   struct addrinfo hints;
   struct addrinfo *servinfo;
   struct addrinfo *p;
   struct sockaddr_storage their_addr; // connector's address information
   socklen_t sin_size;
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

   while(true){

   printf("server: waiting for connections...\n");

   std::string player1 = "";
   std::string player2 = "";

     while(player1 == "")    // main accept() loop
       {
         sin_size = sizeof their_addr;
         p1_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
         if (p1_fd == -1)
           {
             perror("accept");
             continue;
           }

         inet_ntop(their_addr.ss_family,
                   get_in_addr((struct sockaddr *)&their_addr),
                   s, sizeof s);
         printf("server: got connection from %s\n", s);

         //reads from client.
         if ((numbytes = recv(p1_fd, buf, MAXDATASIZE-1, 0)) == -1)
           {
             perror("recv");
             exit(1);
           }

         buf[numbytes] = '\0';
         player1 = buf;
         printf("server: received '%s'\n",buf);

         //sends to client.
         // if (send(p1_fd, "Waiting for player 2", 20, 0) == -1)
         //   perror("send");
       }

     while(player2 == "")    // main accept() loop
       {
         sin_size = sizeof their_addr;
         p2_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
         if (p2_fd == -1)
           {
             perror("accept");
             continue;
           }

         inet_ntop(their_addr.ss_family,
                   get_in_addr((struct sockaddr *)&their_addr),
                   s, sizeof s);
         printf("server: got connection from %s\n", s);

         //reads from client.
         if ((numbytes = recv(p2_fd, buf, MAXDATASIZE-1, 0)) == -1)
           {
             perror("recv");
             exit(1);
           }

         buf[numbytes] = '\0';

         printf("server: received '%s'\n",buf);
         player2 = buf;

         //sends to client.
         //if (send(p2_fd, "Getting results!", 16, 0) == -1)
         //  perror("send");
         //std::string results = player1 + player2;

         computeRes(player1, player2);

         if (send(p2_fd, player2.c_str(), player2.length(), 0) == -1)
           perror("send");
         if (send(p1_fd, player1.c_str(), player1.length(), 0) == -1)
           perror("send");
         close(p1_fd);
         close(p2_fd);
       }
   }
   return 0;
}

  void computeRes(std::string &player1, std::string &player2)
  {
    switch (player1[0] - player2[0]) {
    case 0:
      player1 = "draw";
      player2 = "draw";
      break;
    case 'r' - 's':
      player1 = "win";
      player2 = "lose";
      break;
    case 'r' - 'p':
      player1 = "lose";
      player2 = "win";
      break;
    case 's' - 'p':
      player1 = "win";
      player2 = "lose";
      break;
    case 's' - 'r':
      player1 = "lose";
      player2 = "win";
      break;
    case 'p' - 's':
      player1 = "lose";
      player2 = "win";
      break;
    case 'p' - 'r':
      player1 = "win";
      player2 = "lose";
      break;
    default:
      player1 = "error";
      player2 = "error";
      break;
    }
}
