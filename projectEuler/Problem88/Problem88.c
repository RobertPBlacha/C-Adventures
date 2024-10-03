#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define SIZE 12000

unsigned int **allFactors;
unsigned int avail[SIZE*2];
unsigned int add[SIZE*2];
unsigned int* factor(unsigned int n) {
	unsigned int counter = 0;
	unsigned int *fact = calloc(100, sizeof(unsigned int)); //gross overestimation
	for(int i = 1; i*i <= n; i++) {
		if(n%i == 0) {
			*(fact+counter) = i;
			*(fact+counter+1) = n/i;
			counter += 2;
		}
	}
	fact = realloc(fact, (counter+2)*sizeof(unsigned int));
	return fact;
}

unsigned int *getFactor(unsigned int n) {
	if(avail[n])
		return allFactors[n];
	avail[n] = 1;
	allFactors[n] = factor(n);
	return allFactors[n];
}

unsigned int summation(unsigned int *nums, unsigned int size) {
	unsigned int sum = 0;
	for(int i = 0; i < size; i++) { 
		sum += *(nums+i);
		//printf("%u, ", *(nums+i));
	}
	//printf("Returning %u\n", sum);
	return sum;
}

int permuteNEW(unsigned int* oldNums, unsigned int size, unsigned int sum, unsigned int totalSize) {
	if(size > totalSize)
		return 0;
	unsigned int *newNums = malloc(sizeof(unsigned int) * (size+1));
	unsigned int *newFacts;
	memcpy(newNums, oldNums, size*sizeof(unsigned int));
	for(int i = 0; i < size; i += 1) {
		newFacts = getFactor(newNums[i]);
		for(int j = 2; j < oldNums[i]; j+=2) {
			if(!newFacts[j])
				break;
			newNums[i] = newFacts[j];
			newNums[size] = newFacts[j+1];
			if(summation(newNums, size+1) + totalSize-(size+1) == sum) {
				free(newNums);
				return 1;	
			}
			if(permuteNEW(newNums, size+1, sum, totalSize)) { 
				free(newNums);
				return 1; 
			}
		}
		newNums[i] = oldNums[i];
	}
	free(newNums);
	return 0;
}

int main() {
	unsigned long result = 0;
	unsigned int n;
	unsigned int max = 0;
	unsigned int size = SIZE;
	for(int i = 0; i < SIZE*2; i++) {
		add[i] = 0;
		avail[i] = 0;
	}
	allFactors = calloc(SIZE*2, sizeof(unsigned int *));
	int step = 2;
	for(int i = 2; i < SIZE+1; i++) {
		n = i;
		while(!permuteNEW(&n, 1, n, i)) {
			n = n+1;
		}
		if(i % 1000 == 0)
			printf("%u\n", i);
		//printf("%u for %u\n", n, i);
		if(!add[n]) {
			result += n;
			add[n] = 1;
		}
	}
	for(int i = 0; i < SIZE*2; i++) 
		if(avail[i]) {
			free(allFactors[i]);
		}
	free(allFactors);
	printf("Result is %lu\n", result);
}
