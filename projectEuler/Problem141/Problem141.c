#include <stdlib.h>
#include <stdio.h>

unsigned long cubes[1001];

void init() {
	for(int i = 1; i < 1000;
}


unsigned long testEfficient(unsigned long n) {
	unsigned long r, n2, q;
	n2 = n*n;
	for(unsigned long d = 2; d < n; d++) {
		r = (n2%d);
		if(r == 0)
			continue;
		if(n2*r == d*d*d+r*r) {
			printf("%ld^2: q=%ld, d=%ld, r=%ld\n", n, n*n/d, d, r);
			return n2;
		}
	}
	return 0;
}


unsigned long newTestEfficient(unsigned long n) {
	unsigned long r, n2, q;
	n2 = n*n;
	for(unsigned long d = 2; d < n; d++) {
		r = (n2%d);
		if(r == 0) // Possible Candidate for b
			continue;
		if(n2*r == d*d*d+r*r) {
			printf("%ld^2: q=%ld, d=%ld, r=%ld\n", n, n*n/d, d, r);
			return n2;
		}
	}
	return 0;
}


int main() {
	unsigned long sum = 0;
	for(int i = 3; i < 1000000; i++) {
		sum += testEfficient(i);
	}
	printf("sum is %lu\n", sum);
}
