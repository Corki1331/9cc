#! /bin/bash

try() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
      echo "$input => $actual"
    else
      echo "$input => $expected expected, but got $actual"
      exit 1
    fi
}
try 32 'return 32;'
try 112 'a = 1*3; b=a+1;return 112;c=9/9+1;'
try 0 '0;'
try 42 '42;'
try 21 "5+20-4;"
try 41 " 12 + 34 - 5;"
try 47 '5+6*7;'
try 15 '5*(9-6);'
try 4 '(3+5)/2;'
try 10 '-10+20;'
try 10 '- -10;'
try 10 '- - +10;'
try 0 '0==1;'
try 1 '42==42;'
try 1 '0!=1;'
try 0 '42!=42;'

try 1 '0<1;'
try 0 '1<1;'
try 0 '2<1;'
try 1 '0<=1;'
try 1 '1<=1;'
try 0 '2<=1;'

try 1 '1>0;'
try 0 '1>1;'
try 0 '1>2;'
try 1 '1>=0;'
try 1 '1>=1;'
try 0 '1>=2;'
try 7 'a = 5+2;'
try 10 'a = (3+2)*4/2;'
try 12 'a=5+2;b=2+3;c=1+4;a+b;'
try 60 'a=5+2;b=2+3;c=1+4;dog = a+b;1;22;33;e = 1+1;dog*b;'
try 60 'a=5+2;b=2+3;if (a==7){dog=a+b;}if(a==7){e=1+1;}dog*b;'
try 220 '
     a = 10;
     b = 15;
     if(a < b){
       b = b + 7;
     } 
     c = 6;
     return  b * a;'
try 10 '
     a = 10;
     b = 15;
     if(a > b){
       b = b + 7;
     } else {
       b = 1;
     }
     c = 6;
     return  b * a;'
try 45 'i = 1;while(i < 45){i = i+1;}return i;'
try 0 'i = 10;while(i){i = i-1;}return i;'

echo OK