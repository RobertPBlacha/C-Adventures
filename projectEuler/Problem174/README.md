# Problem 173
## Prompt 
We shall define a square lamina to be a square outline with a square "hole" so that the shape possesses vertical and horizontal symmetry.

Given eight tiles it is possible to form a lamina in only one way: $3\times3$ square with a $1\times1$ hole in the middle. However, using thirty-two tiles it is possible to form two distinct laminae.
![32 Tile Lamina](https://projecteuler.net/resources/images/0173_square_laminas.gif?1678992055)

If $t$ represents the number of tiles used, we shall say that $t=8$ is type $L(1)$ and $t=32$ is type $L(2)$.
Let $N(n)$ be the number of $t \leq 1000000$ such that  $t$ is type $L(n)$; for example, $N(15) = 832$.

What is $\Sigma_{n=1}^{10}N(n)$?
## Methodology  
### Reusing Previous Concepts
I solved an easier version of this problem [here](https://github.com/RobertPBlacha/C-Adventures/tree/main/projectEuler/Problem173), so most of the explanation for why this solution works will be there
### Small optimizations
We can make one change to our code that makes it work, and another that makes it slightly faster.
The first change is changing the return of `factor()` so that it returns 1 if and only if $1 \leq total \leq 10$
The second change is returning zero whenever $total > 10$, because this is not going to be counted anyway.
With these two changes we have our modified solution
## Code
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
## Answer
When run we get our solution:

    gcc Problem174.c && time ./a.out && rm ./a.out
	Total is 209566

	real    0m1.488s
	user    0m1.488s
	sys     0m0.000s

> Written with [StackEdit](https://stackedit.io/).
