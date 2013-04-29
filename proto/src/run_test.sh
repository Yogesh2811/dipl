#!/bin/sh

function test () {
  for i in $(seq 0 $1)
  do
    ruby client.rb $1 &
  done
  ruby client.rb $1 >> results.txt
}

test 1
#test 60
#test 70
#test 80
#test 90
#test 100
#test 110
#test 120
#test 130
#test 140
#test 150
