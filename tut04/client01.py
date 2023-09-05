
# Import socket module
import socket            
 
# Create a socket object
s = socket.socket()        
 
# Define the port on which you want to connect
port = 17346              
 
# connect to the server on local computer
s.connect(('127.0.0.1', port))
 
s.send("Hello IITP!".encode())

print(s.recv(1024).decode())

# close the connection
s.close()  