#include <stdlib.h>
#include <stdio.h>

int triangleContains(double *points) {
	int base;
	int location = 0;
	float m1, m2, b1, b2;
	for(int i =0; i < 6; i += 2) {
		location += (*(points+i) > 0);
	}
	if(!(location % 3))
		return 0; 
	if(location == 2) { // If 2 points x > 0
		for(int i =0; i < 6; i += 2) {
			if(*(points+i) <= 0) { //point with higher x is base
				base = i;	
			}
		}
	}
	else {
		for(int i =0; i < 6; i += 2) {
			if(*(points+i) > 0) { //point with higher x is base
				base = i;	
			}
		}
	}
	m1 = (*(points+base+1)-*(points+((base+3)%6)))/(*(points+base)-*(points+((base+2)%6)));//m = (y1-y2)/(x1-x2)
	b1 = *(points+base+1) - m1*(*(points+base));// b = y1-m*x
	m2 = (*(points+base+1)-*(points+((base+5)%6)))/(*(points+base)-*(points+((base+4)%6)));//m = (y1-y2)/(x1-x2)
	b2 = *(points+base+1) - m2*(*(points+base));// b = y1-m*x
		
	return ((b1 >= 0 && b2 <= 0) || (b1 <= 0 && b2 >= 0));
}


int main() {
	FILE *f = fopen("triangles.txt", "r");
	double point[6];
	unsigned long sum = 0;
	int a = 0;
	for(int total = 0; total < 10000; total++) {
		for(int i = 0; i < 5; i++)
			fscanf(f, "%lf,", point+i);
		fscanf(f, "%lf\n", &point[5]);
		a = triangleContains(point);
		sum += a;
	}
	printf("%lu\n", sum);
}
