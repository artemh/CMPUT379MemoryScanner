/*
 * File:   memchunk_test.c
 * Author: Artem Herasymchuk (herasymc)
 *
 *
 * Part of: CMPUT 379 Assignment 01
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "memchunk.h"

int main () {
	/*
	 * simple test of get_mem_layout() from memchunk.c
	 * that maps all chunks and prints their info
	 */
	struct memchunk *chunk_list;
	int i, chunks;
	chunk_list = malloc(sizeof(struct memchunk) * 100);
	chunks = get_mem_layout(chunk_list, 100);
	for (i = 0; ((i < chunks) && (i < 100)); ++i) {
		printf("Address: 0x%08x   Length: %7ld   RW: %d \n",
		    (int)chunk_list[i].start, chunk_list[i].length,
		    chunk_list[i].RW);
	}
	free(chunk_list);
	return 0;
}
