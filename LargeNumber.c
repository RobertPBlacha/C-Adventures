#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "LargeNumber.h"

struct large {
	unsigned int size; // in terms of 4 byte chunks
	int sign; // 0 if positive, 1 if negative, can use 4 bytes wihtout changing struct size due to alignment
	unsigned int *num;
};


void displayLarge(unsigned int *num, int size) {
	if(size) { 
		displayLarge(num+1, size-1);
		printf("%08x ", *num);
	}
	else 
		printf("%08x ", 0);
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
largenumber *initSizedLargeNumber(unsigned int size) {
	largenumber *large = calloc(1, sizeof(largenumber));
	large->size = size;
	large->sign = 0;
	large->num = calloc(size, sizeof(unsigned int));
}
largenumber *initMemLargeNumber(unsigned int *arr) {
	unsigned int i = 0;
	while(*(arr+i)) //the chance that a random number will contain an all zero unsigned int is 2**-32
		i += 1;
	largenumber *l = calloc(1, sizeof(largenumber));
	l->num = calloc(i , sizeof(unsigned int));
	memcpy(l->num, arr, i*sizeof(unsigned int));
	l->size = i;
	l->sign = *(l->num+i-1) & 0x80000000;
	return l;
}

largenumber *initvLargeNumber(int arg_count, ...) {
	va_list ap;
	va_start(ap, arg_count);
	unsigned int *values = calloc(arg_count+2, sizeof(unsigned int));
	for(int i = 0; i < arg_count; i++) { 
		*(values+i) = va_arg(ap, unsigned int);
	}
	va_end(ap);
	largenumber *large = calloc(1, sizeof(largenumber));
	large->size = arg_count + 2;
	large->sign = 0;
	large->num = values;	
}

void freeLarge(largenumber *l) {
	free(l->num);
	free(l);
}

void resize(largenumber *large, int add) { //Can both scale up and down
	large->num = realloc(large->num, sizeof(unsigned int) * (large->size + add));
	unsigned int old = large->size;
	large->size += add;
	for(int i = old; i < old+add; i++)
		*(large->num+i) = 0;
}

void smartResize(largenumber *large) {
	for(int i = large->size-1; i > 0; i--) {
		if(*(large->num+i)) {
			resize(large, i-large->size+1);
			break;	
		}
	}
}
void carryOne(largenumber *large, unsigned int chunk) {//other functions should call resize before calling this one so there WILL be space
	unsigned int before = *(large->num+chunk);
	*(large->num+chunk) = before + 1;
	if(before > *(large->num+chunk)) 
		carryOne(large, chunk+1);
}
largenumber *copyLarge(largenumber *large) {
	largenumber *res = calloc(1, sizeof(largenumber));
	res->num = calloc(large->size, sizeof(unsigned int));
	memcpy(res->num, large->num, large->size * sizeof(unsigned int));
	res->size = large->size;
	res->sign = large->sign;
	return res;
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

unsigned int determineSize(unsigned int mult) {
	unsigned int ret = 0; 
	mult >>= 1; //multiplying by 1 should not increase size
	while(mult) {
		mult >>= 1;
		ret += 1;
	}
	unsigned int res = ret/(sizeof(unsigned int)*8); //resize takes 4 bytes each call
	if(res == 0 && ret > 0) 
		return 1;
	else 
		return res;
}

void addTwoLargeNumbers(largenumber *addend, largenumber *addto) {
	if(addend->size < addto->size)
		resize(addend, addend->size - addto->size);
	unsigned long cursor = 0;
	unsigned int carry = 0;
	for(unsigned int i = 0; i < addend->size; i++) {
		cursor = (unsigned long)*(addend->num+i) + (unsigned long)*(addto->num+i) + (unsigned long)carry;
		*(addend->num+i) = (unsigned int)cursor;
		carry = (unsigned int)(cursor>>32);
	}
}	

void subTwoLargeNumbers(largenumber *addend, largenumber *addto) { // Just assumes addend > addto
	unsigned int cursor = 0;
	for(unsigned int i = 0; i < addto->size; i++) {
		cursor = *(addend->num+i) - *(addto->num+i);
		if(cursor > *(addend->num+i)) {
			*(addend->num+i+1) -= 1;
		}
		*(addend->num+i) = cursor;
	}
}
void multiplyLargeNumber(largenumber *large, unsigned int mult) {
	resize(large, determineSize(mult));
	unsigned long multiplicationCursor = 0;
	unsigned int new = 0;
	unsigned int carry = 0; //The product of two numbers <2^32 will be bounded by 2^64, but adding the carry to the next number could overflow, need to fix
	for(int i = 0; *(large->num + i) || carry; i++) {
		multiplicationCursor = (unsigned long)*(large->num+i) * (unsigned long)mult;
		*(large->num+i) = (unsigned int)multiplicationCursor + carry;
		carry = (unsigned int)(multiplicationCursor >> 32);
		if(*(large->num+i) < (unsigned int)multiplicationCursor)
			carry += 1; //Carry will be at most fffffffe, so adding one has 0 chance of overflow
	}
}

void zeroLargeNumber(largenumber *large, unsigned int size) { 
	free(large->num);
	large->num = calloc(size, sizeof(unsigned int));
	if(!large->num) {
		printf("Memerror, exiting\n");
		exit(1);
	}
	large->size = size;
}

void shiftLargeNumber(largenumber *large, unsigned int amount) {
	resize(large, amount);
	for(unsigned int i = large->size - 1; i-amount > 0; i--) {
		*(large->num+i) = *(large->num+i - amount);
	}
	for(unsigned int i = amount-1; i != -1; i--) {
		*(large->num+i) = 0;
	}
}
void shiftDownLargeNumber(largenumber *large, unsigned int amount) {
	for(unsigned int i = 0; i+amount != large->size; i++) {
		*(large->num+i) = *(large->num+i + amount);
	}
	for(unsigned int i = amount; i != large->size; i++) {
		*(large->num+i) = 0;
	}
	resize(large, -1 * amount);
}
largenumber *multiplyTwoLargeNumbers(largenumber *base, largenumber *factor) {
	largenumber *l = initSizedLargeNumber(base->size +factor->size);//Theoretically could cause overflow but unlikely for the sizes I'm going to use this library for
	largenumber *res = initSizedLargeNumber(base->size + factor->size);
	//Naive Multiplication, should change later to something not O(n^2)
	unsigned long resL = 0;
	for(unsigned int i = 0; i < base->size; i++) {
		for(unsigned int j = 0; j < factor->size; j++) {
			zeroLargeNumber(l, l->size);
			resL = (unsigned long)*(base->num+i) * (unsigned long)*(factor->num+j);
			*(l->num+i+j) = (unsigned int)resL;
			*(l->num+i+j+1) = (unsigned int)(resL>>32);
			addTwoLargeNumbers(res, l);
		}
	}
	freeLarge(l);
	smartResize(res);
	return res;
}
int equalsLarge(largenumber *l, largenumber *l2) {
	if(l->size < l2->size)
		return equalsLarge(l2, l);
	for(int i = 0; i < l2->size; i++) {
		if(*(l->num+i) != *(l2->num+i))
			return 0;
	}
	return l->size > l2->size && l->num+l2->size;
}
int greaterThanLarge(largenumber *l, largenumber *l2) {
	if(l->size < l2->size) 
		if(equalsLarge(l, l2))
			return 0;//handles case of l=l2 but different sizes
		else
			return !greaterThanLarge(l2, l);
	for(int i = l->size-1; i >l2->size-1; i--) {
		if(*(l->num+i))
			return 1;
	}
	unsigned int a, b;
	for(int i = l2->size-1; i > -1; i--) {
		a = *(l->num+i);
	        b = *(l2->num+i);
		if(a!=b)
			return a > b;
	}
	return 0;
}	
largenumber *modTwoLargeNumbers(largenumber *l, largenumber *mod) {
	largenumber *c = copyLarge(l); 
	largenumber *modC = copyLarge(mod);
	while(greaterThanLarge(c, modC)) {
		shiftLargeNumber(modC, 1); //mod = mod*2^32 until they are within the same block
	}	
	if(equalsLarge(modC, c)) {
		freeLarge(c);
		freeLarge(modC);
		return initLargeNumber();
	}
	printf("Done Shifting\n");
	if(!equalsLarge(modC, mod))
		shiftDownLargeNumber(modC, 1);
	subTwoLargeNumbers(c, modC);
	while(greaterThanLarge(c, mod))
		subTwoLargeNumbers(c, mod);
	freeLarge(modC);
	return c;
}
largenumber *fastModLarge(largenumber *l, largenumber *mod) {
	//faster
	
}
int main() {
//	printf("Library, do not run");
	FILE *rand = fopen("/dev/urandom", "r");
	unsigned int num1[5];
       	unsigned int num2[4];
	fread(&num1, sizeof(unsigned int), 4, rand);	
	fread(&num2, sizeof(unsigned int), 3, rand);	
	num1[4] = 0;
	num2[3] = 0;
	largenumber *l = initMemLargeNumber(num1);
	largenumber *l2 = initMemLargeNumber(num2);
	printf("Modulo\n");
	displayLargeNum(l);
	displayLargeNum(l2);
	printf("Result\n");
	largenumber *l3 = modTwoLargeNumbers(l, l2);
	displayLargeNum(l3);
	freeLarge(l);
	freeLarge(l2);
	freeLarge(l3);
}
