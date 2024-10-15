#ifndef LARGENUMBER_H
#define LARGENUMBER_H
typedef struct large largenumber;
void displayLarge(unsigned int *, int);
void displayLargeNum(largenumber *);
largenumber *initLargeNumber();
largenumber *initSizedLargeNumber(unsigned int);
largenumber *initMemLargeNumber(unsigned int *);
largenumber *initvLargeNumber(int, ...);
void freeLarge(largenumber *);
void resize(largenumber *, long);
void smartResize(largenumber *);
void carryOne(largenumber *, unsigned int);
largenumber *copyLarge(largenumber *);
void addLargeNumber(largenumber *, unsigned long);
unsigned int determineSize(unsigned int);
void addTwoLargeNumbers(largenumber *, largenumber *);
void subTwoLargeNumbers(largenumber *, largenumber *);
void subLargeNumber(largenumber *, unsigned int);
void multiplyLargeNumber(largenumber *, unsigned int);
void zeroLargeNumber(largenumber *, unsigned int);
void shiftLargeNumber(largenumber *, unsigned int);
void shiftDownLargeNumber(largenumber *, unsigned int);
largenumber *multiplyTwoLargeNumbers(largenumber *, largenumber *);
int equalsLarge(largenumber *, largenumber *);
int greaterThanLarge(largenumber *, largenumber *);
void granularShiftDown(largenumber *, unsigned int);
void granularShiftUp(largenumber *, unsigned int);
void setLargeEqual(largenumber *, largenumber *);
largenumber *modTwoLargeNumbers(largenumber *, largenumber *);
largenumber *divTwoLargeNumbers(largenumber *, largenumber *);
largenumber *largeModPow(largenumber *, largenumber *, largenumber *);
void eulerExtended(largenumber *, largenumber *, largenumber **, largenumber **, largenumber *, largenumber *);
largenumber *modInv(largenumber *, largenumber *);
#endif
