# Problem 146	
## Prompt
The smallest positive integer for which the numbers $n^2+1$, $n^2+3$, $n^2+7$, $n^2+9$, $n^2+13$, and $n^2+27$ are consecutive primes is $10$. The sum of all such integers below one-million is $1242490$
What is the sum of all such integers $n$ below $150$ million?
## Methodology
### Naive Approach
Brute force checking every $n$ would simply take too long, so we will need to find some way to filter down the actual numbers we are checking against
### N must be a multiple of 10
All of our guesses must be a multiple of 10 for the following reasons.
#### N must be a multiple of 2
For $n = 2k+1$, $n^2+1 = 4k^2+4k+1+1 = 4k^2+4k+2$ which is divisible by $2$
#### N must be a multiple of 5
For all $n\neq 5k$ $n^2 \% 5\equiv 1$ or $n^2 \% 5 \equiv 4$ 
We need $n^2+1$ and $n^2+9$ to be prime, so $n^2+1 \% 5 = 0$ or $n^2 + 4 \% 5 = 0$
For all $n\neq 5k$ one of $n^2+1$ or $n^2+4$ will be not prime, so $n=5k$
### N cannot be divisible by 3,7, or 13
We know that 
$$n = ax, n,a,x\in \mathbf{Z}\\
n^2 = a^2x^2\\
\therefore a | n^2+a$$
So because we check $n^2+3$,$n^2+7$, and $n^2+13$, we know n cannot be divisible by $3$,$7$, or $13$
### Filtering bad guesses
We now have $7912087$ leftover valid guesses for $n < 150000000$ which is still too much for checking for primes. We can modify the last property we used for filtering to further filter candidates. If we generate a list of primes (which takes $O(nlog(log(n)))$ time) we can begin checking for each prime $p$ and each addend $a \in \{1,3,7,9,13,27\}$  if $((n\%p)^2 +a) \% p\equiv 0$
This will let us further filter our answers. The actual number of answers that we filter out will vary based on the number of primes $p$ we generate. For $100$ primes, we have further filtered our guesses to a much more reasonable $7497$. From here we can begin actually checking our guesses in-depth.
### Checking if a guess is valid
We will need to check if our remaining guesses hold the prime property. This can be done simply by taking the last prime $p$ we generated, and checking it against $n^2+a$ for $a\in\{1,3,7,9,13,27\}$. We will then increment $p$ by two until $p^2 > n^2+27$
### One Last Edge Case
We have failed to verify one important condition in the problem. The primes must be **consecutive**
Thankfully, this does not mean we will have to check all numbers between $n^2+1$ and $n^2+27$ for primality but it does mean that we have 8 more numbers to check, except these have to be composite: $n^2+c$ for $c \in \{5,11,15,17,19,21,23,25\}$ notice that we do not have to check even added values to $n^2$ because $n^2$ is already even, so any even addend will always result in a composite number
## Code
Finally we have our methodology so we can begin coding. Below is my code in C for the problem 

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
## Answer
Running `time ./Problem146` yields
> 12 such numbers with sum = 676333270
>
> real	0m16.137s
user	0m16.129s
sys	0m0.005s
 

> Written with [StackEdit](https://stackedit.io/).

