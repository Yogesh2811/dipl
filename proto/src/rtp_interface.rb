require "observer"
require "socket"
load "AES.rb"


MSG_MAX_SIZE = 1024

        


class UdpSocketThread
    include Observable

    def initialize(iface, id, address, port)
        @id = id
        @socket = UDPSocket.new
        @socket.bind(address, port)
        @address = address
        @port = port
        puts "#{id} - #{address}:#{port}"
        @close = false

        add_observer(iface, func=:msg_received)

        run()
    end

    def run()
        @thread = Thread.new {
            while !@close
                msg, info = @socket.recvfrom(1024)                
                msg_received(msg)
            end
            @socket.close
            puts "#{id} closed"
        }
    end

    def close_socket()
        puts "called close_socket for #{@id}"
        @socket.close
    end

    def send(msg, remote_address, remote_port)
        @socket.send(msg, 0, remote_address, remote_port)
    end

    def set_key(key)
        @key = key
    end

    def parse_packet(packet)
        seq = packet[96..99]
        ssrc = packet[108..115]
        payload = packet[116..-9]
        return seq, ssrc, payload
    end

    def msg_received(msg)
        #seq, ssrc, payload = parse_packet(msg)
        changed
        notify_observers(@id, msg)
    end
end


class Interface
    def initialize(type, address, port)
        @sockets = []
        @main = UdpSocketThread.new(self, type, address, port)
    end

    def new_connection(socket_id, local_address, local_port)
        @sockets[socket_id] = 
            UdpSocketThread.new(self, local_port, local_address, local_port)
    end

    def msg_received(socket_id, msg)
        puts "#{socket_id} \"#{msg}\""
    end

    def send_msg(socket_id, msg, remote_address, remote_port)
       @sockets[socket_id].send(msg, remote_address, remote_port) 
    end

    def close_interface(stream=nil)
        puts "called close for Interface#{stream}"
        if stream == nil
            @sockets.each {|s| s.close_socket if s!=nil}
            @main.close_socket
        else
            @sockets[stream].close_socket
        end
    end
end









#class CryptographicContexts
#    attr_accessor :roc          # rollover counter
#    attr_accessor :master_keys  # array of master keys
#    attr_accessor :master_salts # array of master salts
#    attr_accessor :n_e          # length of session key
#    attr_accessor :counter_ep   # encrypted packets counter
#    attr_accessor :key_rate     # session key derivation rate
#
#    def initialize(ssrc, dest_add, dest_port)
#        @roc = 0
#        @ssrc = ssrc
#        @dest_add = dest_add
#        @dest_port = dest_port
#        @master_keys = []
#        @master_salts = []
#        @n_e = 0
#        
#        @id = get_id()
#    end
#
#    def get_id()
#        return (@ssrc.to_s + @dest_add.to_s + @dest_port.to_s).to_sym unless @id
#        @id
#    end
#end
