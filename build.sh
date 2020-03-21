#!/bin/sh

CC_FLAGS="-Wall -Wextra -pedantic -I include -O0"

build_cc () {
  if ! [ -e $2 ] || [ $(date +%s -r $1) -gt $(date +%s -r $2) ]; then
    echo gcc $CC_FLAGS -c $1 -o $2
    gcc $CC_FLAGS -c $1 -o $2
  fi
}

build_cc json.c  json.o
build_cc jn.c  jn.o

gcc *.o -o jn
