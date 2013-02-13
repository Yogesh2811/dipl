require 'socket'


s = UDPSocket.new(Socket::AF_INET6)
s.send("ahoj", 0, "::1", 16000)
p s.recvfrom(10)

