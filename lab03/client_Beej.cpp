/***********************************************************************
* Program:
*    Lab RockClient, Rock/Paper/Scissors with Sockets - Client Code
*    Brother Jones, CS 460
* Author:
*    Trevor Adams
* Summary:
*    Client code for the RPS game over TCP using berkley sockets
************************************************************************/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <string>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>
using namespace std;

#define PORT "3490" // the port client will be connecting to 

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

int main(int argc, char *argv[])
{
   int sockfd;
   int numbytes;  
   char buf[MAXDATASIZE];
   struct addrinfo hints;
   struct addrinfo *servinfo;
   struct addrinfo *p;
   int rv;
   char s[INET6_ADDRSTRLEN];

   if (argc != 2)
   {
      fprintf(stderr,"usage: client hostname\n");
      exit(1);
   }

   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
   {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      return 1;
   }

   // loop through all the results and connect to the first we can
   for (p = servinfo; p != NULL; p = p->ai_next)
   {
      if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1)
      {
         perror("client: socket");
         continue;
      }

      if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
      {
         perror("client: connect");
         close(sockfd);
         continue;
      }

      break;
   }

   if (p == NULL)
   {
      fprintf(stderr, "client: failed to connect\n");
      return 2;
   }

   inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
         s, sizeof s);
   printf("client: connecting to %s\n", s);

   freeaddrinfo(servinfo); // all done with this structure

   if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
   {
      perror("recv");
      exit(1);
   }

   buf[numbytes] = '\0';
   printf("client: received '%s'\n",buf);
   
   // Game stuff...
   char input; 
   cout << "----- Welcome to Rock Paper Scissors -----\n" 
        << "\tEnter 'r' for Rock\n"
        << "\tEnter 'p' for Paper\n"
        << "\tEnter 's' for Scissors\n"
        << "\tEnter 'q' to exit program.\n" << endl;
   while (input != 'q') {
      cout << "Your command > ";
      cin >> input;

      if (input != 'r' && input != 'p' && input != 's' && input != 'q') 
      {
         cout << "Invalid input - Enter 'r', 'p', 's', or 'q'" << endl;
      }
      
      //Send and recieve commands
      if(send(sockfd, &input, 1, 0) == -1)
         perror("recv");
         
      if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1){
         perror("send");
         exit(1);
      }
            
      buf[numbytes] = '\0';
      printf("Other player command: '%s'\n",buf);
      
      if (buf[0] == 'q') {
         cout << "Other player left" << endl;
         break;
      }
         
      
      if (buf[0] == input) 
         cout << "It's a draw" << endl;
      else if (buf[0] == 'p' && input == 'r') 
         cout << "You Lost" << endl;
      else if (buf[0] == 's' && input == 'p') 
         cout << "You Lost" << endl;
      else if (buf[0] == 'r' && input == 's') 
         cout << "You Lost" << endl;
      else if (input != 'q')
         cout << "You won!" << endl;
      
   }

   close(sockfd);

   return 0;
}

