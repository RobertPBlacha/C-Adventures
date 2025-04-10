#include <stdlib.h>
#include <stdio.h>
#define FILTERSIZE 100
//FILTER SIZE Is a hyperparameter that should be tweaked if run for different maxes
int *firstPrimes;

int check(int n) {
	int arr[6] = {1,3,7,9,13,27};
	int prime;
	for(int i = 0; i< 6; i++) {
		for(int j = 0; j < FILTERSIZE; j++) {
			prime = *(firstPrimes+j);
			if(prime > n)
				break;
			if((((n%prime)*(n%prime)) + arr[i]) % prime == 0) {
				return 0;
			}
		}
	}
	return 1;
}

void genPrimes() {
	firstPrimes = calloc(FILTERSIZE, sizeof(int));
	int index = 1;
	*firstPrimes = 2;
	int guess = 1;
	int cont;
	while(index < FILTERSIZE) {
		cont = 0;
		guess += 2;
		for(int i = 0; i < index; i++)
			if(guess % *(firstPrimes+i) == 0) {
				cont = 1;
				continue;
			}
		if(cont)
			continue;
		*(firstPrimes+index) = guess;
		index += 1;
	}
}

int isPrime(long n) {
	if(n == 100)
		return 1; //Edge case where its too small for below to work, this will only happen for 10 so we can just skip it without changing anything
	long arr[6] = {n+1,n+3,n+7,n+9,n+13,n+27};
	long notArr[8] = {n+5,n+11,n+15,n+17,n+19,n+21,n+23,n+25}; //Don't need to check n^2+even because thats trivially not prime
	int badPrime[8] = {1,1,1,1,1,1,1,1};
	for(long i = *(firstPrimes+FILTERSIZE-1); i*i < n+27; i+=2) {
		for(int j = 0; j < 6; j++) {
			if(arr[j] % i == 0)
				return 0;
		}
		for(int j = 0; j < 8; j++) {
			if(badPrime[j] && notArr[j] % i == 0)
				badPrime[j] = 0;
		}
	}	
	int cont;
	for(int i = 0; i < 8; i++) { //need this for "CONSECUTIVE PRIMES" (literally one edge case)
		cont = 0;
		if(badPrime[i]) {
			for(int j = 0; j < FILTERSIZE; j++) {
				if(notArr[i] % *(firstPrimes+j)== 0) {
					cont = 1;
					break;
				}
			}
			if(cont)
				continue;
			return 0;
		}
	}
	return 1;
}

int main() {
	long total = 0;
	long sum = 0;
	genPrimes();
	for(long i = 10; i < 150000000; i+=10) { // increment by 10 because must be divisble by 2, and must be divisible by 5 because all numbers not divisible by 5 will be filtered by check because for all n, n^2 is either 1 or 4 which means n+1 or n+9 will not be prime
		if(i%3==0 || i%7==0 || i%13==0) // These are obvious because a | ax + a
			continue;
		if(check(i) && isPrime(i*i)) {//check runs first and continues, saving time
			total+= 1;
			sum += i;
		}
	}
	printf("%ld such numbers with sum = %ld\n", total, sum); 
	free(firstPrimes);
}
