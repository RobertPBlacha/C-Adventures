#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "LargeNumber.h"

struct large {
	unsigned long size; // in terms of 4 byte chunks
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
	large->num = calloc(large->size, sizeof(unsigned int));
	return large;
}
largenumber *initSizedLargeNumber(unsigned int size) {
	largenumber *large = calloc(1, sizeof(largenumber));
	large->size = size;
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
	large->num = values;	
}

void freeLarge(largenumber *l) {
	free(l->num);
	free(l);
}

void resize(largenumber *large, long add) { //Can both scale up and down
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
		resize(addend, addto->size - addend->size);
	unsigned long cursor = 0;
	unsigned int carry = 0;
	for(unsigned int i = 0; i < addend->size; i++) {
		cursor = (unsigned long)*(addend->num+i) + (unsigned long)*(addto->num+i) + (unsigned long)carry;
		*(addend->num+i) = (unsigned int)cursor;
		carry = (unsigned int)(cursor>>32);
	}
}	
void subLargeNumber(largenumber *addend, unsigned int sub) {
	unsigned int cursor = 0;
	for(unsigned int i = 0; i < addend->size; i++) {
		cursor = *(addend->num+i) - sub;
		if(cursor > *(addend->num+i)) {
			*(addend->num+i+1) -= 1;
			*(addend->num+i) = cursor;
		}
		else {
			*(addend->num+i) = cursor;
			break;
		}
	}
}
void subTwoLargeNumbers(largenumber *addend, largenumber *addto) { // Just assumes addend > addto
	unsigned int cursor = 0;
	for(unsigned int i = 0; i < addto->size && i < addend->size; i++) {
		if(!*(addto->num+i) && !*(addend->num+i)) {
			break;
		}
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
	for(unsigned int i = large->size - 1; i-amount != -1; i--) {
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
	largenumber *l = initSizedLargeNumber(base->size+factor->size); //Theoretically could cause overflow but unlikely for the sizes I'm going to use this library for
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
		if(*(l->num+i) != *(l2->num+i)) {
			return 0;
		}
	}
	return l->size == l2->size || (l->size > l2->size && !*(l->num+l2->size));
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
void granularShiftDown(largenumber *l, unsigned int i) { //shifts by chars
	char *cast = (char *)(l->num);
	unsigned int total = sizeof(unsigned int) * (l->size);
	char * a = malloc(total);
	memcpy(a, cast, total); //This memory passing is necessary because memcpy is undefined for overlapping segments (in this case it copied 4 bytes and then stopped)
	memcpy(cast, a+i, total - i);
	free(a);
	*(cast+total-1)=0;
	smartResize(l);
}
void granularShiftUp(largenumber *l, unsigned int i) { //shifts by chars
	if(*(l->num+l->size-1)) {
		resize(l, 1);
	}
        char *cast = (char *)(l->num);
        unsigned int total = sizeof(unsigned int) * (l->size-1);
        char * a = calloc(sizeof(char), total);
        memcpy(a, cast, total); //This memory passing is necessary because memcpy is undefined for overlapping segments (in this case it copied 4 bytes and then stopped)
        memcpy(cast+i, a, total);
        free(a);
	for(int j = 0; j < i; j++) {
        	*(cast+j)=0;
	}
}
void setLargeEqual(largenumber *dest, largenumber *src) {
	free(dest->num);
	dest->num = calloc(src->size, sizeof(unsigned int));
	memcpy(dest->num, src->num, sizeof(unsigned int) * src->size);
	dest->size = src->size;
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
	while(greaterThanLarge(c, mod) || equalsLarge(c, mod)) {
		while(greaterThanLarge(modC, c)) {
			granularShiftDown(modC, 1);
		}
		subTwoLargeNumbers(c, modC);
	}
	freeLarge(modC);
	return c;
}
largenumber *divTwoLargeNumbers(largenumber *l, largenumber *mod) {
        largenumber *c = copyLarge(l);
        largenumber *modC = copyLarge(mod);
	largenumber *ret = initLargeNumber();
	largenumber *zero = initLargeNumber();
	unsigned long shift = 0;
        while(greaterThanLarge(c, modC)) {
                shiftLargeNumber(modC, 1); //mod = mod*2^32 until they are within the same block
		shift += 1;
        }
        if(equalsLarge(modC, c)) {
                freeLarge(c);
                freeLarge(modC);
		addLargeNumber(ret, 1); 
		shiftLargeNumber(ret, shift);
                return ret;
        }
	// We now know the size of the return value
	resize(ret, shift/sizeof(unsigned int)+1);
        while(greaterThanLarge(c, mod)) {
                while(greaterThanLarge(modC, c) && !equalsLarge(modC, c)) {
                        granularShiftDown(modC, 1);
			if(!equalsLarge(ret, zero)) {
				granularShiftUp(ret, 1);
			}
                }
                subTwoLargeNumbers(c, modC);
		addLargeNumber(ret, 1);
        }
        freeLarge(modC);
	freeLarge(c);
	free(zero);
        return ret;
}

largenumber *largeModPow(largenumber *l, largenumber *pow, largenumber *mod) {	
	largenumber *res = initvLargeNumber(2, 0x1, 0);
	largenumber *powC = copyLarge(pow);
	largenumber *lC = copyLarge(l);
	largenumber *zero = initLargeNumber();
	largenumber *work;
	unsigned char LSC;
	work = modTwoLargeNumbers(lC, mod);
	setLargeEqual(lC, work);
	freeLarge(work);
	while(greaterThanLarge(powC, zero)) {
		LSC = (unsigned char)(*(powC->num));
		for (int i = 0; i < 8; i++) {
			if (LSC & 1) {
				work = multiplyTwoLargeNumbers(res, lC);
				setLargeEqual(res, work);//three lines needed for memory reasons
				freeLarge(work);	
				work = modTwoLargeNumbers(res, mod);
				setLargeEqual(res, work);
				freeLarge(work);
			}
			work = multiplyTwoLargeNumbers(lC, lC);
			setLargeEqual(lC, work);
			freeLarge(work);

			work = modTwoLargeNumbers(lC, mod);
			setLargeEqual(lC, work);
			freeLarge(work);

			LSC >>= 1;
		}
		granularShiftDown(powC, 1);
	}
	freeLarge(powC);
	freeLarge(lC);
	freeLarge(zero);
	return res;
}
void eulerExtended(largenumber *a, largenumber *b, largenumber **x, largenumber **y, largenumber *zero, largenumber *mod) { //zero used for checks
	if(equalsLarge(zero, a)) {
		*x = initvLargeNumber(1, 0);
		*y = initvLargeNumber(1, 1);
		return;
	}
	largenumber *x1, *y1;
	largenumber *b2 = modTwoLargeNumbers(b, a);
	largenumber *scratch;
	eulerExtended(b2, a, &x1, &y1, zero, mod); // now x1 and y1 have values
	b2 = divTwoLargeNumbers(b, a);
	scratch = multiplyTwoLargeNumbers(b2, x1);
	*y = copyLarge(x1);
	freeLarge(x1);
	x1 = scratch;
	scratch = modTwoLargeNumbers(x1, mod);
	while (!greaterThanLarge(y1, scratch)) {
		addTwoLargeNumbers(y1, mod);
	}
	subTwoLargeNumbers(y1, scratch);
	*x = copyLarge(y1);
	// x = y - (b/a) * x
	freeLarge(b2);
	freeLarge(scratch);
	freeLarge(x1);
	freeLarge(y1);
}
largenumber *modInv(largenumber *e, largenumber *mod) { //Assumes whatever you send it has a mod inv
	largenumber *x = initLargeNumber();
	largenumber *y = initLargeNumber();
	largenumber *z = initLargeNumber();
	eulerExtended(e, mod, &x, &y, z, mod);
	freeLarge(y);
	freeLarge(z);
	return x;
}
/*
int main() {
	//FILE *rand = fopen("/dev/urandom", "r");
	//unsigned int a[4], b[2], m[3];
       	//fread(a, sizeof(unsigned int), 3, rand);	
       	//fread(b, sizeof(unsigned int), 1, rand);	
	largenumber *l1 = initvLargeNumber(1, 4);
	largenumber *l2 = initvLargeNumber(1, 2);
	displayLargeNum(l1);
	displayLargeNum(l2);
	largenumber *l3 = modTwoLargeNumbers(l1, l2);
	printf("RES\n");
	displayLargeNum(l3);
}*/
