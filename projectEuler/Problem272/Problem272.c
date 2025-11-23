#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define THREAD_COUNT 20

const unsigned long segment_size = 256*1024;
const unsigned long max = 100000000000;
unsigned long tdivprimes = 0;
unsigned long *aprimes;

unsigned long *iprimes[THREAD_COUNT + 1]; 
unsigned long size_table[THREAD_COUNT + 1]; 

void *segment(void *input) {
	char *c = malloc(segment_size* sizeof(char));
	unsigned long size2 = 0;
	unsigned long total2 = 20000;
	unsigned long *interesting = malloc(total2 * sizeof(unsigned long));
	unsigned long start = (unsigned long)input;
	unsigned long tr; 
	unsigned long ind;
	for(int j = 0; !start && j < tdivprimes; j++) {
		if((aprimes[j]-1)%3==0){
			interesting[size2] = aprimes[j];
			size2 += 1;
		}
	}
	while(start <= max/(segment_size)) {
		tr = start * segment_size;
		start += THREAD_COUNT;
		if(tr * 9 * 7 * 13 * 19 > max)
			break;
		memset(c, 0, segment_size);
		for(int j = 0; j < tdivprimes; j++) {
			ind = aprimes[j] - tr % aprimes[j];
			if(ind == aprimes[j])
				ind = 0;
			for(unsigned long k = ind; k < segment_size; k += aprimes[j]) {
				c[k] = 1;
			}
		}
		for(int j = 0; j < segment_size && tr + j < max; j++) {
			if(tr+j<2lu)
				continue;
			if(!c[j]) {
				if((tr+j-1)%3==0){
					interesting[size2] = tr+j;
					size2 += 1;
				}
				if(size2 == total2) {
					total2 *= 2;
					interesting = realloc(interesting, total2*sizeof(unsigned long));
				}
			}
		}
	}
	interesting = realloc(interesting, size2*sizeof(unsigned long));
	start = (unsigned long)input;
	size_table[start] = size2;
	free(c);
	return interesting;
}

void *find(void *i) {
	unsigned long start = (unsigned long)i;
	char *c = malloc(segment_size* sizeof(char));
	unsigned long ans = 0;
	unsigned long ind, tr;
	
	while(start <= max/segment_size) {
		memset(c, 1, segment_size);
		tr = start * segment_size;
		start += THREAD_COUNT;
		for(int j = 0; j < THREAD_COUNT + 1; j++) {
			for(int k = 0; k < size_table[j] && iprimes[j][k] < (tr + segment_size); k++) {
				ind = iprimes[j][k] - tr % iprimes[j][k];
				if(ind == iprimes[j][k])
					ind = 0;
				for(unsigned long l = ind; l < segment_size; l += iprimes[j][k]) {
					c[l] <<= 1;
				}
			}
		}
		for(int j = 0; j < segment_size; j++) {
			if(c[j] & 0x20 && tr + j < max) {
				ans += tr + j;
			}
		}
	}
	free(c);
	return (void *)(ans);
}

void genPrimes() {
	unsigned long t = 10000;
	aprimes = malloc(t * sizeof(unsigned long));
	unsigned long s = 1;
	int add;
	aprimes[0] = 2;
	// generate first segment slowly
	for(unsigned long i = 3; i*i < max;i+=2){
		add = 1;
		for(int j = 0; j < s; j++) {
			if(i % aprimes[j] == 0) {
				add = 0;
				break;
			}
		}
		if(add) {
			s += 1;
			if(s == t) {
				t *= 2;
				aprimes = realloc(aprimes, t*sizeof(unsigned long));
			}
			aprimes[s-1] = i;
		}
	}
	aprimes = realloc(aprimes, s*sizeof(unsigned long));
	tdivprimes = s;

	pthread_t threads[THREAD_COUNT];

	// for every segment
	for(long i = 0; i < THREAD_COUNT; i++) {
		pthread_create(&threads[i], NULL, segment, (void *)i);
	}
	for(int i = 0; i < THREAD_COUNT; i++) {
		pthread_join(threads[i], (void *)&iprimes[i]);
	}

	free(aprimes);
	
	unsigned long ans[THREAD_COUNT]; 
	iprimes[THREAD_COUNT] = calloc(1, sizeof(unsigned long));
	unsigned long q = 3;
	size_table[THREAD_COUNT] = 1;
	iprimes[THREAD_COUNT][0] = 9;

	for(long i = 0; i < THREAD_COUNT; i++) {
		pthread_create(&threads[i], NULL, find, (void *)i);
	}
	
	for(int i = 0; i < THREAD_COUNT; i++) {
		pthread_join(threads[i], (void *)&ans[i]);
	}
	unsigned long total = 0;

	for(int i = 0; i < THREAD_COUNT; i++) {
		total += ans[i];
	}
	printf("ANSWER: %lu\n", total);
}

unsigned int main() {
	genPrimes();
}
