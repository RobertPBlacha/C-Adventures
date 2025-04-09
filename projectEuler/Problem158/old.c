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
	int check = 0;
	for(int i = 0; i < 26; i++) {
		if(offset == 0) {
			st[offset] = i;
			check = choose(26-1-i-offset, size-offset-1);
			check += recurse(offset+1, size);
			printf("%c: %d (%ld)\n", 'A'+i, check, recurse(offset+1, size));
			ret += choose(26-1-i-offset, size-offset-1);
			ret += recurse(offset+1, size);	
		}
		else {
			if(i < st[offset-1]) { //comes before, the other case was handled already
				st[offset] = i;
				ret += choose(26-1-i-offset, size-offset-1); //There will be offset number of invalid options that are included before
				if(26-1-i-offset > size-offset-1 && size > offset+1) //if more interesting picks left than number of things we have to pick
					ret += recurse(offset+1, size);
			}
		}
	}
	return ret;
}

int main() {
	long max = 0;
	long calc = 0;
	for(int i = 2; i <= 3; i++) {
		calc = recurse(0, i);
		if(calc > max)
			max = calc;
		printf("FINISHED %d: %ld\n", i, calc);
	}
	printf("%ld\n", max);
}
