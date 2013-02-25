require 'socket'

def parse_packet(packet)
    seq1,seq2 = packet[2].ord, packet[3].ord
    seq = (seq1<<8) | (seq2&0xff)  
    return seq.to_s
end

#s = UDPSocket.new(Socket::AF_INET6)
#s.bind("::1",16000)
s = UDPSocket.new(Socket::AF_INET)
s.bind("127.0.0.1",16000)

while true do
    packet = s.recvfrom(1024)
    remote_address = packet[1][2].to_s
    remote_port = packet[1][1].to_i
    response = parse_packet(packet[0])
    #sleep(0.001)
    puts response
    s.send(response, 0, remote_address, remote_port)
end

