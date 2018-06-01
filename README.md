# CS211 Computer Architecture Spring 2017 - Simulate Caches
This C program emulates the function of a cache. The cache size, associativity type, and block size are the parameters given to the program along with a text file that contains instructions for the cache, which consist of read/write operations followed by memory addresses. The purpose of this program is to solely simulate memory read/writes and hit/misses, and does not actually the moving of memory into blocks. 

# Cache Operations
The program simulates two different types of caches Type A and Type B, both of which use 48 bit memory addresses, are write-through caches, and utilize a first in, first out replacement policy. Type A caches use an indexing scheme where the most significant bits are used for the tag bits, followed by index bits and lastly block bits. The Type B cache uses the most significant bits for the index, followed by the bits for the tag and blocks respectively. 
