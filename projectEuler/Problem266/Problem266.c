#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int *primes;
long *logprimes;
long total = pow(10, 15);
char *path;
char min_switch = 0;

typedef struct node {
	struct node *left;
	struct node *right;
	long max;
	long *factor;
	int level;
} node;


typedef struct print_node {
	long sum;
	long prod;
	struct print_node **inner_nodes; // These are inner nodes don't have edges to them (moving right)
	struct print_node **points; //These are edges to other nodes (moving left)
	int points_size;
	int inner_size;
} print_node;

long genPrimes() {
	int size = 1;
	primes = calloc(1, sizeof(int));
	int count = 2;
	long ret = 2;
	int *holder;
	primes[0] = 2;
	int add = 1;
	for(int count = 3; count < 190; count++) {
		add = 1;
		for(int j = 0; j < size; j++) {
			if (count % primes[j] == 0) {
				add = 0;
				break;
			}
		}
		if(add) {
			size += 1;
			holder = realloc(primes, size*sizeof(int));
			primes = holder;
			primes[size-1] = count;
		}
	}
	for(int i = 0; i < size/2; i++) {
		count = primes[i];
		primes[i] = primes[size-i-1];
		primes[size-i-1] = count;
	}
	return size;
}

void logPrimes(int size) {
	logprimes = calloc(size, sizeof(long));
	for(int i = 0; i < size; i++) {
		logprimes[i] = (long)(log(primes[i])*pow(10, 15));
	}
}

void free_print_node(print_node *p) {
	if(p->inner_size) {
		free(p->inner_nodes);
	}
	if(p->points_size) {
		free(p->points);
	}
	free(p);
}

void traverse(print_node *node, long *prod1, long *prod2, double *p1, double *p2) {
	*prod1 = *prod1 * node->prod % 10000000000000000;
	*p1 = *p1 * node->prod;
	for(int i = 0; i < node->inner_size; i++) {
		traverse(node->inner_nodes[i], prod1, prod2, p1, p2);
	}
	for(int i = 0; i < node->points_size; i++) {
		traverse(node->points[i], prod2, prod1, p2, p1);
	}

	free_print_node(node);
}
// This can be expensive because we do not call it a lot
// A 1 in the path joins the two biggest nodes into one node
// A 0 in the path draws an edge between the two biggest nodes
// then we two-color

void print(int max) {
	print_node** arr = calloc(max, sizeof(print_node *));
	print_node** holder;
	print_node* holder2;
	for(int i = 0; i < max; i++) {
		arr[i] = calloc(1, sizeof(print_node));
		arr[i]->sum = logprimes[i];
		arr[i]->prod = primes[i];
	}
	for(int i =0; i < max-1; i++) {
		if(!path[i]) { //merge two largest nodes
			if(arr[0]->inner_size) {
				holder = realloc(arr[0]->inner_nodes, (arr[0]->inner_size+1)*sizeof(print_node *));
				arr[0]->inner_nodes = holder;
				arr[0]->inner_nodes[arr[0]->inner_size] = arr[1];
				arr[0]->inner_size += 1;
				arr[0]->sum += arr[1]->sum;
			}
			else {
				arr[0]->inner_nodes = calloc(1, sizeof(print_node *));
				arr[0]->inner_nodes[arr[0]->inner_size] = arr[1];
				arr[0]->inner_size += 1;
				arr[0]->sum += arr[1]->sum;
			}
	
		}
		else { //draw edge between two largest nodes
			if(arr[0]->points_size) {
				holder = realloc(arr[0]->points, (arr[0]->points_size+1)*sizeof(print_node *));
				arr[0]->points = holder;
				arr[0]->points[arr[0]->points_size] = arr[1];
				arr[0]->points_size += 1;
				arr[0]->sum -= arr[1]->sum;
			}
			else {
				arr[0]->points = calloc(1, sizeof(print_node *));
				arr[0]->points[arr[0]->points_size] = arr[1];
				arr[0]->points_size += 1;
				arr[0]->sum -= arr[1]->sum;
			}
		}
		// Resort List
		holder2 = arr[0];
		int k = 0;
		while(k < max - i - 2 && holder2->sum < arr[k+2]->sum) {
			arr[k] = arr[k+2];
			k += 1;
		}
		arr[k] = holder2;
		while( k < max - i - 2 ) {
			arr[k+1] = arr[k+2];
			k += 1;
		}
	}
	double p1 = 1;
	double p2 = 1;
	long prod1 = 1;
	long prod2 = 1;
	traverse(arr[0], &prod1, &prod2, &p1, &p2);
	free(arr);
	if(p1 < p2) {
		printf("\r%ld", prod1);
	}
	else {
		printf("\r%ld", prod2);
	}
	fflush(0);
}

void genCandidates(node *root, int max) {
	long diff;
	if(root->level == max-1) {
		diff = root->factor[0] - root->factor[1];
		if( diff < total ) {
			total = diff;
			print(max);
		}
		return;
	}	
	if(root->factor[0] > root->max) {
		if(root->factor[0] - root->max < total) {
			total = root->factor[0] - root->max;
			for(int i = root->level; i < max; i++) {
				path[i] = 1; 
			}	
			print(max);
		}
		return;
	}
	node *left = calloc(1, sizeof(node));
	node *right = calloc(1, sizeof(node));

	left->level = root->level + 1;
	right->level = root->level + 1;

	left->factor = calloc(max - root->level - 1, sizeof(long));
	right->factor = calloc(max - root->level - 1, sizeof(long));

	long leftIns = root->factor[0] - root->factor[1];
	char ins = 1;

	right->factor[0] = root->factor[0] + root->factor[1];
	long rsum = 0;
	long lsum = 0;
	
	for(int k = 2; k < max - root->level; k++) {
		right->factor[k-1] = root->factor[k];
		rsum += right->factor[k-1];
	}
	right->max = rsum;
	int i = 0;
	while(i < max - root->level - 2 && leftIns < root->factor[i+2]) {
		left->factor[i] = root->factor[i+2];
		lsum += left->factor[i];
		i += 1;
	}
	left->factor[i] = leftIns;
	lsum += left->factor[i];
	while( i < max - root->level - 2 ) {
		left->factor[i+1] = root->factor[i+2];
		lsum += left->factor[i+1];
		i += 1;
	}
	left->max = lsum - left->factor[0];
	path[root->level] = 1;
	genCandidates(left, max);
	path[root->level] = 0;
	genCandidates(right, max);

	free(left->factor);
	free(right->factor);
	free(left);
	free(right);

	return;
}
int main() {
	int size = genPrimes();
	logPrimes(size);
	node *root = calloc(1, sizeof(node));
	root->level = 0;
	root->factor = logprimes;
	long max = 0;
	for(int i = 1; i < size; i++) {
		max += root->factor[i];
	}
	root->max = max;
	path = calloc(size, sizeof(char));
	genCandidates(root, size);
	printf("\n");
	free(root);
	free(path);
	free(primes);
	free(logprimes);
}
