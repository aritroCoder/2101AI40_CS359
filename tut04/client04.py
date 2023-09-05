# Import socket module
import socket

# Create a socket object
s = socket.socket()        

# Define the port on which you want to connect
port = 17343          
 
# connect to the server on local computer
s.connect(('127.0.0.1', port))

arr = [2, 5, 3, 6]
stri = ""
for i in arr:
  stri = stri + str(i) + " "
print("sending array: "+stri)

s.send(stri.encode())

print("sum = " + s.recv(1024).decode())

# close the connection
s.close()  