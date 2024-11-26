#include <stdlib.h>
#include <stdio.h>

char num[100];
int len = 100;
int sum = 0;

void updateSum(int ln) {
	int new = 0, ten = 1;
	if(ln < 2)
		return;
	if(ln > 5)
		ln = 5;
	for(int i = 0; i < ln; i++) {
		new += num[i]*ten;
		ten *= 10;
	}
	sum += new;
	sum = sum % 100000;
}

int genRR(int n0, int n1) {
	int ind = 0;
	int s = n0;
	int carry = 0;
	while(ind < len) {
		num[ind] = s;
		s = (num[ind] * n1 + carry) % 10;
		carry = (num[ind] * n1 + carry) / 10;
		if(s == n0 && num[ind] && !carry) {
			updateSum(ind+1);
		}
		ind += 1;
	}
}

int main() {
	for(int i = 1; i < 10; i++)
		for(int j = 1; j < 10; j++)
			genRR(i, j);
	printf("sum is %d\n", sum);
}
