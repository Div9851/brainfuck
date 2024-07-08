rm -f a.out
./bf $1 > tmp.s
gcc tmp.s
./a.out
