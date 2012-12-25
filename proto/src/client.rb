require 'io/console'
load 'rtp_interface.rb'

iface = [] 
iface << Interface.new(:main, "127.0.0.1", 0)
msg = "message"

remote_address = "127.0.0.1"
remote_port = 5060

local_port = 6666

begin
    str = gets.chomp
    p0, p1, p2, p3, p4 = str.split(' ')
    x = p0

    if x == 'b'
        local_port = p1.to_i if p1
        iface[0].new_connection(local_port, remote_address, local_port)
    elsif x == 's'
        msg = p1 if p1
        remote_address = p2 if p2
        remote_port = p3.to_i if p3
        local_port = p4.to_i if p4
        iface[0].send_msg(local_port, msg, remote_address, remote_port)
    elsif x == 'sp'
        Thread.new{
            (0..500).each {|x|
                iface[0].send_msg(local_port, msg, remote_address, remote_port)
                sleep 0.02
            }
        }
    end
end until !x

iface.each {|i| i.close_interface}
