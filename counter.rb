C_BIG = 5
C_SMALL = 1

f = File.new(ARGV[0], "r")

distr = Hash.new(0)

f.each_line{ |l|
    delay = (l.to_f * 2).round / 2.0 
    distr[delay] += 1
}   

(0.0..60.0).step(0.5){ |x|
    puts "#{x}\t#{distr[x]}"
}
#distr.sort.each { |key, val|
#    puts "#{key}\t#{val}"
#}
