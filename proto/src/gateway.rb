require 'io/console'
load 'rtp_interface.rb'

iface = Interface.new(:sip, "127.0.0.1", 5060)

begin
    str = gets.chomp 
    x, y = str.split(' ')
end until !x

iface.close_interface

