#include <stdlib.h>
#include <stdio.h>

long factorial(long n) {
	if(n == 1)
		return 1;
	return n*factorial(n-1);
}


long power(long n, long e) {
	long a = 1;
	while(e) {
		if(e&1)
			a *= n;
		n *= n;
		e >>= 1;
	}
	return a;
}

long addDig(long size, long ind, long contains) {
	long sum = 0;
	//printf("HERE\n%d, %d, %d\n", size, ind, contains);
	if(size-ind < 3-contains) 
		return 0;
	if(size-ind == 3-contains)
		return factorial(3-contains);
	if(contains == 3) { 
		return power(16, size-ind);
	}
	sum += (3-contains) * addDig(size, ind+1, contains+1);
	sum += (16-3+contains) * addDig(size, ind+1, contains);
	return sum;
}

long count(long size) {
	long sum = 0;
	sum += 13*addDig(size, 1, 0);
	sum += 2*addDig(size, 1, 1);
	return sum;
}

int main() {
	long sum = 0;
	for(long i = 3; i < 17; i++) {
		sum += count(i);
	}
	printf("%lX\n", sum);
}
