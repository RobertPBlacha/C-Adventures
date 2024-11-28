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
