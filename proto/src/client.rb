require 'socket'

msg = IO.binread("rtp_packet")
s = UDPSocket.new(Socket::AF_INET6)
s.send(msg, 0, "::1", 16000)
p s.recvfrom(10)

