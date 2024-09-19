#include <stdio.h>
#include "LargeNumber.h"
#include <stdlib.h>
#define MAXARRSIZE 1000
#define CIPHERSIZE 100
#define fermatRepeat 50
typedef struct {
	unsigned long second64; // Little Endian
	unsigned long first64;
} largelong;

unsigned long n, e, d;

void sieve(int* array) {
	int* empty = array;
	int check = 1;
	char prime;
	while(empty-array + 1 < MAXARRSIZE) { //exit loop when first primes found and array is filled
		empty = array; // start loop at first prime
		prime = 1;
		check += 1;
		while(*empty) {
			if(check % *empty == 0) {
				prime = 0;
				break;
			}
			empty += 1;
		}
		if(prime) {//if no divider found
			*empty  = check;
		}
	}
}

unsigned long modPow(unsigned long a, unsigned long no, unsigned long mod) {
	unsigned long res = 1; //error here where overflow happening despite everything having enough(?) space
	a = a % mod;
	if(mod > 0xffffffffl) { printf("mod big\n"); }
	if (a == 0) return 2;
	while(no > 0) {
		if(n & 1) {
			if (res * a < res)
				printf("Overflow in res * a");
			res = (res * a) % mod;
		}
		no = no >> 1;
		if (a*a < a)
			printf("Overflow in a*a");
		a = (a*a) % mod;
	}
	return res;
}

unsigned long fermatGeneratePrime(int *primes) {
	FILE *rand = fopen("/dev/urandom", "r");
	int prime = 0;
	unsigned long a = 0;
	unsigned long guess = 0;
	while(!prime) {
		guess = 0;
		fread(&guess, sizeof(unsigned int), 1, rand);
		// guess is n in a^n-1 = 1 mod n, generate guesses for a
		prime = 1;
		int cont = 0;
		for(int i = 0; i < MAXARRSIZE; i++) {
			if (guess % *(primes+i) == 0) {
				cont = 1;
				prime = 0;
				break;
			}
		}
		if(cont)
			continue;
		for(int i = 0; i < 10; i++) {
			a = 0;
			fread(&a, sizeof(unsigned int), 1, rand);
			a = a % guess; //guarantees a < n
			if(modPow(a, guess-1, guess) != 1) {
				prime = 0;
				break; // Proven composite
			}	
		}	
	}
	return guess;
}

unsigned long generatePrime(int *primes) {
	FILE *rand = fopen("/dev/urandom", "r");
	int prime = 0;
	unsigned long guess;
	while(!prime) {
		fread(&guess, sizeof(unsigned int), 1, rand);
		for(int i = 0; i < MAXARRSIZE; i++)
			if(guess % i == 0)
				continue;	
	}
}

unsigned long eulerExt( unsigned long a,  unsigned long b,  unsigned long *x1, unsigned long *y1) {
	// Always gcd of 1, because e is prime
	// Works backward from gcd of 1 to determine integers x and y such that ax+by=1. b is m, so implementing modulo m on the equation yields ax = 1 mod m
	if(!a) {
		*x1 = 0;
		*y1 = 1;
		return b;
	}
	unsigned long x, y;
	eulerExt(b % a, a, &x, &y);
	*x1 = y - (b / a) * x;
	*y1 = x;
	return 0;
}

unsigned long modInv(unsigned long a, unsigned long m) {
	// There should always be a solution to this because e is forced prime
	// Implemented as extended euclid
	unsigned long x, y;
	eulerExt(a, m, &x, &y);
	unsigned long res = (x % m + m) % m;
	return res;
}

void RSAGenerate(unsigned long q, unsigned long p) {
	// both p and q are going to be < 2^32, therefore their product will be < 2^64
	printf("RSA TIME\n");
	n = p*q;
	e = 65537;
	unsigned long totn = (p-1)*(q-1);
	d = modInv(e, totn);
	printf("p: %lu\nq: %lu\nn: %lu\ne: %lu\nd: %lu\n", p, q, n, e, d);
}

char *RSAEncrypt(char *string) {
	unsigned long *stringReader = (unsigned long *)string;
	unsigned long *stringWriter = calloc(CIPHERSIZE, sizeof(unsigned long));
	for(int i = 0; i < CIPHERSIZE; i++) {
		*(stringWriter+i) = modPow(*(stringReader+i), e, n);
	}
	return (char *)stringWriter;
}

char *RSADecrypt(char *string) {
	unsigned long *stringReader = (unsigned long *)string;
	unsigned long *stringWriter = calloc(CIPHERSIZE, sizeof(unsigned long));
	for(int i = 0; i < CIPHERSIZE; i++) {
		*(stringWriter+i) = modPow(*(stringReader+i), d, n);
	}
	return (char *)stringWriter;
}

int main() {
	int* array = calloc(MAXARRSIZE, sizeof(int));
	sieve(array); //array now has first MAXARRSIZE primes
	char* plaintext = calloc(CIPHERSIZE, sizeof(unsigned long)); //needs size to be divisible by 8 for encryption
	//fgets(plaintext, CIPHERSIZE, stdin);
	unsigned long fermat1 = fermatGeneratePrime(array);
	unsigned long fermat2 = fermatGeneratePrime(array);
	RSAGenerate(fermat1, fermat2);
	//char *encrypted = RSAEncrypt(plaintext);
	//printf("Encrypred: %s\n", encrypted);
	//char *decrypted = RSADecrypt(encrypted);
	//printf("Decrypted: %s\n", decrypted);
	//free(encrypted);
	//free(decrypted);
	free(array);
}
