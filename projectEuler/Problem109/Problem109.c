#include <stdlib.h>
#include <stdio.h>
#define MAXSCORE 100

int scores[21] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,25};

int permuteSums(int total, int nums, int last) {
	int sums = 0;
	int totalGoal = 0;
	if(nums == 3) { //Last must be a double
		for(int i =0; i < 21; i++) {//Check doubles = total
			if(scores[i]*2 > total) 
				break;
			if(scores[i]*2 == total) {
				sums += 1;
				break;
			}
			else {
				totalGoal = total-scores[i]*2;
				sums += permuteSums(totalGoal, nums-1, last);
			}
		}
	}	
	else { //Now any summing can be computed
	      	if (last >= 3) 
		for(int i = 0; i < 20; i++) { //triples
			if(scores[i]*3 > total) 
				break;
			if(scores[i]*3 == total) {
				sums += 1;
				break;
			}
			else if(nums-1){ //prevents double counting
				totalGoal = total-scores[i]*3;
				last = 3;
				if(scores[i]*6 > total)
					last -=1;
				sums += permuteSums(totalGoal, nums-1,last);
			}
		}
	      	if (last >= 2) 
		for(int i = 0; i < 21; i++) { //doubles
			if(scores[i]*2 > total) 
				break;
			if(scores[i]*2 == total) {
				sums += 1;
				break;
			}
			else if (nums-1){
				totalGoal = total-scores[i]*2;
				last = 2;
				if(scores[i]*4 > total)
					last -=1;
				sums += permuteSums(totalGoal, nums-1,last);
			}
		}
		for(int i = 0; i < 21; i++) { //singles
			if(scores[i] == total) {
				sums += 1;
				break;
			}
			else if(nums-1 && scores[i]*2 <= total){
				totalGoal = total-scores[i];
				sums += permuteSums(totalGoal, nums-1,1);
			}
		}
	}

	return sums;
}

int main() {
	int sum = 0;
	for(int i = 1; i < 100; i++) {
		sum += permuteSums(i, 3, 5);
	}
	printf("%i\n", sum);
}
