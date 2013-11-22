#include "syscalls.h"

/*
 * Many ideas stolen from Minix's implementation:
 * http://www.cise.ufl.edu/~cop4600/cgi-bin/lxr/http/source.cgi/lib/ansi/malloc.c
 */

#define ALIGN(x, a) (((x) + (a) - 1) & ((a) - 1))

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
static void *xmem_grow(size_t units, struct slot *last_free)
{
	/* TODO: write something. */
}

void *xmalloc(size_t size)
{
	struct slot *curr_free, *prev_free;
	size_t units;
	
	/* Make up the total alloc size */
	units = ALIGN(size, 8) / 8 + 1;
	
	/* Don't bother -- cannot allocate that much */
	if (units > 0xFFFFFFU) {
		return NULL;
	}
	
	/* Walk the free list, see if anything useful in there. */
	prev_free = NULL;
	curr_free = free_anchor;
	while (curr_free) {
		struct slot *new_slot = curr_free + units;
		/* Check for an overflow */
		if (new_slot <= curr_free)
			return NULL;
		/* Too small. */
		if (curr_free->next_slot < new_slot) {
			/* Advance to the next item. */
			free_prev = free_slot;
			free_slot = free_slot->next_free;
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
	
	/* Nothing useful was found. Try to allocate some memory from the system. */
	return xmem_grow(units, prev_free);
}

void *xrealloc(void *ptr, size_t size)
{
	struct slot *this = (struct slot *)ptr - 1;
	
	if (!ptr)
		return xmalloc(size);
	if (!size) {
		xfree(ptr);
		return NULL;
	}
	
	/* TODO: write this up. */

	return NULL;
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
		/* No previous -- the first in the list. */
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





