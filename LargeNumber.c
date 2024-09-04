#include <stdlib.h>
#include <stdio.h>


typedef struct {
	unsigned int size; // in terms of 4 byte chunks
	int sign; // 0 if positive, 1 if negative, can use 4 bytes wihtout changing struct size due to alignment
	unsigned int *num;
} largenumber;


void displayLarge(unsigned int *num, int size) {
	if(size) { 
		displayLarge(num+1, size-1);
		printf("%x ", *num);
	}
	else 
		printf("%x ", 0);
}

void displayLargeNum(largenumber *large) {
	displayLarge(large->num, large->size);
	printf("\n");
}

largenumber *initLargeNumber() {
	largenumber *large = calloc(1, sizeof(largenumber));
	large->size = 4; //smallest it should ever be
	large->sign = 0;
	large->num = calloc(large->size, sizeof(unsigned int));
	return large;
}

void resize(largenumber *large, int add) { //Can both scale up and down
	large->num = realloc(large->num, large->size + add);
	large->size += add;
}

void carryOne(largenumber *large, unsigned int chunk) {//other functions should call resize before calling this one so there WILL be space
	unsigned int before = *(large->num+chunk);
	*(large->num+chunk) = before + 1;
	if(before > *(large->num+chunk)) 
		carryOne(large, chunk+1);
}

void addLargeNumber(largenumber *large, unsigned long add) {
	if (*(unsigned long *)(large->num+large->size-2)) { //check last two byte to be sure, 
		resize(large, 2); //No more than two needed
	}
	unsigned long least8 = *(unsigned long *)(large->num);
	unsigned long newLeast8 = least8 + add;
	if(newLeast8 < least8) //handle overflows
		carryOne(large, 2);
	*(unsigned long *)(large->num) = newLeast8;
}

int main() {
//	printf("Library, do not run");
	largenumber *l = initLargeNumber();
	displayLargeNum(l);
	addLargeNumber(l, 0xffffffffffffffff);	
	displayLargeNum(l);
	addLargeNumber(l, 0xffffffffffffffff);	
	displayLargeNum(l);
}
