#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "LargeNumber.h"

struct large {
	unsigned long size; // in terms of 4 byte chunks
	unsigned int *num;
};

typedef struct large large;

void displayLarge(unsigned int *num, int size) {
	if(size) { 
		displayLarge(num+1, size-1);
		printf("%08x ", *num);
	}
	else 
		printf("%08x ", 0);
}

void displayLargeNum(large *large) {
	displayLarge(large->num, large->size);
	printf("\n");
}

large *initLargeNumber() {
	large *input = calloc(1, sizeof(large));
	input->size = 1; //smallest it should ever be
	input->num = calloc(input->size, sizeof(unsigned int));
	return input;
}
large *initSizedLargeNumber(unsigned int size) {
	large *input = calloc(1, sizeof(large));
	input->size = size;
	input->num = calloc(size, sizeof(unsigned int));
	return input;
}
large *initMemLargeNumber(unsigned int *arr) {
	unsigned int i = 0;
	while(*(arr+i)) //the chance that a random number will contain an all zero unsigned int is 2**-32
		i += 1;
	large *l = calloc(1, sizeof(large));
	l->num = calloc(i , sizeof(unsigned int));
	memcpy(l->num, arr, i*sizeof(unsigned int));
	l->size = i;
	return l;
}
large *initSizedMemLargeNumber(unsigned int *arr, unsigned int size) {
	large *l = calloc(1, sizeof(large));
	l->num = calloc(size , sizeof(unsigned int));
	memcpy(l->num, arr, size*sizeof(unsigned int));
	l->size = size;
	return l;
}
large *initvLargeNumber(int arg_count, ...) {
	va_list ap;
	va_start(ap, arg_count);
	unsigned int *values = calloc(arg_count+2, sizeof(unsigned int));
	for(int i = 0; i < arg_count; i++) { 
		*(values+i) = va_arg(ap, unsigned int);
	}
	va_end(ap);
	large *init = calloc(1, sizeof(large));
	init->size = arg_count + 2;
	init->num = values;	
	return init;
}

void freeLarge(large *l) {
	free(l->num);
	free(l);
}

void resize(large *input, long add) { //Can both scale up and down
	input->num = realloc(input->num, sizeof(unsigned int) * (input->size + add));
	unsigned int old = input->size;
	input->size += add;
	for(int i = old; i < old+add; i++)
		*(input->num+i) = 0;
}

void smartResize(large *input) {
	for(int i = input->size-1; i > 0; i--) {
		if(*(input->num+i)) {
			resize(input, i-input->size+1);
			break;	
		}
	}
}
void carryOne(large *input, unsigned int chunk) {//other functions should call resize before calling this one so there WILL be space
	unsigned int before = *(input->num+chunk);
	*(input->num+chunk) = before + 1;
	if(before > *(input->num+chunk)) 
		carryOne(input, chunk+1);
}

void addLargeNumber(large *input, unsigned long add) {
	if (*(unsigned long *)(input->num+input->size-2)) { //check last two byte to be sure, 
		resize(input, 2); //No more than two needed
	}
	unsigned long least8 = *(unsigned long *)(input->num);
	unsigned long newLeast8 = least8 + add;
	if(newLeast8 < least8) //handle overflows
		carryOne(input, 2);
	*(unsigned long *)(input->num) = newLeast8;
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

void addTwoLargeNumbers(large *addend, large *addto) {
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
void subLargeNumber(large *addend, unsigned int sub) {
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
void subTwoLargeNumbers(large *addend, large *addto) { // Just assumes addend > addto
	unsigned int cursor = 0;
	smartResize(addend);
	smartResize(addto);
	for(unsigned int i = 0; i < addto->size && i < addend->size; i++) {
		cursor = *(addend->num+i) - *(addto->num+i);
		if(cursor > *(addend->num+i)) {
			*(addend->num+i+1) -= 1;
		}
		*(addend->num+i) = cursor;
	}
}
void multiplyLargeNumber(large *input, unsigned int mult) {
	resize(input, determineSize(mult));
	unsigned long multiplicationCursor = 0;
	unsigned int new = 0;
	unsigned int carry = 0; //The product of two numbers <2^32 will be bounded by 2^64, but adding the carry to the next number could overflow, need to fix
	for(int i = 0; *(input->num + i) || carry; i++) {
		multiplicationCursor = (unsigned long)*(input->num+i) * (unsigned long)mult;
		*(input->num+i) = (unsigned int)multiplicationCursor + carry;
		carry = (unsigned int)(multiplicationCursor >> 32);
		if(*(input->num+i) < (unsigned int)multiplicationCursor)
			carry += 1; //Carry will be at most fffffffe, so adding one has 0 chance of overflow
	}
}

void zeroLargeNumber(large *input, unsigned int size) { 
	free(input->num);
	input->num = calloc(size, sizeof(unsigned int));
	if(!input->num) {
		printf("Memerror, exiting\n");
		exit(1);
	}
	input->size = size;
}

void shiftLargeNumber(large *input, unsigned int amount) {
	resize(input, amount);
	for(unsigned int i = input->size - 1; i-amount != -1; i--) {
		*(input->num+i) = *(input->num+i - amount);
	}
	for(unsigned int i = amount-1; i != -1; i--) {
		*(input->num+i) = 0;
	}
}
void shiftDownLargeNumber(large *input, unsigned int amount) {
	for(unsigned int i = 0; i+amount != input->size; i++) {
		*(input->num+i) = *(input->num+i + amount);
	}
	for(unsigned int i = amount; i != input->size; i++) {
		*(input->num+i) = 0;
	}
	resize(input, -1 * amount);
}
large *multiplyTwoLargeNumbers(large *base, large *factor) {
	large *l = initSizedLargeNumber(base->size+factor->size); //Theoretically could cause overflow but unlikely for the sizes I'm going to use this library for
	large *res = initSizedLargeNumber(base->size + factor->size);
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
int equalsLarge(large *l, large *l2) {
	if(l->size < l2->size)
		return equalsLarge(l2, l);
	for(int i = 0; i < l2->size; i++) {
		if(*(l->num+i) != *(l2->num+i)) {
			return 0;
		}
	}
	return l->size == l2->size || (l->size > l2->size && !*(l->num+l2->size));
}
int greaterThanLarge(large *l, large *l2) {
	smartResize(l);
	smartResize(l2);
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
void granularShiftDown(large *l, unsigned int i) { //shifts by chars
	char *cast = (char *)(l->num);
	unsigned int total = sizeof(unsigned int) * (l->size);
	char * a = malloc(total);
	memcpy(a, cast, total); //This memory passing is necessary because memcpy is undefined for overlapping segments (in this case it copied 4 bytes and then stopped)
	memcpy(cast, a+i, total - i);
	free(a);
	*(cast+total-1)=0;
	smartResize(l);
}
void granularShiftUp(large *l, unsigned int i) { //shifts by chars
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
void setLargeEqual(large *dest, large *src) {
	free(dest->num);
	dest->num = calloc(src->size, sizeof(unsigned int));
	memcpy(dest->num, src->num, sizeof(unsigned int) * src->size);
	dest->size = src->size;
}
large *modTwoLargeNumbers(large *l, large *mod) {
	large *c = copyLarge(l); 
	large *modC = copyLarge(mod);
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
large *divTwoLargeNumbers(large *l, large *mod) {
        large *c = copyLarge(l);
        large *modC = copyLarge(mod);
	large *ret = initLargeNumber();
	large *zero = initLargeNumber();
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
	freeLarge(zero);
        return ret;
}

large *largeModPow(large *l, large *pow, large *mod) {	
	large *res = initvLargeNumber(2, 0x1, 0);
	large *powC = copyLarge(pow);
	large *lC = copyLarge(l);
	large *zero = initLargeNumber();
	large *work;
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
void eulerExtended(large *a, large *b, large **x, large **y, large *zero, large *mod) { //zero used for checks
	if(equalsLarge(zero, a)) {
		*x = initvLargeNumber(1, 0);
		*y = initvLargeNumber(1, 1);
		return;
	}
	large *x1, *y1;
	large *b2 = modTwoLargeNumbers(b, a);
	large *scratch;
	eulerExtended(b2, a, &x1, &y1, zero, mod); // now x1 and y1 have values
	freeLarge(b2);
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
unsigned long byteSize(large *n) {
	smartResize(n); //guarantees n has value /kinda/
	int mask = 0x80000000;
	int bytes = 4;
	while(1) {
		if(*(n->num+n->size-1) & mask) {
			return n->size*sizeof(unsigned int)+bytes;
		}
		mask >>= 1;
		if(0xff << ((bytes-1)*8) & mask)
		       bytes -= 1;	
	}
}
char *charRep(large *a, int blockSize) {
	if(a->size < 1+blockSize/4) {
		resize(a, 1+blockSize/4 - a->size);
	}
	return (char *)a->num;
}
large *modInv(large *e, large *mod) { //Assumes whatever you send it has a mod inv
	large *x = initLargeNumber();
	large *y = initLargeNumber();
	large *z = initLargeNumber();
	eulerExtended(e, mod, &x, &y, z, mod);
	freeLarge(y);
	freeLarge(z);
	return x;
}
large *copyLarge(large *copy) {
	large *res = calloc(1, sizeof(large));
	res->num = calloc(copy->size, sizeof(unsigned int));
	memcpy(res->num, copy->num, copy->size * sizeof(unsigned int));
	res->size = copy->size;
	return res;
}
