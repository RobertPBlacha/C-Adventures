
# Problem 158
## Prompt 
Taking three different letters from the $26$ letters of the alphabet, character strings of length three can be formed.  
Examples are 'abc', 'hat' and 'zyx'.  
When we study these three examples we see that for 'abc' two characters come lexicographically after its neighbour to the left.  
For 'hat' there is exactly one character that comes lexicographically after its neighbour to the left. For 'zyx' there are zero characters that come lexicographically after its neighbour to the left.  
In all there are $10400$ strings of length $3$ for which exactly one character comes lexicographically after its neighbour to the left.

We now consider strings of $n \leq 26$ different characters from the alphabet.  
For every $n$, $p(n)$ is the number of strings of length $n$ for which exactly one character comes lexicographically after its neighbour to the left.

What is the maximum value of $p(n)$?
## Methodology  
### Solving for n = 2
Trying to solve the problem for the smallest possible case will reveal some important information. Starting with $n=2$, (because $n=1$ has no solutions), we can see that given the first digit $n_0$, the second digit $n_1$ must satisfy $n_0 < n_1$. This means that we can iterate through all possible values of $n_0$, and we will know that there are $25-n_0$ elements in the set of characters that are less than $n_0$. For example, given $n_0 = 0$, or equivalently the first character being 'A', we know there are $25$ letters that are after 'A' in the alphabet.
### Generalizing the thinking
Let's continue to assume that $n_0 < n_1$, but now what if we have more than one spot after our first digit? We know that the sequence $n_1, n_2, \dots, n_i$ must be strictly decreasing, which can help us construct a closed form solution for all sequences where $n_0 < n_1$.

Given a finite set of distinct elements, there is only one sequence of them that is strictly decreasing. You can consider this to be the reverse sorted list of the elements. We know that the set of possible elements for $n_1, n_2, \dots, n_i$ is the set of all characters that are not $n_0$. So for $n_0 = 0$, or 'A', the set of remaining characters are from 'B' to 'Z'. Their reverse sorted order is $[Z, Y,\dots , B]$. Now we have $i$ places to fill from these elements. We can choose any $i$ indices from this list, and this will give a valid string. So for $i = 3$, we can choose $1, 6,10$ which corresponds to the string "AYTP", which is a valid string.

Given a length $n$ and a $n_0 = k$, we can calculate the number of all such strings using ${ 25 \choose n-1}$. We see that in this case the actual number of $n_0$ does not matter unless length is 2 or if $n_0 = 25$, where the formula is ${ 25-k \choose 1}$. This is because any element after $n_0$ can come after elements before $n_0$. This will be resolved in the next section
###  Generalizing for any index
 $$\sum_{k=0}^{25}{ 25 \choose n-1}$$ 
Right now our formula above works for any string where the ordered pair is the first pair of characters, but how can we generalize this further?

Consider constructing a string of length 3, and arbitrarily choosing the first letter to be 'C'. We know from our first formula that the number of strings like 'CZA', where the alphabetically ordered pair is first is ${ 25 \choose 2}$. Let us now consider the case where the next character is not less than 'C'. Choosing 'A' means the last character that is less than A, and not C, for example 'CAZ'. Returning to our model of a reverse sorted order of a set, we have a set of size 24, with one possible choice of index. This gives the general formula ${ 26-i \choose n-i}$. Here i is 2 because we are at the second index, and $n$ is 3. 

With these we can construct a recursive function that solves the problem in efficient time:
## Code
		
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
## Answer
When run we get our solution:

    gcc Problem158.c && time ./a.out && rm a.out
	409511334375

	real    0m8.953s
	user    0m8.952s
	sys     0m0.000s

> Written with [StackEdit](https://stackedit.io/).
