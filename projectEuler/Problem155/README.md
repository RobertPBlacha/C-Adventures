# Problem 155
## Prompt 
An electric circuit uses exclusively identical capacitors of the same value $C$.  The capacitors can be connected in series or in parallel to form sub-units, which can then be connected in series or in parallel with other capacitors or other sub-units to form larger sub-units, and so on up to a final circuit.

Using this simple procedure and up to $n$ identical capacitors, we can make circuits having a range of different total capacitances. For example, using up to $n=3$ capacitors of $60\mu F$ each, we can obtain: $$(180\mu F, 120\mu F, 90 \mu F, 60\mu F, 45\mu F, 30\mu F, 20\mu F)$$
If we denote $D(n)$ by the number of distinct total capacitance values we can obtain when using up to $n$ equal-valued capacitors and the simple procedure described above, we have: $D(1) = 1, D(2) = 3, D(3) = 7, \dots$
Find $D(18)$
_Reminder:_ When connecting capacitors etc in parallel, the total capacitance is $C_T = C_1+C_2+\dots$  
whereas when connecting them in series, the overall capacitance is given by: $\frac{1}{C_T} = \frac{1}{C_1}+\frac{1}{C_2}+\dots$
## Methodology  
### Recursion
To find $D(18)$ we will need to find $D(n) | n < 18$ 
This is because included in the set of $D(n)$ is any unique values of $D(n-1)$ which can only be known by finding the set representing $D(n-1)$. Even if an element in $D(m-1)$ is already present in $D(m)$ for some $m < n$, we will want to have some memory of it because it could sum with some element in $D(n-m)$ to form a unique capacitor in $D(n)$
The need to find solutions for smaller values of $n$ hints at a recursive solution to the problem, which we will keep in mind as we attempt to represent a capacitor, or combination of capacitors in memory.
### Creating New Capacitors
 A very important insight is that when creating new connections of capacitors, we will only need to combine two connections at a time. This is because for any combination of three, a combination of two of the three connections will already be found by our recursive process. This means we can simplify finding new capacitors to finding two connections with numbers of capacitors that add up to $n$, and combining them in parallel or in series.
### Data Representation
 For a while trying to solve the problem, I tried to represent the capacitance of a circuit $c$ as a real number (or a long double in C). This made the program significantly slower and led to problems with precision until I realized that I could instead represent $c$ as a rational number (a numerator and denominator). Lets examine why this is true with a little proof by induction:
 #### Base case
 For $n=1$, $c=1$ can be represented as $\frac{1}{1}$
 #### Inductive Step
 For any two connections of capacitors $c_1, c_2$ which can be represented as $\frac{n_1}{d_1}, \frac{n_2}{d_2}$ they can either be connected in series or in parallel
 ##### Parallel
 $\frac{n_1}{d_1} + \frac{n_2}{d_2} = \frac{n_1d_2+n_2d_1}{d_1d_2}$
 The integers are closed under addition and multiplication, so $n_1d_2+n_2d_1\in\mathbf{Z}$ and $d_1d_2\in\mathbf{Z}$
 Because the numerator and denominator are integers, the parallel combination of any two capacitor connections has a rational capacitance $\blacksquare$
 ##### Series
 $\frac{1}{\frac{d_1}{n_1} + \frac{d_2}{n_2}} = \frac{n_1n_2}{n_1d_2+n_2d_1}$
 The integers are closed under addition and multiplication, so $n_1d_2+n_2d_1\in\mathbf{Z}$ and $n_1n_2\in\mathbf{Z}$
 Because the numerator and denominator are integers, the series combination of any two capacitor connections has a rational capacitance $\blacksquare$
 
 This means that we can represent the capacitors as two unsigned integers, and never worry about integer precision or costly division

We will also need to store the size of these connections,  the number of capacitors needed to make them, this can be stored as integer, since it will never exceed 18.
 ### Storage
 We will want to store these capacitors in a data structure that is conducive to quickly checking if an element with a specific value exists. This motivates the use of a binary tree, but we need to find a way to efficiently compare the numerator and denominator of two capacitors.
 If we always ensure that the numerator and denominator are simplified, we can simply check if `n_1==n_2 && d_1==d_2`. For checking if the capacitance of a connection is less than or greater than the capacitance of another, we can avoid multiplication with some algebra:
 $\frac{n_1}{d_1} > \frac{n_2}{d_2} \implies n_1\times d_2>n_2\times d_1$
 This again allows us to not worry about division precision, which is nice and speeds the program up significantly.
 ### Combining Answers
 We now have 18 different trees representing the different connections of capacitors we can make, but we still need to combine them into one final tree. Thankfully, because we have already defined a binary tree that does not take duplicates to store the sets of $D(n)$, we can iterate through all of these elements and combine them into one final tree, which we can then count to get our answer.
 ## Code
 

    #include <stdlib.h>
	#include <stdio.h>

	typedef struct capacitor {
	        unsigned int capacitance[2];
	        struct capacitor *left;
	        struct capacitor *right;
	        int size;
	} capacitor;

	capacitor** trees;

	void insertCap(unsigned int capacitance1, unsigned int capacitance2, int size, capacitor* node) {
	        if(!trees[size]){
	                trees[size] = calloc(1, sizeof(capacitor));
	                trees[size]->capacitance[0] = capacitance1;
	                trees[size]->capacitance[1] = capacitance2;
	                trees[size]->size = size;
	                return;
	        }
	        if(capacitance1 == node->capacitance[0] && capacitance2 == node->capacitance[1]) {
	                return;
	        }
	        else if(node->capacitance[0]*capacitance2 < capacitance1*node->capacitance[1]) {
	                if(!node->right) {
	                        node->right = calloc(1, sizeof(capacitor));
	                        node->right->size = size;
	                        node->right->capacitance[0] = capacitance1;
	                        node->right->capacitance[1] = capacitance2;
	                }
	                else {
	                        insertCap(capacitance1, capacitance2, size, node->right);
	                }
	        }
	        else {
	                if(!node->left) {
	                        node->left = calloc(1, sizeof(capacitor));
	                        node->left->size = size;
	                        node->left->capacitance[0] = capacitance1;
	                        node->left->capacitance[1] = capacitance2;
	                }
	                else {
	                        insertCap(capacitance1, capacitance2, size, node->left);
	                }
	        }
	}

	long sumTree(capacitor * node) {
	        long sum = 0;
	        if(node->left) {
	                sum += sumTree(node->left);
	        }
	        if(node->right) {
	                sum += sumTree(node->right);
	        }
	        sum += 1;
	        return sum;
	}

	unsigned int gcd(unsigned int a, unsigned int b) {
	        if(b == 0)
	                return a;
	        else
	                return gcd(b,a%b);
	}

	void reduce(unsigned int *num, unsigned int *dec) {
	        unsigned int div;
	        div=gcd(*num, *dec)
            *num /= div;
            *dec /= div;
	}

	void iterate(capacitor *small, capacitor *tree) {
	        int newSize = small->size + tree->size;
	        unsigned int num = small->capacitance[0]*tree->capacitance[1]+small->capacitance[1]*tree->capacitance[0];
	        unsigned int dec = small->capacitance[1]*tree->capacitance[1];
	        reduce(&num, &dec);
	        insertCap(num, dec, newSize, trees[newSize]);
	        num = small->capacitance[1]*tree->capacitance[0]+small->capacitance[0]*tree->capacitance[1];
	        dec = small->capacitance[0]*tree->capacitance[0];
	        reduce(&dec, &num);
	        insertCap(dec, num, newSize, trees[newSize]);
	        if(tree->left)
	                iterate(small, tree->left);
	        if(tree->right)
	                iterate(small, tree->right);
	}

	void combine(capacitor *small, capacitor *large) {
	        iterate(small, large);
	        if(small->left)
	                combine(small->left, large);
	        if(small->right)
	                combine(small->right, large);
	}

	void recurse(int size) {
	        if(size == 1) {
	                insertCap(1,1,1,trees[1]);
	                return;
	        }
	        recurse(size-1);
	        for(int i = 1; i <= size/2; i++) {
	                combine(trees[i], trees[size-i]);
	        }
	        return;
	}
	void add(capacitor *node) {
	        insertCap(node->capacitance[0], node->capacitance[1], 0, trees[0]);
	        if(node->left)
	                add(node->left);
	        if(node->right)
	                add(node->right);
	        free(node);
	}

	void addToZero(int size) {
	        for(int i = 1; i <= size; i++)
	                add(trees[i]);
	}

	void removeFromZero(capacitor *node) {
	        if(node->left)
	                removeFromZero(node->left);
	        if(node->right)
	                removeFromZero(node->right);
	        free(node);

	}

	int main() {
	        int target = 18;
	        trees = calloc(target+1, sizeof(capacitor *));
	        recurse(target);
	        addToZero(target);
	        printf("Sum is %ld\n", sumTree(trees[0]));
	        removeFromZero(trees[0]);
	        free(trees);
	}
## Answer
When run we get our solution:

    gcc Problem155.c && time ./a.out
    Sum is 3857447

	real    0m7.954s
	user    0m7.864s
	sys     0m0.090s

> Written with [StackEdit](https://stackedit.io/).
