require 'packetfu'
include PacketFu

MSG_MAX_SIZE = 1024



class UdpSocketThread
    def initialize(iface, id, address, port)
        @id = id
        @interface = iface 
        @socket = UDPSocket.new
        @socket.bind(address, port)
        puts "#{id} - #{address}:#{port}"
        @close = false

        @thread = Thread.new do
            while !close
                msg, info = @socket.recvfrom(1024)                
                @interface.msg_received(id, msg)
            end
            @socket.close
            puts "#{id} closed"
        end
    end

    def close()
    end

    def close_socket()
        puts "called close_socket for #{@id}"
        @socket.close
    end

    def send(msg, remote_address, remote_port)
        @socket.send(msg, 0, remote_address, remote_port)
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
        puts "#{socket_id} received \"#{msg}\""
    end

    def send_msg(socket_id, msg, remote_address, remote_port)
       @sockets[socket_id].send(msg, remote_address, remote_port) 
    end

    def close(stream=nil)
        puts "called close for Interface#{stream}"
        if stream == nil
            @sockets.each {|s| s.close_socket if s!=nil}
            @main.close_socket
        else
            @sockets[stream].close_socket
        end
    end
end

