require 'socket'

@payload="\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"+
         "\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa\xfa"+ 
         "\xfa\xfa\xfa"

#@payload="12345678901234567890123456789012345678901234567890"

module Enumerable

    def sum
      self.inject(0){|accum, i| accum + i }
    end

    def mean
      self.sum/self.length.to_f
    end

    def sample_variance
      m = self.mean
      sum = self.inject(0){|accum, i| accum +(i-m)**2 }
      sum/(self.length - 1).to_f
    end

    def standard_deviation
      return Math.sqrt(self.sample_variance)
    end

end 

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
    s.send(msg, 0, "::1", 16000)
    #s.send(msg, 0, "127.0.0.1", 16000)
end

def measure_stop(s)
    packet = s.recvfrom(1024)
    t = Time.now.to_ms
    seq = packet[0].to_i
    #p seq
    @t2[seq] = t
    sleep(0.018)
end

s = UDPSocket.new(Socket::AF_INET6)
#s = UDPSocket.new(Socket::AF_INET)

for i in 0..1000
    measure_start(s, i)
    measure_stop(s)
end

diff = @t1.zip(@t2).map {|x| x[1]-x[0]}
mean = diff.inject(:+)/diff.size

above = 0
under = 0

variance = diff.standard_deviation/2

for val in diff
    if (val > mean+variance)
        above += 1
    end
    if (val < mean-variance)
        under += 1
    end
end


#puts "min max: #{diff.minmax}"
#puts "down up: #{mean - variance/2} #{mean + variance}"
#puts "above: #{above} under: #{under} total: #{diff.size}"

puts "[\'#{ARGV[0]}\', #{diff.min}, #{mean-(variance)}, #{mean+(variance)}, #{diff.max}, #{above}]"

#@t1.each_with_index { |t1, seq|
#    puts "#{@t2[seq]-t1}"
#}
