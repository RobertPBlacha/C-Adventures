# Problem 169
## Prompt 
Define $f(0) = 1$ and $f(n)$ to be the number of different ways $n$ can be expressed as a sum of integer powers of
$2$ using each power no more than twice.

For example, $f(10) = 5$ since there are five different ways to express $10$:

$1+1+8 = 10$

$1+1+4+4 = 10$

$1+1+2+2+4$

$2+4+4=10$

$2+8=10$

What is $f(10^{25})$?
## Methodology  
### $10^{25}$ is too big
From the start we can see that $10^{25}$ is too big to be effectively stored in a long. Our solution will need to be able to calculate this sum without storing the full number, we will keep this in mind for later.
### Rewriting the sums
We can start rewriting the example sums given to us in 'binary', and see a pattern:

$1+1+8 = 1002_2$

$1+1+4+4 = 0202_2$

$1+1+2+2+4 = 0122_2$

$2+4+4 = 0210_2$

$2+8 = 1010_2$

We can see a general pattern where when a one is preceded by a string of zeros it can be decomposed as follows:
$$100\dots_2 \rightarrow020\dots_2\rightarrow012\dots_2$$
The first time a power of two gets split, it leaves a zero behind with two of the smaller powers, future splits leave a one behind with two powers of the smaller twos. 

We now have an equation for the pattern $1(0^n)_2$: $$n+1$$
Where the one is the regular binary representation of the number and the $n$ represents the one being decomposed into the number of zeros trailing the one.
### Combing Patterns
We can look at the given sums again to see what happens when two patterns of $1(0^n)_2$ are combined

We can see what we can trivially expect, which is multiplying $f(2)*f(2)$: 

$1002_2$

$1010_2$

$0202_2$

$0210_2$

But there is an interesting case here where there exists the sum $0122_2$, where the expression $012_2$ and $002_2$ are combined. We can think of this as the latter term 'lending' it leading zero to the front term. Because only one solution to the number of representations of $1(0^n)_2$ will end with a $2$ in the least significant digit, this two can be decomposed into the leading zero of the second pattern for every single version of the second pattern that starts with a zero. This is where $0122_2$ comes from.

This means that there will be one extra combination for every representation of the second pattern that starts with zero. This is equal to one less than the size of the chunk.

This means that for two chunks with $Z(n)$ being a function that returns the number of solutions with a leading zero, we can calculate $$f(n_1||n_2) = f(n_2)*f(n_1)+Z(n_2)$$
And with $s$ being the number of bits of $n_1$ we can calculate $$Z(n_1||n_2) = Z(n_2)+(s-1)*f(n_2)$$
### Splitting $10^{25}$
We can finally deal with the problem of $10^{25}$ being too big to fit in a long. We can make the deduction that $10^{25} = 2^{25}*5^{25}$, which means we can simulate the return of the first chunk with $f(2^{25}) = 26$ and $Z(2^{25})=25$

Now $5^{25} < 2^{64}$ so we can run our above calculations to find the answer 
## Code
	#include <stdlib.h>
	#include <stdio.h>

	long power(long n, long e) {
	        long ret = 1;
	        while(e) {
	                if(e&1)
	                        ret *= n;
	                e = e >> 1;
	                n *= n;
	        }
	        return ret;
	}


	int main() {
	        long e = 25;
	        long num = power(5, e);
	        long sol = e+1; // the way to represent 1 and 25 0s
	        long numZeros = e; // Number of solutions starting with 0
	        long chunkSize;
	        long old;
	        num = num >> 1;
	        while(num) {
	                chunkSize = 1; //Size of next 1(0^n)
	                while(!(num & 1) && num) {
	                        chunkSize += 1;
	                        num = num >> 1;
	                }
	                // Now we have the formula for num of representations and zeros for 1(0^n)
	                old = sol;
	                sol = sol * chunkSize + numZeros;
	                numZeros = numZeros + (chunkSize-1)*old;
	                num = num >> 1;
	        }
	        printf("sol is %ld\n", sol);
	}
## Answer
When run we get our solution:

     gcc Problem169.c && time ./a.out && rm a.out
	sol is 178653872807

	real    0m0.001s
	user    0m0.000s
	sys     0m0.000s
(1 millisecond solution!!!)
> Written with [StackEdit](https://stackedit.io/).
