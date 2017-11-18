#############################################################################
# Program:
#    labFinalProj
#    Brother Jones, CS 460
# Author:
#    Winson So
# Summary:
#    This program a server program.
#
# Note: If you put #!/usr/bin/python as the first line of this file and
#       make the program executable, the submit command will not be happy.
#       Run your Python program using:  python yourCode.py
#
#############################################################################

#!/usr/bin python3
""" A simple chat TCP server """
import socket
import select

def broadcast_data(message):
    """ Sends a message to all sockets in the connection list. """
    # Send message to everyone, except the server.
    for sock in CONNECTION_LIST:
        if sock != SERVER_SOCKET:
            try:
                sock.sendall(message) # send all data at once
            except Exception as msg: # Connection was closed. Errors
                print(type(msg).__name__)
                sock.close()
                try:
                    CONNECTION_LIST.remove(sock)
                except ValueError as msg:
                    print("{}:{}".format(type(msg).__name__, msg))

CONNECTION_LIST = []
RECV_BUFFER = 4096 # Advisable to keep it as an exponent of 2
PORT = 6789

SERVER_SOCKET = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
SERVER_SOCKET.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
SERVER_SOCKET.bind(("", PORT)) # empty addr string means INADDR_ANY

print("Listening...")
SERVER_SOCKET.listen(5) # 5 connections

CONNECTION_LIST.append(SERVER_SOCKET)
print("Server started!")
while True:
    # Get the list sockets which are ready to be read through select
    READ_SOCKETS, WRITE_SOCKETS, ERROR_SOCKETS = select.select(CONNECTION_LIST, [], [])
    for SOCK in READ_SOCKETS: # New connection
        # Handle the case in which there is a new connection recieved through server_socket
        if SOCK == SERVER_SOCKET:
            SOCKFD, ADDR = SERVER_SOCKET.accept()
            CONNECTION_LIST.append(SOCKFD) # add socket descriptor
            # Adding \r to prevent message overlapping when another user
            # types it's message.
            name = SOCKFD.recv(RECV_BUFFER)
            print("\rClient ({0}, {1}, {2}) connected".format(ADDR[0], ADDR[1], name.decode()))
            broadcast_data("{0} entered room\n"
                           .format(name.decode()).encode())
        else: # Some incoming message from a client
            try: # Data recieved from client, process it
                DATA = SOCK.recv(RECV_BUFFER)
                if DATA:
                    ADDR = SOCK.getpeername() # get remote address of the socket
                    DATA = DATA.decode().split('-')
                    message = "\r[{}]: {}".format(DATA[0], DATA[1])
                    print(message, end="")
                    broadcast_data(message.encode())
            except Exception as msg: # Errors happened, client disconnected
                print(type(msg).__name__, msg)
                print("\rClient ({0}) disconnected.".format(DATA[0]))
                broadcast_data("\rClient ({0}, {1}:{3}) is offline\n"
                               .format(ADDR[0], ADDR[1], DATA[0]).encode())
                SOCK.close()
                try:
                    CONNECTION_LIST.remove(SOCK)
                except ValueError as msg:
                    print("{}:{}.".format(type(msg).__name__, msg))
                continue

SERVER_SOCKET.close()
