#include <stdio.h>
#include "LargeNumber.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#define MAXARRSIZE 1000
#define THREADCOUNT 8
#define PRIMESIZE 16
#define CIPHERSIZE 100
#define fermatRepeat 10

unsigned long n, e, d;
sem_t sem_primes;
int neededPrimes;
largenumber * n2, *e2, *d2;
void sieve(largenumber **array) {
	largenumber** empty = array;
	largenumber *check = initvLargeNumber(1, 1); // = 1
	char prime;
	largenumber *zero = initLargeNumber();
	largenumber *scratch;
	int count = 0;
	while((empty-array)+1 < MAXARRSIZE) { //exit loop when first primes found and array is filled
		empty = array; // start loop at first prime
		prime = 1;
		addLargeNumber(check, 1l);
		while(*empty) {
			scratch = modTwoLargeNumbers(check, *empty);
			if(equalsLarge(scratch, zero)) {
				prime = 0;
				freeLarge(scratch);
				break;
			}
			empty += 1;
			freeLarge(scratch);
		}
		if(prime) {//if no divider found
			*empty = initLargeNumber();
			setLargeEqual(*empty, check);
		}
		count += 1;
	}
	freeLarge(check);
	freeLarge(zero);
}

unsigned long modPow(unsigned long a, unsigned long no, unsigned long mod) {
	unsigned long res = 1; //error here where overflow happening despite everything having enough(?) space
	a = a % mod;
	if (a == 0) return 2;
	while(no > 0) {
		if(n & 1) {
			if (res * a < res)
				printf("Overflow in res * a");
			res = (res * a) % mod;
		}
		no = no >> 1;
		a = (a*a) % mod;
	}
	return res;
}

void *fermatGeneratePrime(void *primesPTR) {
	largenumber **primes = (largenumber **)primesPTR;
	FILE *rand = fopen("/dev/urandom", "r");
	int prime = 0;
	largenumber *zero = initLargeNumber();
	largenumber *a;
	unsigned int *aMem = calloc(PRIMESIZE, sizeof(unsigned int));
	largenumber *scratch, *scratch2 = initLargeNumber();
	unsigned int *guessMem = calloc(PRIMESIZE+1, sizeof(unsigned int));
	largenumber *guess;
	while(!prime) {
		guessMem[PRIMESIZE] = 0; //Chance of needing to set less than this to zero is negligible
		fread(guessMem, sizeof(unsigned int), PRIMESIZE, rand);
		/*if(!(guessMem[PRIMESIZE-1] & 0x80000000 && guessMem[PRIMESIZE-1] & 0x40000000)) {
			continue;
		}*/
		guessMem[PRIMESIZE-1] = guessMem[PRIMESIZE-1] | 0xc0000000; 
		/* I do this as a way to ensure that the MSB of n is always bigger than typable characters. 
		 * I understand why this kinda violates the integrity of the random number generator and that 
		 * I'm doing the block cipher part of this wrong but this is more of a proof of concept than anything else*/
		guess = initMemLargeNumber(guessMem);
		prime = 1;
		int cont = 0;
		for(int i = 0; i < MAXARRSIZE; i++) {
			scratch = modTwoLargeNumbers(guess, *(primes+i));
			if (equalsLarge(scratch, zero)) {
				cont = 1;
				prime = 0;
				freeLarge(scratch);
				break;
			}
			freeLarge(scratch);
		}
		sem_wait(&sem_primes);
		if(neededPrimes == 0) {
			freeLarge(guess);
			guess = 0; //Stop running, two primes found
			sem_post(&sem_primes);
			break;
		}
		sem_post(&sem_primes);
		if(cont) {
			freeLarge(guess);
			continue;
		}
		// guess is n in a^n-1 = 1 mod n, generate guesses for a
		for(int i = 0; i < fermatRepeat; i++) { //probability of < 2^-10 that this is not prime a^(n-1) = 1 mod n
			aMem[PRIMESIZE-1] = 0;
			fread(aMem, sizeof(unsigned int), PRIMESIZE-1, rand);
			scratch = initMemLargeNumber(aMem);
			setLargeEqual(scratch2, guess);
			subLargeNumber(scratch2, 1);
			a = modTwoLargeNumbers(scratch, guess); //guarantees a < n
			freeLarge(scratch);
			scratch = largeModPow(a, scratch2, guess);
			subLargeNumber(scratch, 1);
			if(!equalsLarge(scratch, zero)) {
				prime = 0;
				freeLarge(scratch);
				freeLarge(a);
				break; // Proven composite
			}	
			freeLarge(scratch);
			freeLarge(a);
		}	
		if(!prime)
			freeLarge(guess);
		sem_wait(&sem_primes);
		if(neededPrimes == 0) { //Other threads are done
			guess = 0;
			prime = 1;
		}
		else if(prime) { //One thread finished
			neededPrimes -= 1; //one thread is done
		}	
		sem_post(&sem_primes);
	}
	free(aMem);
	free(guessMem);
	freeLarge(scratch2);
	freeLarge(zero);
	fclose(rand);
	if(guess) {
		return (void *)guess;
	}
	else
		return 0;
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

unsigned long modInvPrimitive(unsigned long a, unsigned long m) {
	// There should always be a solution to this because e is forced prime
	// Implemented as extended euclid
	unsigned long x, y;
	eulerExt(a, m, &x, &y);
	unsigned long res = (x % m + m) % m;
	return res;
}

void RSAGenerate(largenumber *q, largenumber *p) {
	// both p and q are going to be < 2^32, therefore their product will be < 2^64
	n2= multiplyTwoLargeNumbers(p, q);
	subLargeNumber(p, 1);
	subLargeNumber(q, 1);
	largenumber *totn = multiplyTwoLargeNumbers(p, q);
	e2 = initvLargeNumber(1, 65537);
	d2 = modInv(e2, totn);
	addLargeNumber(p,1);
	addLargeNumber(q,1);
	printf("p:\n");
	displayLargeNum(p);
	printf("q:\n");
	displayLargeNum(q);
	printf("e\n");
	displayLargeNum(e2);
	printf("d\n");
	displayLargeNum(d2);
	printf("n\n");
	displayLargeNum(n2);
}

char *RSACrypt(char *string, int blocks, int blockSize, largenumber *key) {
	// Change to largenumber *
	char *res = calloc(blocks+1, blockSize); //+1 handles null termination
	int i = 0;
	while(i < blocks) {
		largenumber *l = initSizedMemLargeNumber((unsigned int *)(string+(i*blockSize)), blockSize/sizeof(unsigned int));
		largenumber *write = largeModPow(l, key, n2);
		char *stringWriter = charRep(write, blockSize);
		memcpy(res+i*blockSize, stringWriter, blockSize);
		i += 1;
		freeLarge(write);
		free(l);
	}
	
	return res;
}

int main() {
	largenumber **array = calloc(MAXARRSIZE, sizeof(largenumber *));
	sieve(array); //array now has first MAXARRSIZE primes
	pthread_t *threadArray = malloc(sizeof(pthread_t)*THREADCOUNT);
	largenumber *fermat1;
	largenumber *fermat2;
	void *res = 0;
	neededPrimes = 2;
	sem_init(&sem_primes, 0, 1);
	if(THREADCOUNT > 1) {
		for(int i = 0; i < THREADCOUNT; i++) {
			pthread_create(threadArray+i, NULL, fermatGeneratePrime, array);
		}
		for(int i = 0; i < THREADCOUNT; i++) {
			pthread_join(threadArray[i], &res);
			if(res && fermat1) {
				fermat2 = res;
			}
			else if(res) {
				fermat1 = res;
			}
		}
		free(threadArray);
	}
	else {
		fermat1 = (largenumber *)fermatGeneratePrime(array);
		fermat2 = (largenumber *)fermatGeneratePrime(array);
	}
	sem_destroy(&sem_primes);
	RSAGenerate(fermat1, fermat2);
	unsigned long blocks = 1;
	unsigned long fullBlock = sizeof(unsigned int)*PRIMESIZE*2; // Need to solve problem of block plaintext > N
	char *plaintext = calloc(sizeof(unsigned int), fullBlock + 1); //need +1 because of fgets null terminate
	plaintext = fgets(plaintext, fullBlock+1, stdin);
	unsigned int c, len;
	while((c = getc(stdin)) && c != '\n' && c != EOF && c != 0x1) {
		plaintext = realloc(plaintext, (blocks+1)*fullBlock+1);
		fgets(plaintext+blocks*fullBlock+1, fullBlock, stdin);
		*(plaintext+blocks*fullBlock) = c;
		len = strlen(plaintext+blocks*fullBlock);
		for(int i = len; i < fullBlock; i++) {
			*(plaintext+blocks*fullBlock+i) = 0;
		}
		blocks+=1;
	}
	char *encrypted = RSACrypt(plaintext, blocks, fullBlock, e2);
	printf("Encrypted: %s\n", encrypted);
	// Maybe one day I'll add multi threading here
	char *decrypted = RSACrypt(encrypted, blocks, fullBlock, d2);
	printf("Decrypted: %s\n", decrypted);
	free(encrypted);
	free(decrypted);
	freeLarge(fermat1);
	freeLarge(fermat2);
	for(int i = 0; i < MAXARRSIZE; i++)
		freeLarge(*(array+i));
	free(array);
	freeLarge(e2);
	freeLarge(d2);
	freeLarge(n2);
}
