#ifndef LARGENUMBER_H
#define LARGENUMBER_H
typedef struct large largenumber;
largenumber *modTwoLargeNumbers(largenumber *, largenumber *);
void granularShiftDown(largenumber *, unsigned int);
void displayLarge(unsigned int *, int);
void displayLargeNum(largenumber *);
largenumber *initLargeNumber();
largenumber *initSizedLargeNumber(unsigned int);
largenumber *initvLargeNumber(int, ...);
void freeLarge(largenumber *);
void resize(largenumber *, int); //Can both scale up and down
void smartResize(largenumber *);
void carryOne(largenumber *, unsigned int); //other functions should call resize before calling this one so there WILL be space
largenumber *copyLarge(largenumber *);
void addLargeNumber(largenumber *, unsigned long);
unsigned int determineSize(unsigned int);
void addTwoLargeNumbers(largenumber *, largenumber *);
void subTwoLargeNumbers(largenumber *, largenumber *); // Just assumes addend > addto
void multiplyLargeNumber(largenumber *, unsigned int);
void zeroLargeNumber(largenumber *, unsigned int);
void shiftLargeNumber(largenumber *, unsigned int);
void shiftDownLargeNumber(largenumber *, unsigned int);
largenumber *multiplyTwoLargeNumbers(largenumber *, largenumber *);
int equalsLarge(largenumber *, largenumber *);
int greaterThanLarge(largenumber *, largenumber *);
largenumber *modTwoLargeNumbers(largenumber *, largenumber *);
#endif 
