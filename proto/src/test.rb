require 'io/console'
load 'rtp_interface.rb'

iface = [] 
iface << Interface.new(:main, "127.0.0.1", 0)

begin
    str = gets.chomp
    x, p1, msg, p2 = str.split(' ')
    local_port = p1.to_i
    remote_port = p2.to_i
    if x == 'b'
        iface[0].new_connection(local_port, "127.0.0.1", local_port)
    elsif x == 's'
        iface[0].send_msg(local_port, msg, "127.0.0.1", remote_port)
    elsif x == 'sip'
        iface << Interface.new(:sip, "127.0.0.1", 5060)
    end
end until !x

iface.each {|i| i.close}
