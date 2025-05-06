# Problem 168
## Prompt 
Consider the number $142857$. We can right-rotate this number by moving the last digit ($7$) to the front of it, giving us $714285$.  
It can be verified that $714285 = 5 \times 142857$.  
This demonstrates an unusual property of $142857$: it is a divisor of its right-rotation.

Find the last $5$ digits of the sum of all integers $n$, $10 < n < 10^{100}$, that have this property.
## Methodology  
### $10^{100}$ is too big
From the start we can see that $10^{100}$ is too big to be effectively stored in a long. Our solution will need to be able to check if a number holds the property without being able to store it in a primitive
### Examining the Example
When we start to verify the equation given to us we can see something interesting: 

$5 \times 7 = 5$, carry the $3$\\
$5 \times 5 = 5+3 = 8$ carry the $2$\\
$5\times 8 = 0 + 2 = 2$ carry the $4$\\
$5 \times 2 = 0 + 4 = 4$ carry the $1$\\
$5 \times 4 = 0 + 1 = 1$ carry the $2$\\
$5 \times 1 = 5 + 2 = 7$ carry the $0$\\

We can see that digit by digit, the multiplication of the factor and the $n$ th digit will yield the $(n+1)$ th digit. This sequence concludes with yielding the original digit, with a carry of zero.

We can use this pattern to iterate through the possible last digits and multipliers to efficiently enumerate all right rotation divisors.
### The Numbers to Check
We can trivially determine that the last digit must be between 1 and 9, because it is a digit and an all zero number is not valid. 

Because $d_0,d_{n-1},\dots d_1 = k\times d_{n-1},d_{n-2},\dots d_0$ for digits $d_i$, we know that $k < 10$ because if it was not, $d_0,d_{n-1},\dots d_1$ would have more digits.

We can also show that $k \leq d_0$ by examining how to ensure that the carry is effected if $k > d_0$, but the actual time save on this is negligible.

Now that we can iterate through possible values for $d_0$ and $k$ we just need to ensure that there is more than 1 digit in the number and that we sum them correctly.
### Storing the sequence
We still need to store these 100 digits somewhere, however because we know that we are only generating subsequent digits based on a multiplier and the most significant digit, we can use an array of 1 byte integers, because at max we will be calculating $9\times9<256$
### The Sum
We want to calculate the last $5$ digits of the sum of the right rotation divisor numbers. Because the digits past the $5$th digit will not effect this sum we can just take the last 5 digits of the right rotation divisors, and sum them under modulus $100000$
## Code
	#include <stdlib.h>
	#include <stdio.h>

	char num[100];
	int len = 100;
	int sum = 0;

	void updateSum(int ln) {
	        int new = 0, ten = 1;
	        if(ln < 2)
	                return;
	        if(ln > 5)
	                ln = 5;
	        for(int i = 0; i < ln; i++) {
	                new += num[i]*ten;
	                ten *= 10;
	        }
	        sum += new;
	        sum = sum % 100000;
	}

	int genRR(int n0, int n1) {
	        int ind = 0;
	        int s = n0;
	        int carry = 0;
	        while(ind < len) {
	                num[ind] = s;
	                s = (num[ind] * n1 + carry) % 10;
	                carry = (num[ind] * n1 + carry) / 10;
	                if(s == n0 && num[ind] && !carry) {
	                        updateSum(ind+1);
	                }
	                ind += 1;
	        }
	}

	int main() {
	        for(int i = 1; i < 10; i++)
	                for(int j = 1; j < 10; j++)
	                        genRR(i, j);
	        printf("sum is %d\n", sum);
	}
## Answer
When run we get our solution:

    gcc Problem168.c && time ./a.out && rm a.out
	sum is 59206

	real    0m0.001s
	user    0m0.000s
	sys     0m0.000s
(1 millisecond solution!!!)
> Written with [StackEdit](https://stackedit.io/).


