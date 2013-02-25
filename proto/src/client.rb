require 'socket'

@payload="\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+
         "\xfa\xfa\xfa"

@t1 = Array.new
@t2 = Array.new

class Time
    def to_ms
        1000.0 * self.to_f
    end
end

def make_packet(seq, timestamp, ssrc)
    seq1 = seq >> 8
    seq2 = seq & 0xff
    pck = String.new
    pck << "\x80" #v, p, e, csrc count
    pck << "\x09" #marker, codec
    pck << seq1 #seq 1.byte
    pck << seq2 #seq 2.byte
    pck << timestamp
    pck << ssrc
    pck << @payload
    return pck
end

def measure_start(s, seq)
    msg = make_packet(seq, "\xff\xff\xff\xff",
                      "\x45\x92\x29\x67")
    t = Time.now.to_ms
    @t1[seq] = t
    s.send(msg, 0, "127.0.0.1", 16000)
end

def measure_stop(s)
    packet = s.recvfrom(1024)
    t = Time.now.to_ms
    seq = packet[0].to_i
    @t2[seq] = t
end

#s = UDPSocket.new(Socket::AF_INET6)
#s.send(msg, 0, "::1", 16000)

s = UDPSocket.new(Socket::AF_INET)

for i in 0..10
    measure_start(s, i)
    measure_stop(s)
end

@t1.each_with_index { |t1, seq|
    puts "#{@t2[seq]-t1}"
}
