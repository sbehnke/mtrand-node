mtrand-node
===========

A C++ Mersenne Twister Random Number Generator exposed to Node.js



This is my first attempt at a Node.js add-on written in C++. I'm fairly
certainly there's still a lot broken with it but it a pretty simple exercise for
me to learn from. I took the C++ implementation of the Mersenne Twister
implementation from Jasper Bedaux <http://www.bedaux.net/mtrand/>



Currently the random number is seeded when the module is loaded. It reads random
data from /dev/urandom on OS X and uses CryptGenRandom on Win32. I have not yet
tested it on other platforms, or put in code to explicitly allow for Win64 to
use the same CryptGenRandom API call. I'll test it on my Ubuntu VM and 64-bit
Windows Server 2012 soon.
