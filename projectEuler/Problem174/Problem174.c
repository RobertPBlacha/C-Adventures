#include <stdlib.h>
#include <stdio.h>

int factor(long a) {
	long midpoint;
	long m, n;
	long total = 0;
	for(long i = 2; i*i < a; i++) {
		if(a % i == 0) {
			midpoint = a/i - i;
			if(midpoint % 2)
				continue;
			m = midpoint/2;	
			n = i+m;
			if((n-m)*(n+m) == a && !(n&1^m&1)) {
				total += 1;
			}
		}
		if(total > 10)
			return 0;
	}
	return total > 0;
}

int main() {
	long trueTotal = 0;
        for(long t = 1; t <= 1000000; t++) {
		trueTotal+=factor(t);
        }
	printf("Total is %ld\n", trueTotal);
}
