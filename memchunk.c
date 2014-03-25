/*
 * File:   memchunk.c
 * Author: Artem Herasymchuk (herasymc)
 *
 *
 * Part of: CMPUT 379 Assignment 01
 *
 * Portions of this file taken from CMPUT 379 material/examples
 * (used with permission):
 *
 * Copyright (c) 2008 Bob Beck <beck@obtuse.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <signal.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "memchunk.h"

static sigjmp_buf jumpbuf;
static struct sigaction sa, oldsa;

static void signal_handler (int signum) {
	/*
	 * signal handler replacement for SIGSEGV
	 * doesn't need to do anything but jump
	 */
	siglongjmp(jumpbuf, 1);
}

static void new(struct memchunk *chunk_list, struct info i) {
	/* creates a new chunk in *chunk_list */
	chunk_list[i.chunks].start = (void*)(i.p);
	chunk_list[i.chunks].RW = i.write;
}

static void end(struct memchunk *chunk_list, struct info i) {
	/* ends a chunk in *chunk_list */
	chunk_list[i.chunks].length = (i.length);
}

int get_mem_layout (struct memchunk *chunk_list, int size) {
	int n;
	int cur_chunk = 0;
	const int pagesize = getpagesize();
	char val;
	char *cur_start = 0;
	struct info i;
	i.read = 0;
	i.write = 0;
	i.chunks = 0;

	/* save old SIGSEGV handler and replace with new one */
	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGSEGV, NULL, &oldsa) == -1)
		err(1, "can't save old sigaction");
	if (sigaction(SIGSEGV, &sa, NULL) == -1)
		err(1, "can't do new sigaction");

	/* begin looping through 32-bit memory space */
	for (n = 0; n < (0xFFFFFFFF / pagesize); ++n) {
		if (!(sigsetjmp(jumpbuf, 1))) {
			i.p = (char *)(n * pagesize);
			if (i.read && !cur_chunk) {
				/* new chunk found */
				cur_chunk = 1;
				cur_start = i.p;
				if (i.chunks < size) {
					new(chunk_list, i);
				}
			}
			if (cur_chunk && !i.read) {
				/* chunk ended, no chunk after */
				cur_chunk = 0;
				i.length = (i.p - cur_start);
				if (i.chunks < size) {
					end(chunk_list, i);
				}
				i.chunks++;
			}
			if (cur_chunk && i.read && chunk_list[i.chunks].RW &&
			    !i.write) {
				/* new RO chunk right after RW chunk */
				i.length = (i.p - cur_start);
				if (i.chunks < size) {
					end(chunk_list, i);
				}
				i.chunks++;
				cur_start = i.p;
				if (i.chunks < size) {
					new(chunk_list, i);
				}
			}
			if (cur_chunk && i.write && !chunk_list[i.chunks].RW) {
				/* new RW chunk right after RO chunk */
				i.length = (i.p - cur_start);
				if (i.chunks < size) {
					end(chunk_list, i);
				}
				i.chunks++;
				cur_start = i.p;
				if (i.chunks < size) {
					new(chunk_list, i);
				}
			}
			i.read = 0;
			i.write = 0;
			val = *(i.p);
			i.read = 1;
			*(i.p) = val;
			i.write = 1;
		}
	}
	return i.chunks;
}
