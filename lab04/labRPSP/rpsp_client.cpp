/****************************************************************************
* Program:
*    Lab RPSPclient, RPSP Rock/Paper/Scissors Protocol - Client Code
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

#define ROCK 0
#define PAPER 1
#define SCISSORS 2
#define QUIT 3
#define BAD_MOVE -1

/**********************************************************************
 * This is a helper that writes an error out to the console and 
 * exits the program.
 ***********************************************************************/
void error(const char *msg)
{
   perror(msg);
   exit(0);
}

/**********************************************************************
 * This is a helper method for comparing one character at the same time
 ***********************************************************************/
bool compareOne(char const* input, char const* against) {
   return tolower(input[0]) == tolower(against[0]);
}
/**********************************************************************
 * This converts the cstring command into an integer.
 ***********************************************************************/
int toInt(char buff[]) {
   if (compareOne(buff, "r")) return ROCK;
   if (compareOne(buff, "p")) return PAPER;
   if (compareOne(buff, "s")) return SCISSORS;
   if (compareOne(buff, "q")) return QUIT;
   else                       return BAD_MOVE;
}
/**********************************************************************
 * This is basically the Tic-tac toe logic. It tells who wins by
 * returning a -1 for p1 win, 1 for p2 win and a 0 for a tie.
 * bad moves always loose. One bad moves results in a tie.
 ***********************************************************************/
int whoWins(int p1, int p2) {
   if (p1 == p2) return 0;
   
   else if (p1 == SCISSORS && p2 == PAPER    ||
            p1 == PAPER   && p2 == ROCK     ||
            p1 == ROCK    && p2 == SCISSORS  )
      return -1;
   else 
      return 1;
}
/**********************************************************************
 * This will print the results of who won and lost based on what the
 * server sent us.
 ***********************************************************************/
bool printResults( char *sdbuff, char *buff) {
   int p1 = toInt(sdbuff);
   int p2 = toInt(buff);
   int side = whoWins(p1, p2);
   
   if (p1 == QUIT || p2 == QUIT) {
      printf("One of the players quit the game!\n");
      return true; // end the game
   }
   else if (p1 == BAD_MOVE || p2 == BAD_MOVE) {
      printf("Bad input form one of the players...\n");
      return true;
   }
   else if (side == 0) {
      printf("\t Tie!\n");
      return false;
   }
   else if (side == -1) {
      printf("\t You Won!\n");
      return false;
   } else {
      printf("\t You Lost!\n");
      return false;
   }
}

int main(int argc, char *argv[])
{
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  char buffer1[225];

  /*************
   * Setup
   *************/
  char buffer[2];
  char sendbuffer[2];
  
  if (argc < 3) {
     fprintf(stderr,"usage %s hostname port\n", argv[0]);
     exit(0);
  }
  
  portno = atoi(argv[2]);
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0)
     error("ERROR opening socket");
  
  server = gethostbyname(argv[1]);
  
  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host\n");
    exit(0);
  }
  
  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  
  serv_addr.sin_family = AF_INET;
  
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

  serv_addr.sin_port = htons(portno);
  
  int con_status = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  
  if (con_status < 0)
     error("ERROR connecting");
  
  int n1;
  system("clear");
  bzero(buffer1, 225);
  n1 = recv(sockfd, buffer1, 225, 0);
  if (n1 < 0) error("ERROR reading from socket (p1)\n");
  printf("%s\n",buffer1);
         
  printf("COMMANDS:\n");
  printf(" r - play Rock\n");
  printf(" p - play Paper\n");
  printf(" s - play Scissors\n");
  printf(" q - Quit the game\n");
  printf("\n");
  printf("**** NOTE: Type first letter only!! ****\n");

  /********************************
   * Procol (MAIN LOOP)
   ********************************/
  while(1) {
     
     
     printf("> ");
     
    bzero(buffer, 2);
    
    fgets(buffer, 256, stdin);
    

    if (n < 0)
       error("ERROR writing to socket");
    
    if (compareOne(buffer, "P")) {
       buffer[0] = 'p';
    } else if (compareOne(buffer, "S")) {
       buffer[0] = 's';
    } else if (compareOne(buffer, "R")) {
       buffer[0] = 'r';
    } else if (compareOne(buffer, "Q")) {
       buffer[0] = 'q';
       printf("You quit the game.\n");
    } else {
       printf("Invalid command. This round is invalid.\n");
    }
    
    printf("\n... Waiting for server...\n");
    
    n = send(sockfd, buffer, 1, 0);
    strcpy(sendbuffer,buffer);
    //printf("send %s\n",sendbuffer);
    
    if (buffer[0] == 'q')
       break;

    bzero(buffer, 1);
    
    n = read(sockfd,buffer, 1);
    
    if (n < 0)
       error("ERROR reading from socket");
    //printf("recv %s\n",buffer);
    if (printResults(sendbuffer,buffer)) break;
  }

  /*******************
   * Close
   ********************/

  close(sockfd);

  return 0;
}
