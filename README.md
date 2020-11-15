echo-client-server
=====

echo-client:  
syntax : echo-client \<ip\> \<port\>  
sample : echo-client 192.168.10.2 1234 
  
echo-server:  
syntax : echo-server \<port\> \[-e\[-b\]\]  
sample : echo-server 1234 -e -b

## example
#### Server   
$ echo-server 1234 -e -b    
Waiting for Connection Request  
Connected with \[User0\] at 192.168.0.5  
Waiting for Connection Request  
Connected with \[User1\] at 192.168.0.7  
Waiting for Connection Request  
Received 0  
Connection with \[User0\] closed  
Received 0  
Connection with \[User1\] closed  

#### Client1
$ echo-client 192.168.0.3 1234  
Connection success  
Press Enter to Exit  
hi  
\[Echo\] hi  
\[Broadcast: User0\] hi  
\[Broadcast: User1] hello  
\[Echo\] who r u?  
\[Broadcast: User0\] who r u?  
\[Broadcast: User1\] none of your business   
\[Broadcast: User1\] haha    

#### Client2
$ echo-client 192.168.0.3 1234  
Connection success  
Press Enter to Exit  
\[Broadcast: User0\] hi  
hello  
\[Echo\] hello  
\[Broadcast: User1\] hello     
\[Broadcast: User0\] who r u?  
none of your business   
\[Echo\] none of your business   
\[Broadcast: User1\] none of your business   
\[Echo\] haha  
\[Broadcast: User1\] haha   
