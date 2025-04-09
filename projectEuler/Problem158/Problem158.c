#include <stdlib.h>
#include <stdio.h>

char st[26];

// Once we generate an ordered pair, the remaining can be permutations of a reverse sorted order list of the remaining elements

unsigned long choose(long n, long k) {
	if(!k)
		return 0;
	if(n==k)
		return 1;
	if(n < k) 
		return 0;
	unsigned long res = n;
	if(k << 1 > n) // equivalent and reduces number of mults/divs
		k = n - k;
	for(int i = 2; i <= k; i++) { // n!/(n-k)! will cancel out elements up to (n-k), the k remaining parts are then divided by k!
		res *= (n-i+1);
		res /= i;
	}
	return res;
}
void print(int size) {
	for(int i = 0; i < size; i++)
		printf("%c", 'A'+st[i]);
	printf(" ");
}

long recurse(int offset, int size) {
	long ret = 0;
	if(offset == size)
		return 0;
	if(offset > 0) {
		for(int i = st[offset-1] - 1; i >= 0; i--) {
			st[offset] = i;
			ret += choose(26-1-offset, size-offset-1); // total is 26-1, there are total offset numbers removed from the list
			ret += recurse(offset + 1, size);
		}
	}
	else {
		for(int i = 0; i < 26; i++) {
			st[offset] = i;
			ret += choose(26-1-i, size-offset-1);
			ret += recurse(offset+1, size);	
		}
	}
	return ret;
}

int main() {
	long max = 0;
	long calc = 0;
	for(int i = 2; i <= 26; i++) {
		calc = recurse(0, i);
		if(calc > max)
			max = calc;
	}
	printf("%ld\n", max);
}
