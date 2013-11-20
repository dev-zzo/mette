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
	unsigned int units : 24;
	unsigned int is_free : 1;
	unsigned int reserved : 7;
	void *next_free;
	/* User data area */
};

/* Keeps the free list head */
static struct slot *free_anchor;

/* Remember where we start/end allocating. */
static void *arena_start;
static void *arena_brk;

/* Linker-defined, marks the end of the program's data area. */
extern char end;

/* Verify pointer validity. */
static int xmem_is_valid(struct slot *this)
{
	return this >= arena_start && this < arena_brk;
}

/* Allocates with getting some system memory. */
static void *xmem_grow(size_t units)
{
	/* TODO: write something. */
}

void *xmalloc(size_t size)
{
	struct slot *free_slot, *free_prev;
	size_t units;
	
	/* Make up the total alloc size */
	units = ALIGN(size, 8) / 8 + 1;
	
	/* Don't bother if cannot allocate that much */
	if (units > 0xFFFFFFU) {
		return NULL;
	}
	
	/* Walk the free list, see if anything useful in there. */
	free_prev = NULL;
	free_slot = free_anchor;
	while (free_slot) {
		/* Does it match perfectly? */
		if (free_slot->units == units) {
			/* Remove it from the list. */
			if (free_prev) {
				free_prev->next_free = free_slot->next_free;
			} else {
				free_anchor = free_slot->next_free;
			}
			/* Keep it clean. */
			free_slot->next_free = NULL;
			/* Delivered. */
			return free_slot + 1;
		}
		
		/* Is it bigger, so it can be split? */
		if (free_slot->units >= units + 1 ) {
			struct slot *new_slot;
			/* Fill in the newly formed slot. */
			new_slot = free_slot + units;
			new_slot->units = free_slot->units - units;
			new_slot->next_free = free_slot->next_free;
			/* Update this slot. */
			free_slot->units = units;
			/* Remove it from the list. */
			if (free_prev) {
				free_prev->next_free = new_slot;
			} else {
				free_anchor = new_slot;
			}
			/* Keep it clean. */
			free_slot->next_free = NULL;
			/* Delivered. */
			return free_slot + 1;
		}
		
		/* Advance to the next item. */
		free_prev = free_slot;
		free_slot = free_slot->next_free;
	}
	
	/* Nothing useful was found. Try to allocate some memory from the system. */
	return xmem_grow(units);
}

void *xrealloc(void *ptr, size_t size)
{
	struct slot *this = (struct slot *)ptr - 1;

	return NULL;
}

void xfree(void *ptr)
{
	struct slot *this = (struct slot *)ptr - 1;
	struct slot *prev;
	struct slot *next = this + this->units;
	int merged = 0;
	
	/* NOTE: Since we do a merge each time free() is called, there may be
	 * at most one free slot on each side of this one.
	 */
	 
	/* NOTE: Can be optimized if we assume that free list is sorted. */
	
	/*
	 * Merge upwards.
	 * Walk the free list, see if there is any slot that is near this one.
	 * If so, we can simply eat {this} slot.
	 */
	for (prev = free_anchor; prev; prev = prev->next_free) {
		if (prev + prev->units == this) {
			prev->units += this->units;
			this = prev;
			merged = 1;
			break;
		}
	}
	
	/* Merge downwards. */
	if (xmem_is_valid(next) && next->is_free) {
		this->next_free = next->next_free;
		this->units += next->units;
		if (free_anchor == next) {
			/* Replace the slot completely. */
			free_anchor = this;
		} else {
			/* Fix up the free slot that points to {next}. */
			for (prev = free_anchor; prev->next_free != next; prev = prev->next_free)
				;
			prev->next_free = this;
		}
		merged = 1;
	}
	
	/* Add this to the free list, if have not been done so via merges. */
	if (!merged) {
		this->next_free = free_anchor;
		free_anchor = this;
	}
}





