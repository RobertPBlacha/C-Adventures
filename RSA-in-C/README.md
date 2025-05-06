# RSA Key-Gen In C  
As a programmatic test of strength I wanted to create an RSA key generator in C using nothing but the the standard libraries, string, and pthread libraries. To accomplish this I also implemented a library in C that will handle unsigned integers of arbitrary length. I want to eventually output the values into a PEM format, but for now they are just presented in hex, which was the simpler display method. Below is a description of how to use the library and the key-gen.

## The LargeNumber Library
The LargeNumber library is intended to be a library that supports arbitrary length unsigned integers and various mathematical operations associated with them. I wrote this trying not to reference other materials as much as possible other than c documentation, and some help writing the expanded euler method, so I am aware that certain algorithms can be improved (like implementing karatsuba multiplication).

The data type the LargeNumber Library gives the user is called a large, which is a type defined struct that you can use upon importing LargeNumber.h and LargeNumber.c

### Initialization of Larges
You can initialize Larges in 5 ways:
  * initLargeNumber() will return a Large with value zero, taking up as little space as possible.
  * initSizedLargeNumber(uint size) will return a Large with value zero and number of digits equal to size.
  * initvLargeNumber(...) will return a Large with user defined value, passed little endian into the function as parameters
  * initMemLargeNumber(uint *arr) will take an array of unsigned integers and copy them into a Large struct. This is useful when reading from file streams like /dev/urandom
  * initSizedMemLargeNumber(uint *arr, uint size) will act like initMemLargeNumber() but with user defined number of digits to read from arr.

If you have a large but would like to zero its value with a new size, use the void zeroLargeNumber function, which expects a large pointer and an unsigned integer for its new size.

### Showing Large Values
Larges can be displayed with displayLargeNum(large *l), which will print out the large in hex. All larges will be printed with a leading null 4 bytes, which is meant to show that there will always be more space to add to the large.

### Storing the Values and Memory Efficiency
I store the values of a Large using a continuous array of unsigned integers which can be thought of as the digits of the large. I do this because they are the largest values I can atomically multiply with having their results stored in an unsigned long, which is useful for later methods. I also store the number of digits in each Large, which is useful for indexing the digits. I try to automatically resize Larges to prevent them from taking up memory when they don't have to using the smartResize function. One can call the smartResize function directly if they suspect a Large is taking up too much memory, providing it only the large as input. 

### Larges and Primitive Types
#### Mathematical Operations
Larges can be added or subtracted with longs, and multiplied by unsigned ints. These functions are addLargeNumber(), subLargeNumber(), and multiplyLargeNumber(), which expect a large number pointer as their first argument and the primitive as the second argument.
#### Logical Opearations
Larges can be shifted by unsigned integers with shiftLargeNumber and shiftDownLargeNumber, which shift left and right respectively. They take a large pointer as their first argument and an unsigned integer as their second argument. ShiftDownLargeNumber expects a second argument that is less than or equal to the size of the Large. 

### Large-Large Operations
#### Adding And Subtracting
The adding and subtracting larges from each other can be accomplished using addTwoLargeNumbers, and subTwoLargeNumbers. These functions are in place and add/subtract the second large from the first. If you want to get the results of the addition or subtraction in a separate long, you can use the copyLarge function to create a copy of your first parameter before calling the addition functions.

#### Multiplication, Division, and Modulo
Multiplying, Dividing, and Modulus are supported with multiplyTwoLargeNumbers, divTwoLargeNumbers, and modTwoLargeNumbers, which each expect two arguments. These functions return new Larges with the results. Modulo Inverse is supported using the modInv() function, which takes the modulus as its second argument and the element you are finding the inverse of as the first argument. It assumes that there does exist a modulo inverse, so nonsensical output is possible for cases where an inverse does not exist.

#### Modular Exponentiation
Only Modular Exponentiation is supported by the library, with the function largeModPow which takes arguments the base, the power, and the modolus as arguments in that order, all of which are in the form of large pointers. Similar to multiplication, division and modulus, this returns a new large with the result.

### Char Representation
To cast the values in a large to a char array, you can call the charRep function with the large as its first argument and minimum size of the char array as its second argument. The function will return the data referenced as a char pointer. 

### Freeing Larges
Larges should be freed with the freeLarge function. Freeing larges directly leads to a memory leak of the actual data in the large.
