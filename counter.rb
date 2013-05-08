f = File.new(ARGV[0], "r")

distr = []

f.each_line{ |l|
    delay = l.to_f
    
