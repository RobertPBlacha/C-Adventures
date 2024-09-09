#include <stdlib.h>
#include <stdarg.h>
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

void resize(largenumber *large, int add) { //Can both scale up and down
	large->num = realloc(large->num, sizeof(unsigned int) * (large->size + add));
	unsigned int old = large->size;
	large->size += add;
	for(int i = old; i < old+add; i++)
		*(large->num+i) = 0;
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
	displayLargeNum(large);
	resize(large, -1 * amount);
}
//TODO Add multiplication of LargeNumbers
largenumber *multiplyTwoLargeNumbers(largenumber *base, largenumber *factor) {
	//XY = XlYl2^n + ((Xl + Xr)(Yl + Yr) - XlYl - XrYr)2^n/2 + XrYr
	largenumber *l = initLargeNumber();
	resize(l, base->size * factor->size - l->size);
	//multTwoLarge(base, factor, l);
	return NULL;
}

int main() {
//	printf("Library, do not run");
	largenumber *l = initvLargeNumber(3, 0xffffffff, 0xffffffff, 0xffffffff);
	displayLargeNum(l);
	shiftLargeNumber(l, 2);
	displayLargeNum(l);
	shiftDownLargeNumber(l, 3);
	displayLargeNum(l);

}
