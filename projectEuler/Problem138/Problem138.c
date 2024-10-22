#include <stdlib.h>
#include <stdio.h>

long double NewtonSquare(long double N, long double guess) {
	long double ret = (guess + N/guess)/2.;
	if(ret == guess) {
		return ret;
	}
	return NewtonSquare(N, ret);
}

int main() {
	long double height = 3;
	long total = 0;
	long double sum = 0;
	long double square = 0;
	long double b1, b2, val;
	while(total < 12) {
		b1 = (height+1)/2;
		b1 = b1*b1;
		b2 = (height-1)/2;
		b2 = b2*b2;
		square = height*height;
		val = NewtonSquare(square + b1, height);
		if(val - (long)val == 0) {
			printf("Adding %Lf for height %Lf\n", val, height);
			sum += val;
			total += 1;
		}
		val = NewtonSquare(square + b2, height);
		if(val - (long)val == 0) {
			printf("Adding %Lf for height %Lf\n", val, height);
			sum += val;
			total += 1;
		}
		height += 2;
	}
	printf("%Lf\n", sum);
}
