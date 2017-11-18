#############################################################################
# Program:
#    Lab PythonWebServerT2, Computer Communication and Networking
#    Brother Jones, CS 460
# Author:
#    Winson So
# Summary:
#    This program is to create and run a webserver capable of returning
#    requested webpages written in python2
#
##############################################################################
#
# Changes made to my code for the re-submission:
#    Nothing changes..
#
##############################################################################

#!/usr/bin/python

# This is TCPServer.py

import os.path
import sys
from socket import *


DEFAULT_PORT = 6789
serverPort = int(sys.argv[1]) if len(sys.argv) == 2 else DEFAULT_PORT
serverSocket = socket(AF_INET, SOCK_STREAM)
serverSocket.bind(('', serverPort))
serverSocket.listen(1)
print 'The Server is ready to receive'

def contentType(file):
   f = str(file)
   if(f.endswith("htm")):
      return "text/html"
   elif(f.endswith("html")):
      return "text/html"
   elif(f.endswith("jpeg")):
      return "image/jpeg"
   elif(f.endswith("jpg")):
      return "image/jpeg"
   elif(f.endswith("gif")):
      return "image/gif"
   elif(f.endswith("php")):
      return "text/html"
   else:
      return "application/octet-stream"

def getFileContent (fileName):
    f = open('.'+ fileName)
    data = f.read()
    f.close()
    return data + '\r\n'

try:
   while 1:
      connectionSocket, addr = serverSocket.accept()
      req = connectionSocket.recv(1024).split()
     
      if req[0] == 'GET' :
         fileName = req[1]
         if os.path.exists('./' + fileName):
            print 'yes'
            statusLine = "HTTP/1.0 200 OK \r\n"
            contentTypeLine = "Content-type: "+contentType(fileName)+" \r\n\r\n"
            entityBody = getFileContent(fileName)
         else:
            print 'no'
            statusLine = "HTTP/1.0 404 NOT FOUND \r\n"
            contentTypeLine = "Content-type: text/html \r\n\r\n"
            entityBody = '<HTML><HEAD><TITLE>Not Found</TITLE></HEAD><BODY>Not Found</BODY></HTML>\r\n'
         
         connectionSocket.send(statusLine + contentTypeLine + entityBody)

      connectionSocket.close()

except KeyboardInterrupt:
   print "\nClosing Server"
   serverSocket.close()

