#include <stdlib.h>
#include <stdio.h>

long power(long n, long e) {
	long ret = 1;
	while(e) {
		if(e&1)
			ret *= n;
		e = e >> 1;
		n *= n;
	}
	return ret;
}


int main() {
	long e = 25;
	long num = power(5, e);
	long sol = e+1; // the way to represent 1 and 25 0s
	long numZeros = e; // Number of solutions starting with 0
	long chunkSize;
	long old;
	num = num >> 1;
	while(num) {
		chunkSize = 1; //Size of next 1(0^n)
		while(!(num & 1) && num) {
			chunkSize += 1;
			num = num >> 1;
		}
		// Now we have the formula for num of representations and zeros for 1(0^n)
		old = sol;
		sol = sol * chunkSize + numZeros;
		numZeros = numZeros + (chunkSize-1)*old;
		num = num >> 1;
	}
	printf("sol is %ld\n", sol);
}
