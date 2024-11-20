# Problem 173
## Prompt 
We shall define a square lamina to be a square outline with a square "hole" so that the shape possesses vertical and horizontal symmetry. For example, using exactly thirty-two square tiles we can form two different square laminae:
![32 Tile Lamina](https://projecteuler.net/resources/images/0173_square_laminas.gif?1678992055)
With one-hundred tiles, and not necessarily using all of the tiles at one time, it is possible to form forty-one different square laminae.

Using up to one million tiles how many different square laminae can be formed?
## Methodology  
### Rewriting the Problem
We can rewrite the problem in terms of the dimensions of the lamina and its hole. This will help us avoid brute forcing a guess for the lamina $t$ tiles can form. Because the lamina itself must be square, if we define its side length to be $n$, then if the entire area would be filled with tiles it would have $n^2$ tiles. There is of course a hole in the lamina, which also must be a square, so this must have a side length $m$ with the hole itself having an area of $m^2$. This means we can write an equation for $t, m,$ and $n$: $$t=n^2-m^2$$ 
### An interesting property of n and m
Looking at the image provided to us, we can discern something interesting, which is $n$ is even if and only if $m$ is even. We can intuit this but a proof can be evident with a property of $t$, which is that $t$ has to be divisible by 4. 
#### Proof 4 | t:
Let t be the number of tiles in the square lamina represented by $t=n^2-m^2$ for $n, m\in\mathbf{Z}^+$ such that $m < n$
$t$ can be constructed to be a set of concentric squares, because otherwise the shapes will not have horizontal or vertical symmetry. 
The equation of a perimeter of a square with side length $s$ is $4\times s$
So, for the set of concentric squares $t$ forms with side lengths $s_1, s_2, \dots s_k$, $t=\Sigma_{i=1}^k 4\times s_k$
Factoring out the $4$ we get $t=4\times\Sigma_{i=1}^k s_k$, t can be written as an integer multiplied by $4$ so it must be divisible by 4. $\blacksquare$

Knowing this we can do a proof to show that n is even if and only if m is even:
#### Proof
Let $t, m, n \in\mathbf{Z}^+$ such that $t=n^2-m^2$, $m < n$ and $\exists j\in\mathbf{Z}$ such that $t = 4j$
##### $n=2k \implies m=2i$
$t=n^2-m^2\equiv 4j=n^2-m^2$
If $\exists k\in\mathbf{Z}$ such that $n = 2k$:
We can divide both sides by 4, getting $4j = 4k^2-m^2\equiv j = k^2-(\frac{m}{2})^2$
We know that $j,k^2\in\mathbf{Z}^+$ because the positive integers are closed under exponentiation, so $\frac{m}{2}$ must be an integer.
This means $\exists i\in\mathbf{Z}$ such that $m = 2i$
This shows $n=2k \implies m=2i$

Without loss of generality, we can use the above proof to show that $m = 2i \implies n = 2k$
$m = 2i \implies n = 2k \wedge n=2k \implies m=2i$
$m = 2i \iff n = 2k\blacksquare$
### Guessing n and m given t
We have some good ways to filter out good and bad guesses for $n$ and $m$ but we still need to generate them from a given $t$. We can do this by manipulating our earlier equation:
$$t=n^2-m^2=(n-m)(n+m)$$
This means if we factor $t$, we can use the factors to find a guess for $n$ and $m$:
$t=pq$ where $p = (n-m)$ and $q = n+m$
$q-p = n+m-n-(-m) = 2m$
So then $\frac{q-p}{2} = m$ and $n = q-m$
Now from a $t$ we can generate guesses for $n, m$ and check with $n^2-m^2 = t$ and $n\mod2=1 \iff m\mod2=1$
What is especially nice about this method of checking is that no division is necessary, we can check the second condition with $\neg(n\&1 \oplus m\&1)$
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
	                        if((n-m)*(n+m) == a && !(n&1 ^ m&1)) {
	                                total += 1;
	                        }
	                }
	        }
	        return total;
	}

	int main() {
	        long trueTotal = 0;
	        for(long t = 1; t <= 1000000; t++) {
	                trueTotal+=factor(t);
	        }
	        printf("Total is %ld\n", trueTotal);
	}
## Answer
When run we get our solution:

    gcc Problem173.c && time ./a.out && rm ./a.out
	Total is 1572729

	real    0m1.535s
	user    0m1.535s
	sys     0m0.000s

> Written with [StackEdit](https://stackedit.io/).
