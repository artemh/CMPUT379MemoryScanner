/*
 * File:   memchunk.h
 * Author: Artem Herasymchuk (herasymc)
 *
 *
 * Part of: CMPUT 379 Assignment 01
 *
 */

#ifndef _MEMCHUNK_H_
#define _MEMCHUNK_H_

struct memchunk {
	void *start;
	unsigned long length;
	int RW;
};

struct info {
	int read;
	int write;
	int chunks;
	unsigned long length;
	char *p;
};

int get_mem_layout (struct memchunk *chunk_list, int size);

static void signal_handler (int signum);

static void new(struct memchunk *chunk_list, struct info i);

static void end(struct memchunk *chunk_list, struct info i);

#endif
