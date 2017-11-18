/****************************************************************************
* Program:
*    Lab RockSrvT2, Rock/Paper/Scissors with Sockets - Server Code
*    Brother Jones, CS 460
* Author:
*    Winson So
* Summary:
*    A simple server in the internet domain using TCP and the port
*    number is passed as an argument.
*
*****************************************************************************
* Changes made to my code for the re-submission:
*   
*   -I added comments such that they would help me remember what I did to
*    write this program in another six months.
*
*
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>

#define BSIZE 256
#define BSIZE_LESS 255
#define ROCK 0
#define PAPER 1
#define SCISSORS 2
#define BAD_MOVE -1

/**********************************************************************
 * This will compare the first two letters of a cstring. This is used
 * to check agianst the two letter codes that our protocol uses
 ***********************************************************************/
bool compareTwo(char const* input, char const* against) 
{
   return toupper(input[0]) == against[0] &&
      toupper(input[1]) == against[1];
}
/**********************************************************************
 * This converts the cstring command into an integer.
 ***********************************************************************/
int toInt(char buff[]) 
{
   if (compareTwo(buff, "RO")) return ROCK;
   if (compareTwo(buff, "PA")) return PAPER;
   if (compareTwo(buff, "SC")) return SCISSORS;
   else                        return BAD_MOVE;
}
/**********************************************************************
 * This is basically the Tic-tac toe logic. It tells who wins by
 * returning a -1 for p1 win, 1 for p2 win and a 0 for a tie.
 * bad moves always loose. Two bad moves results in a tie.
 ***********************************************************************/
int whoWins(int p1, int p2) 
{
   if (p1 == p2) return 0;
   
   else if (p2 == BAD_MOVE                   ||
            p1 == SCISSORS && p2 == PAPER    ||
            p1 == PAPER   && p2 == ROCK      ||
            p1 == ROCK    && p2 == SCISSORS    )
      return -1;
   else 
      return 1;
}

/*****************************************************************
 * Will check p1 and p2 and write the responses to buff1 and buff2 
 ******************************************************************/
char returnResponse(int p1, int p2, int client_1, int client_2) 
{
   int side = whoWins(p1, p2);
   
   const void* wbuff = "YW";
   const void* lbuff = "YL";
   const void* tbuff = "YT";
   
   int n1, n2;
   
   if (side == 0) 
   {
        printf("Tie!\n");
        n1 = write(client_1, tbuff, 3);
        n2 = write(client_2, tbuff, 3);
   }
   else if (side == -1) 
   {
        printf("Player 1 wins\n");
        n1 = write(client_1, wbuff, 3);
        n2 = write(client_2, lbuff, 3);
   } else 
   {
        printf("Player 2 wins\n");
        n1 = write(client_1,  lbuff, 3);
        n2 = write(client_2,  wbuff, 3);
   }

   if (n1 < 0) perror("ERROR writing to socket p1\n");
   if (n2 < 0) perror("ERROR writing to socket p2\n");
}

/************************
 * MAIN
 ************************/
int main(int argc,  char *argv[]) 
{
   
    struct sockaddr_in serv_addr,  cli_addr;
    socklen_t clientLen = sizeof(cli_addr);
    int server, client_1, client_2, port;
    char buffer1[BSIZE];
    char buffer2[BSIZE];
    int n1, n2;
    
    //check if user passes setup a port 
    if (argc < 2) 
    {
       fprintf(stderr, "ERROR,  no port provided\n");
       exit(1);
    }
    
    // setup a port
    port = atoi(argv[1]); 
    
    // create socket
    server = socket(AF_INET,  SOCK_STREAM,  0);
    if (server < 0)
        perror("ERROR opening socket\n");
    
    // setup socket address structure
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    
    if (bind(server,  (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
       perror("ERROR on binding\n");
    
    // convert the socket to listen for incoming connections
    if (listen(server,SOMAXCONN) < 0) {
       perror("listen");
       exit(-1);
    }
    
    // To accept clients
    printf("Waiting for a player 1...\n");
    client_1 = accept(server, (struct sockaddr *) &cli_addr, &clientLen);
    if (client_1 < 0)
       perror("Error Trying to accept player 1!\n");
    else
       printf("Player 1 is connected!!!\n");

    printf("Waiting for a player 2...\n");
    client_2 = accept(server, (struct sockaddr *) &cli_addr, &clientLen);
    if (client_2 < 0)
       perror("Error Trying to accept player 2!\n");
    else
       printf("Player 2 is connected!!!\n");
    
    int round = 0;

    while(1) 
    {
       printf( "\n------------- ROUND %d -------------\n", ++round);
       
       memset(buffer1,0,BSIZE);
       memset(buffer2,0,BSIZE);
       
       printf("Waiting for Player 1's Move...\n");
       n1 = read(client_1, buffer1, BSIZE_LESS);
       if (n1 < 0) perror("ERROR reading from socket (p1)\n");
       printf("Received!\n");
       
       printf("Waiting for Player 2's Move...\n");
       n2 = read(client_2, buffer2, BSIZE_LESS);
       if (n2 < 0) perror("ERROR reading from socket (p2)\n");
       printf("Received!\n");
       
       if(buffer1[0] == 'E' || buffer2[0] == 'E') 
       {
          printf("Closing the connections\n");
          break;
       }
       
       printf("Player 1-(%s) v.s Player 2-(%s)\n", buffer1, buffer2);
       
       int p1 = toInt(buffer1);
       int p2 = toInt(buffer2);
       
       returnResponse(p1, p2, client_1, client_2);
       
    }
    
    close(client_1);
    close(client_2);
    
    close(server);

    return 0;
}
