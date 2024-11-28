
# Problem 182
## Prompt 
The RSA encryption is based on the following procedure:

Generate two distinct primes $p$ and $q$.  
Compute and $n = pq$ and $\phi =(p-1)(q-1)$.  
Find an integer $e$, $1 < e < \phi$, such that $\gcd(e,\phi) = 1$.

A message in this system is a number in the interval $[0, n-1]$.  
A text to be encrypted is then somehow converted to messages (numbers in the interval $[0, n-1]$).  
To encrypt the text, for each message, $m$, $c = m^e \mod n$ is calculated.

To decrypt the text, the following procedure is needed: calculate $d$ such that $ed \equiv 1 \mod \phi$, then for each encrypted message, $c$, calculate $m = c^d\mod n$.

There exist values of $e$ and $m$ such that $m^e \mod n = m$.  
We call messages for which $m^e mod n = m$ unconcealed messages.

An issue when choosing $e$ is that there should not be too many unconcealed messages.  
For instance, let $p=19$ and $q =37$.  
Then $n = 19\times 37=703$ and $\phi = 18\times 36 = 648$.  
If we choose $e=181$, then, although $\gcd(181, 648) = 1$ it turns out that all possible messages $m (0 \leq m \leq n-1$) are unconcealed when calculating $m^e \mod m$.  
For any valid choice of $e$ there exist some unconcealed messages.  
It's important that the number of unconcealed messages is at a minimum.

Choose $p = 1009$ and $q = 3643$.  
Find the sum of all values of $e, 1<e<\phi(1009, 3643)$ and $\gcd(e,\phi) = 1$, so that the number of unconcealed messages for this value of is at a minimum.

## Methodology
### Understanding the Problem
We are asked to sum values of $e$ such that $e$ is in the set $$\argmin_e |\{m | m^e\mod n \equiv m\}|$$ 
We can modify this equation a bit:
$$m^e \equiv m \mod n\implies\\m^{e-1}\equiv1\mod n$$
### Focusing our Approach
We will make an interesting observation. Any element not in the group $\mathbf{Z}^*_n$ will never follow this. Let's examine a brief proof for why this is:

Let $x \notin \mathbf{Z}^*_n$, This implies $\gcd(x,n) \neq 1$. We know that for the message to be unconcealed $x^e \equiv x\implies x\times x^{e-1}\equiv x \mod n\implies x^{e-1}\equiv 1 \mod n \implies x\times x^{e-2} \equiv 1 \mod n$. This implies $x^{e-2}$ is the multiplicative inverse of $x$ under modulo $n$. But we know that there is no multiplicative inverse of $x$ under modulo $n$ because $\gcd(n,x) \neq 1$, so $x\notin \mathbf{Z}^*_n$ such that $x^e \equiv x$ cannot exist. $\blacksquare$
### Subgroups under $\mathbf{Z}_n^*$
Now we know that any message that becomes unconcealed for $e, 1 < e < \phi$ must be in $\mathbf{Z}_n^*$. Now the equation $m^{e-1} \equiv 1 \mod n$ should be familiar!

We know that under the group $\mathbf{Z}_n^*$, there are $\phi(\phi(n))$ generators, and that all other numbers fall in some subgroup of $\mathbf{Z}_n^*$. These subgroups are of order $q$ such that $q \in \mathbf{Z}$ and $q = \phi(n) / d$ where $d\in \mathbf{Z}$. This means that an element $m$ in this subgroup follows $m^q = 1$.

We can make the connection that for unconcealed messages $m$, $e-1$ must be equal to  some $kq$ where $k\in\mathbf{Z}$ and $q$ is the order of the subgroup the element $m$ belongs to in $\mathbf{Z}_n^*$. 
### Checking e
Because we know that $q \in \mathbf{Z}$ and $q = \phi(n) / d$, we can reason that $q | \phi(n)$, which means $e-1 | \phi(n)$. This is not enough however because we want to be able to relate how many unconcealed messages one $e$ has relative to another.

We can do this by seeing that if $e-1 = q_1\times q_2$, then for both the subgroups corresponding to $q_1$ and $q_2$, $m^e \equiv m$: 
$$m^{q_1}\equiv 1 \mod n \implies (m^{q_1})^{q_2} \equiv 1 \mod n\\m^{q_2}\equiv 1 \mod n \implies (m^{q_2})^{q_1} \equiv 1 \mod n$$
Effectively, we can see that the more factors $e-1$ shares with $\phi$, the more messages will follow this property, because this corresponds to more subgroups that will all be unconcealed.

Whenever we find a shared factor $q$ of $e-1$ and $\phi$, that shared factor represents a subgroup where $m^{e-1} \equiv m \mod n$, and this subgroup has $q$ elements, allowing us to directly count the number of messages that follow this property for every $e$ without ever actually looking at the messages!
## Code
	#include <stdlib.h>
	#include <stdio.h>

	typedef struct node {
	        int factor;
	        struct node *next;
	} node;

	void insertBody(node *head, int f) {
	        node *ptr;
	        if(!head->next) {
	                head->next = calloc(1, sizeof(node));
	                head->next->factor = f;
	                return;
	        }
	        else if(head->next->factor % f == 0)
	                return;
	        else if(head->next->factor > f) {
	                ptr = calloc(1, sizeof(node));
	                ptr->next = head->next;
	                head->next = ptr;
	                ptr->factor = f;
	        }
	        else
	                insertBody(head->next, f);
	}

	node *insert(node *head, int f) {
	        node *ptr;
	        if(!head) {
	                head = calloc(1, sizeof(node));
	                head->factor = f;
	        }
	        else if(f % head->factor == 0)
	                return head;
	        else if(head->factor > f) {
	                ptr = calloc(1, sizeof(node));
	                ptr->next = head;
	                ptr->factor = f;
	                return ptr;
	        }
	        else
	                insertBody(head, f);
	        return head;
	}

	int in(node *head, int f) {
	        int ret = 1;
	        if(!head)
	                return 0;
	        if(f % head->factor == 0) {
	                while(f % head->factor == 0) {
	                        ret *= head->factor;
	                        f /= head->factor;
	                }
	                return ret + in(head->next, f); //This isn't perfect but works with the numbers we are given
	        }
	        return in(head->next, f);
	}

	int gcd(int a, int b) {
	        if(!a)
	                return b;
	        return gcd(b % a, a);
	}

	int main() {
	        long p = 1009, q = 3643;
	        long phiN = 1008*3642;
	        node *factors = NULL;
	        long sum = 0;
	        for(int i = 2; i < q; i++) {
	                if((p-1) % i == 0 || (q-1) % i == 0)
	                        factors = insert(factors, i);
	        }
	        int min = 2; //We know that the minimum is at least 2, because e must be odd and phi must be even so e-1 will always share a factor of 2.
	        while(!sum) {
	                for(long i = 2; i < phiN; i++) {
	                        if(in(factors, i-1) == min && gcd(i, phiN) == 1) {
	                                sum += i;
	                        }
	                }
	                min += 1;
	        }
	        printf("%ld\n", sum);
	        node *f = factors;
	        while(f) {
	                factors = f;
	                f = f->next;
	                free(factors);
	        }
	}
## Answer
When run we get our solution:

	gcc Problem182.c && time ./a.out && rm ./a.out
	399788195976

	real    0m0.153s
	user    0m0.153s
	sys     0m0.000s

> Written with [StackEdit](https://stackedit.io/).
