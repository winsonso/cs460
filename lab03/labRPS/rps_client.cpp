/****************************************************************************
* Program:
*    Lab RockClientT2, Rock/Paper/Scissors with Sockets - Client Code
*    Brother Jones, CS 460
* Author:
*    Winson So
* Summary:
*    This is a simple client side program.
*
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>

/**********************************************************************
* To compare one character at the same time
***********************************************************************/
bool compareOne(char const* input, char const* against) 
{
   return toupper(input[0]) == against[0];
}

/**********************************************************************
* To compare two characters at the same time
* This is necicary because I send 2-character messages.
***********************************************************************/
bool compareTwo(char const* input, char const* against) 
{
   return toupper(input[0]) == against[0] &&
      toupper(input[1]) == against[1];
}

/**********************************************************************
 * Print the results of who won and lost based on what the
 * server sent us.
***********************************************************************/
bool printResults(const char *buff) {
   if (compareTwo(buff, "YW")) 
   {
      printf("\tYou won!\n");
      return false;
   }
   if (compareTwo(buff, "YL")) 
   {
      printf("\tYou lost!\n");
      return false;
   }
   if (compareTwo(buff, "YT")) 
   {
      printf("\tYou tie!\n");
      return false;
   }
   else 
   {
      printf("Other Player Quit\n", buff);
      return true;
   }
}

int main(int argc, char *argv[])
{
   int sockfd, portno, n;
  	struct sockaddr_in serv_addr;
  	struct hostent *server;
        
	// Setup
  	char buffer[256];
        
  	if (argc < 3) {
           fprintf(stderr,"usage %s hostname port\n", argv[0]);
           exit(0);
  	}
        
  	portno = atoi(argv[2]);
        
  	sockfd = socket(AF_INET, SOCK_STREAM, 0);
        
  	if (sockfd < 0)
           perror("ERROR opening socket");
        
  	server = gethostbyname(argv[1]);
        
  	if (server == NULL) {
           fprintf(stderr,"ERROR, no such host\n");
           exit(0);
  	}

        
  	bzero((char *) &serv_addr, sizeof(serv_addr));

  	serv_addr.sin_family = AF_INET;
        
  	bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
        
  	serv_addr.sin_port = htons(portno);
        
  	int con_status = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

  	if (con_status < 0)
           perror("ERROR connecting");

  	system("clear");
  	printf("COMMANDS:\n");
  	printf(" r -> play Rock\n");
  	printf(" p -> play Paper\n");
  	printf(" s -> play Scissors\n");
  	printf(" e -> Exit the game\n");
  	printf("\n");
  	printf("**** NOTE: Type first letter only!! ****\n");


        while(1) 
        {
           printf("> ");

           bzero(buffer, 3);
           
           fgets(buffer, 255, stdin);

           if (n < 0)
              perror("ERROR w   riting to socket");
           
           if (compareOne(buffer, "P")) 
           {
              strcpy(buffer, "PA");
           } 
           else if (compareOne(buffer, "S")) 
           {
              strcpy(buffer, "SC");
           } 
           else if (compareOne(buffer, "R")) 
           {
              strcpy(buffer, "RO");
           } 
           else if (compareOne(buffer, "E") || compareOne(buffer, "Q")) 
           {
              strcpy(buffer, "EX");
              printf("You quit the game.\n");
           } 
           else 
           {
              printf("Invalid Command!!\n");
           }
           
           n = write(sockfd, buffer, strlen(buffer));
           
           bzero(buffer, 3);
           
           n = read(sockfd,buffer,3);
           
           if (n < 0)
              perror("ERROR reading from socket");
           
           if (printResults(buffer)) break;
        }
        
        close(sockfd);
        
        return 0;
}
