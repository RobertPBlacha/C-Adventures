#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MATSIZE 80
unsigned long average = 0; //For predicting path from a certain point, assume every step is the max possible

typedef struct node {
	unsigned int value;
	unsigned int position;
	unsigned long cost;
	struct node* parent;
	struct node* next;
} node;

unsigned int predict(unsigned int pos) {
	unsigned int cursorX = pos % MATSIZE, cursorY = pos / MATSIZE;
	return ((MATSIZE-1)-cursorX + (MATSIZE-1)-cursorY) * average; // number of steps left times average
}

int inside(unsigned int position, node *list) {
	if(!list->value) {
		return 0;
	}
	if(list->position == position) {
		return 1;
	}
	return inside(position, list->next);
}

node *addToList(unsigned int position, unsigned int *matrix, node *list, node* parent) {
	node * insert;
	if(!list->value) { //first or last node in linked list
		if(parent) {
			list->value = matrix[position] + parent->cost + predict(position);
			list->cost = matrix[position] + parent->cost;
		}
		else {
			list->value = matrix[position] + predict(position);
			list->cost = matrix[position];
		}
		list->position = position;
		list->next = calloc(1, sizeof(node));
		list->parent = parent;
		return list;
	}
	if(list->position == position) { //node already in linkedList
		return NULL;
	}
	if(list->value > matrix[position]+parent->cost+predict(position)) { //insert in middle of linkedList
		insert = calloc(1, sizeof(node));
		memcpy(insert, list, sizeof(node));
		list->value = matrix[position] + parent->cost + predict(position);
		list->position = position;
		list->next = insert;
		list->parent = parent;
		list->cost = matrix[position] + parent->cost;
		return list;
	}
	addToList(position, matrix, list->next, parent);
}


unsigned long aStarSearch(unsigned int *matrix) {
	unsigned int cursorX = 0, cursorY = 0;
	unsigned long Path = 0;
	node *open = calloc(1, sizeof(node));
	node *closed = calloc(1, sizeof(node));
	node *intermediary = NULL; // Just used as like a scratch space
	addToList(0, matrix, open, NULL);
	unsigned int pos;
	while(open->value) {
		cursorX = open->position % MATSIZE;
		cursorY = open->position / MATSIZE;
		if(cursorX == MATSIZE-1 && cursorY == MATSIZE-1) {//solution found
			addToList(cursorX + cursorY*MATSIZE, matrix, open, open);
			Path = open->cost;
			while(intermediary=open) {
				open = open->next;
				free(intermediary);
			}
			while(intermediary=closed) {
				closed=closed->next;
				free(intermediary);
			}
			return Path;
		}
		intermediary = open;
		open = open->next;
		intermediary->next = closed;
		closed = intermediary;
		pos = cursorX+1 + MATSIZE * cursorY;	
		if(!inside(pos, closed) && cursorX < MATSIZE-1)
			addToList(pos, matrix, open, intermediary);
		pos = cursorX + MATSIZE * (cursorY+1);
		if(!inside(pos, closed) && cursorY < MATSIZE-1) 
			addToList(pos, matrix, open, intermediary);
		pos = cursorX + MATSIZE * (cursorY-1);
		if(!inside(pos, closed) && cursorY > 0) 
			addToList(pos, matrix, open, intermediary);
		pos = cursorX-1 + MATSIZE * (cursorY);
		if(!inside(pos, closed) && cursorX > 0) 
			addToList(pos, matrix, open, intermediary);
	}
}

unsigned int *initArray() {
	FILE *mat = fopen("matrix.txt", "r");
	unsigned int *matrix = malloc(sizeof(unsigned int) * MATSIZE * MATSIZE);
	for(unsigned int i = 0; i < MATSIZE*MATSIZE; i++) {
		fscanf(mat, "%u, ", &matrix[i]);
		if(matrix[i] < average || !i) {
			average = matrix[i];
		}
	}
	return matrix;
}

int main() {
	unsigned int* matrix = initArray();
	printf("%lu\n", aStarSearch(matrix));
	free(matrix);
	exit(0);
}
