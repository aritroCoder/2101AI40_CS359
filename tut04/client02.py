# Import socket module
import socket            
 
# Create a socket object
s = socket.socket()        
 
# Define the port on which you want to connect
port = 17347             
 
# connect to the server on local computer
s.connect(('127.0.0.1', port))
 
s.send("you must be the change you wish to see in the world".encode())

print("string size: " + s.recv(1024).decode())

# close the connection
s.close()  