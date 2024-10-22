#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//IMPORTANT: Before running this I ran `cat network.txt | sed 's/-/-1/g' > new.txt && mv new.txt network.txt` to give all values an integer representation
int arr[40][40]; // Half of this will be not used (bc adjacency matrix is symmetric for undirected graph) but whatever
int newArr[40][40];
int *blockedBase;
int *blockedConn;
void removeRedundancies(int base, int conn) {
	for(int i = 0; i < 40; i++) {
		if(newArr[base][i] == -1 && newArr[conn][i] != -1) {
			arr[base][i] = -1;
			arr[i][base] = -1;
		}
		if(newArr[conn][i] == -1 && newArr[base][i] != -1) {
			arr[conn][i] = -1;
			arr[i][conn] = -1;
		}
	}
}

void recurse(int base, int conn) {
	removeRedundancies(base, conn);
	blockedBase[base] = 1;
	blockedConn[conn] = 1;
	for(int i  = 0; i < 40; i++) {
		if(newArr[conn][i] != -1 && !blockedConn[i]) { //Check for all things connected to conn
			recurse(base, i);
		}		
	}
	for(int i = 0; i < 40; i++) {
		if(newArr[base][i] != -1 && !blockedBase[i]) {
			recurse(i, conn);
		}
	}
}

int main() {
	FILE *f = fopen("network.txt", "r");	
	int originalWeight = 0;
	for(int i = 0; i < 40; i++) {
		for(int j = 0; j < 39; j++) {
			fscanf(f, "%i,", &arr[i][j]);
			newArr[i][j] = -1;
		}
		fscanf(f, "%i\n", &arr[i][39]);
		newArr[i][39] = -1;
	}
	for(int i = 0; i < 40; i++)
		for(int j = 0; j < i; j++) 
			if(arr[i][j] != -1)
				originalWeight += arr[i][j];
	fclose(f);
	int weight = 0;
	int minVal;
	int minI = 0, minJ = 0;	
	// Find the smallest value connection
	int edges = 0;
	blockedBase = calloc(40, sizeof(int));
	blockedConn = calloc(40, sizeof(int));
	while(edges < 39) {
		minVal = 1000;
		//Finds minimum val and respective coords
		for(int i = 0; i < 40; i++) {
			for(int j = 0; j < i; j++) {
				if(arr[i][j] != -1 && arr[i][j] < minVal) {
					minI = i;
					minJ = j;
					minVal = arr[i][j];
				}
			}
		}
		// Draw the connection, and mark all other redundant connections as invalid

		arr[minI][minJ] = -1;
		newArr[minI][minJ] = minVal;
		newArr[minJ][minI] = minVal;
		recurse(minI, minJ);
		weight += minVal;
		for(int i = 0; i < 40; i++) {
			blockedBase[i] = 0;
			blockedConn[i] = 0;
		}
		edges += 1;
	}
	free(blockedBase);
	free(blockedConn);
	printf("%i - %i = %i\n", originalWeight, weight, originalWeight - weight);

}
