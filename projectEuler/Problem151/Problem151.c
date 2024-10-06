#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int sheets[16];

void printSheets(int * sheets) {
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			printf("%i, ", sheets[i*4+j]);
		}
		printf("\n");
	}
	printf("\n\n");
}

void split(int *sheets, int index) {
	int total = 32>>sheets[index];
	int before = sheets[index];
	for(int i = index; i < index+total; i++) {
		sheets[i] = before+1;
	}
	if(sheets[index] != 5)
		split(sheets, index);
	else
		sheets[index] = 0; // sample at end
}


double sample(int *sheets) {
	//printSheets(sheets);
	int *localSheets = calloc(16, sizeof(int));
	memcpy(localSheets, sheets, sizeof(int)*16);
	int change = 0, count = 0, diff=0, skip;
	double result = 0;
	for(int i = 0; i < 16; i++) {
		if(sheets[i] != 0 && sheets[i] != 5)
			diff = 1;
		if(sheets[i]) {
			count += 1;
			i = i - 1 + (32>>localSheets[i]);
		}
	}
	if(count == 1 && diff == 1)
		result += 1;
	else if(diff == 0)
		return 0;
	for(int i = 0; i < 16; i += 1) {
		if(localSheets[i]% 5 != 0) {
			skip = i - 1 + (32>>localSheets[i]);
			split(localSheets, i);
			i = skip;
			change = 1;
			result += sample(localSheets)/count;
		}
		if(change) {
			memcpy(localSheets, sheets, sizeof(int)*16);
			change = 0;
		}
	}
	for(int i = 0; i < 16; i++) {
		if(localSheets[i] == 5) {
			localSheets[i] = 0;
			result += sample(localSheets)/count;
			localSheets[i] = 5;

		}
	}
	free(localSheets);
	return result;

}

int main() {
	for(int i = 0; i < 8; i++) {
		sheets[i] = 2;
	}
	for(int i = 8; i < 12; i++) {
		sheets[i] = 3;
	}
	for(int i = 12; i < 14; i++) {
		sheets[i] = 4;
	}
	for(int i = 14; i < 16; i++) {
		sheets[i] = 5;
	}
	sheets[15] = 0; //simulating the first batch being taken
	
	printf("%lf\n", sample(sheets));
}
