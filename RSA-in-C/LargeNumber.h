#ifndef LARGENUMBER_H
#define LARGENUMBER_H
typedef struct large large;
void displayLarge(unsigned int *, int);
void displayLargeNum(large *);
large *initLargeNumber();
large *initSizedLargeNumber(unsigned int);
large *initMemLargeNumber(unsigned int *);
large *initvLargeNumber(int, ...);
void freeLarge(large *);
void resize(large *, long);
void smartResize(large *);
void carryOne(large *, unsigned int);
large *copyLarge(large *);
void addLargeNumber(large *, unsigned long);
unsigned int determineSize(unsigned int);
void addTwoLargeNumbers(large *, large *);
void subTwoLargeNumbers(large *, large *);
void subLargeNumber(large *, unsigned int);
void multiplyLargeNumber(large *, unsigned int);
void zeroLargeNumber(large *, unsigned int);
void shiftLargeNumber(large *, unsigned int);
void shiftDownLargeNumber(large *, unsigned int);
large *multiplyTwoLargeNumbers(large *, large *);
int equalsLarge(large *, large *);
int greaterThanLarge(large *, large *);
void granularShiftDown(large *, unsigned int);
void granularShiftUp(large *, unsigned int);
void setLargeEqual(large *, large *);
large *modTwoLargeNumbers(large *, large *);
large *divTwoLargeNumbers(large *, large *);
large *largeModPow(large *, large *, large *);
void eulerExtended(large *, large *, large **, large **, large *, large *);
char *charRep(large *, int);
large *initSizedMemLargeNumber(unsigned int *, unsigned int);
unsigned long byteSize(large *);
large *modInv(large *, large *);
#endif
