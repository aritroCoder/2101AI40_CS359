# first of all import the socket library
import socket            
 
# next create a socket object
s1 = socket.socket()
s2 = socket.socket()
s3 = socket.socket()
s4 = socket.socket()   
print ("Socket successfully created")
 
# reserve a port on your computer in our
# case it is 12345 but it can be anything
port1 = 17346
port2 = 17347
port3 = 17344
port4 = 17343

s1.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s2.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s3.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s4.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
 
# Next bind to the port
# we have not typed any ip in the ip field
# instead we have inputted an empty string
# this makes the server listen to requests
# coming from other computers on the network
s1.bind(('', port1))
s2.bind(('', port2))
s3.bind(('', port3))
s4.bind(('', port4))
 
# put the socket into listening mode
s1.listen(5)
s2.listen(5)
s3.listen(5)
s4.listen(5)  
print ("socket is listening")           

# Establish connection with client.

# client1
c, addr = s1.accept()    
print ('Got connection from', addr )
print(c.recv(1024).decode())
try:
  c.send("Have a good day".encode())
except:
  print("sending Error at client 1")
c.close()

#divider
print("--------------------------------------------------")

# client2
c2, addr2 = s2.accept()
print ('Got connection from', addr2 )
stri = c2.recv(1024).decode()
print("string recieved: "+ stri)
# count words in str
words = stri.split()
try:
  c2.send(str(len(words)).encode())
except:
  print("sending Error at client 2")
c2.close()

#divider
print("--------------------------------------------------")

#client3
c3, addr3 = s3.accept()
print ('Got connection from', addr3 )
stri = c3.recv(1024).decode()
print("string recieved: "+ stri)
#check if stri is pallindrome
try:
  if stri == stri[::-1]:
    c3.send("String is pallindrome".encode())
  else:
    c3.send("String is not pallindrome".encode())
except:
  print("sending Error at client 3")
c3.close()

#divider
print("--------------------------------------------------")

#client4
c4, addr4 = s4.accept()
print ('Got connection from', addr4 )
stri = c4.recv(1024).decode()
print("string recieved: "+ stri)
nums = stri.split(' ')
sum = 0;
for i in nums:
  if i != '':
    sum = sum + int(i)
try:
  c4.send(str(sum).encode())
except:
  print("sending Error at client 4")
c4.close()

# cleanup
s1.close()
s2.close()
s3.close()