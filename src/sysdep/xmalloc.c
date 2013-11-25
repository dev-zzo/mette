#include "xmalloc.h"
#include "xstring.h"
#include "syscalls.h"
#include <stdint.h>

/*
 * Many ideas stolen from Minix's implementation:
 * http://www.cise.ufl.edu/~cop4600/cgi-bin/lxr/http/source.cgi/lib/ansi/malloc.c
 */

#define ALIGN(x, a) (((x) + (a) - 1) & ((a) - 1))

#define PAGE_SIZE 4096
#define UNIT_SIZE sizeof(struct slot)

/*
 * We will be allocating with 8 byte granularity here.
 * This means, the max size of user allocation is 524280 bytes.
 * Since the target devices rarely have more than 128M of memory...
 */

struct slot {
	struct slot *next_slot; /* Keeping a pointer here eliminates some ops */
	struct slot *next_free;
	/* User data area */
};

/* Keeps the free list head */
static struct slot *free_anchor;

/* Remember where we start/end allocating. */
static void *arena_start;
static void *arena_brk;

/* Linker-defined, marks the end of the program's data area. */
extern char end;

/* Allocates with getting some system memory. */
static int xmem_grow(size_t units, struct slot *last_free)
{
	struct slot *new_slot;
	void *new_brk;
	
	if (!arena_brk) {
		/* First call -- initialise the pointers */
		arena_brk = arena_start = (void *)ALIGN((uintptr_t)&end, PAGE_SIZE);
		sys_brk(arena_brk);
	}

	new_brk = (void *)sys_brk(arena_brk + ALIGN(units * 8, PAGE_SIZE));
	if (new_brk == arena_brk) {
		/* Seems that brk(2) has failed. Sorry. */
		return 0;
	}
	new_slot = (struct slot *)arena_brk;
	arena_brk = new_brk;
	
	/* Attach the block to the list and return to malloc. */
	if (last_free) {
		last_free->next_free = new_slot;
	} else {
		free_anchor = new_slot;
	}
	return 1;
}

void *xmalloc(size_t size)
{
	struct slot *curr_free, *prev_free;
	size_t units;
	int retry = 1;
	
	/* Make up the total alloc size */
	units = ALIGN(size, UNIT_SIZE) / UNIT_SIZE + 1;
	
	/* Don't bother -- cannot allocate that much */
	if (units > 0xFFFFFFU) {
		return NULL;
	}
	
	/* See if we can allocate anything. */
	for(;;) {
		/* Walk the free list, see if anything useful in there. */
		prev_free = NULL;
		curr_free = free_anchor;
		while (curr_free) {
			struct slot *new_slot = curr_free + units;
			/* Check for an overflow */
			if (new_slot <= curr_free) {
				return NULL;
			}
			/* Too small. */
			if (curr_free->next_slot < new_slot) {
				/* Advance to the next item. */
				prev_free = curr_free;
				curr_free = curr_free->next_free;
				continue;
			}
			/* Is it bigger, so it can be split? */
			if (curr_free->next_slot >= new_slot + 1 ) {
				/* Fill in the newly formed slot. */
				new_slot->next_slot = curr_free->next_slot;
				new_slot->next_free = curr_free->next_free;
				/* Update this slot. */
				curr_free->next_slot = new_slot;
				curr_free->next_free = new_slot;
			}
			/* Remove it from the list. */
			if (prev_free) {
				prev_free->next_free = curr_free->next_free;
			} else {
				free_anchor = curr_free->next_free;
			}
			/* Delivered. */
			curr_free->next_free = NULL;
			return curr_free + 1;
		}
		
		/* We've already tried; didn't help. */
		if (!retry) {
			break;
		}

		/* Nothing useful was found. Try to allocate some memory from the system. */
		if (!xmem_grow(units, prev_free)) {
			break;
		}
	}
	
	return NULL;
}

void *xrealloc(void *ptr, size_t size)
{
	struct slot *this = (struct slot *)ptr - 1;
	size_t old_count;
	struct slot *new;
	struct slot *prev_free;
	struct slot *next_free;
	size_t units;
	
	if (!ptr) {
		return xmalloc(size);
	}
	if (!size) {
		xfree(ptr);
		return NULL;
	}
	
	/* Keep the old slot size, will be used when copying data over. */
	old_count = (this->next_slot - this) * UNIT_SIZE;
	
	/* Make up the total alloc size */
	units = ALIGN(size, UNIT_SIZE) / UNIT_SIZE + 1;
	
	/* Don't bother -- cannot allocate that much */
	if (units > 0xFFFFFFU) {
		return NULL;
	}
	
	new = this + units;
	/* Check for an overflow. */
	if (new <= this) {
		return NULL;
	}
	
	/* See if there is any free slot behind this. */
	prev_free = NULL;
	next_free = free_anchor;
	while (next_free && next_free < this) {
		if (this->next_slot == next_free) {
			/* There is a free slot behind this one. Merge them. */
			this->next_slot = next_free->next_slot;
			if (prev_free) {
				prev_free->next_free = next_free->next_free;
			} else {
				free_anchor = next_free->next_free;
			}
		}
		prev_free = next_free;
		next_free = next_free->next_free;
	}
	
	/* See if the (potentially merged) slot can be used. */
	if (new <= this->next_slot) {
		if (new + 1 <= this->next_slot) {
			/* Split. */
			new->next_slot = this->next_slot;
			this->next_slot = new;
			/* Since both are allocated memory, we can free() the leftover. */
			/* TODO: See if it makes more sense to make this slightly faster... */
			xfree(new);
		}
		return ptr;
	}
	
	/* No. Allocate new one. */
	new = (struct slot *)xmalloc(size);
	if (!new) {
		return NULL;
	}
	
	xmemcpy(new + 1, ptr, old_count);
	xfree(ptr);
	return new + 1;
}

void xfree(void *ptr)
{
	struct slot *this = (struct slot *)ptr - 1;
	struct slot *prev_free;
	struct slot *next_free;
	
	if (!ptr)
		return;
	
	prev_free = NULL;
	next_free = free_anchor;
	while (next_free < this) {
		prev_free = next_free;
		next_free = next_free->next_free;
	}
	
	if (prev_free) {
		prev_free->next_free = this;
		if (prev_free->next_slot == this) {
			/* Merge upwards. */
			prev_free->next_slot = this->next_slot;
			this = prev_free;
		}
	} else {
		free_anchor = this;
	}
	
	this->next_free = next_free;
	if (next_free) {
		if (this->next_slot == next_free) {
			/* Merge downwards. */
			this->next_slot = next_free->next_slot;
			this->next_free = next_free->next_free;
		}
	}
}

