#!/bin/sh

function test () {
  for i in $(seq 0 $1)
  do
    ruby client.rb $1 >> /dev/null &
  done
  ruby client.rb $1
}

test 10

