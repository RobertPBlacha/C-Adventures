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
	div=gcd(*num, *dec);
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
