#include "xstring.h"
#include "syscalls.h"
#include <stdint.h>

#define DEBUG_PRINTS
#include "rtl_debug.h"

/*
 * Many ideas stolen from Minix's implementation:
 * http://www.cise.ufl.edu/~cop4600/cgi-bin/lxr/http/source.cgi/lib/ansi/malloc.c
 */

#define ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))

#define PAGE_SIZE 4096U
#define UNIT_SIZE sizeof(struct slot)

extern char end; /* Linker-defined, marks the end of the program's dataseg. */

#define DSEG_END ((void *)ALIGN((uintptr_t)&end, PAGE_SIZE))

/*
 * We use a fixed 8-byte header here, which is twice the overhead compared to
 * the Minix implementation of the same routines.
 * Might want to dig deeper and optimise this later.
 */

struct slot {
	struct slot *next_slot; /* Keeping a pointer here eliminates some ops */
	struct slot *next_free; /* Next free block (valid only in free list. */
	/* User data area follows */
};

/* Keeps the free list head. The free list is sorted by address. */
static struct slot *free_anchor;

/* Remember the arena start. */
static struct slot *first_slot;

/* Remember the program break. */
static void *arena_brk;

/* Allocates with getting some system memory. */
static int xmem_grow(size_t units, struct slot *last_free)
{
	struct slot *new_slot;
	void *new_brk;
	void *new_arena_brk;

	if (!arena_brk) {
		new_brk = (void *)ALIGN((unsigned)sys_brk(0), PAGE_SIZE);
		arena_brk = new_brk;
		first_slot = (struct slot *)new_brk;
		DBGPRINT("xmem_grow: first start, arena break: %08X.\n", arena_brk);
	}
	
	new_slot = arena_brk;
	new_brk = arena_brk + ALIGN(units * UNIT_SIZE, PAGE_SIZE);
	new_arena_brk = (struct slot *)sys_brk(new_brk);
	if ((void *)new_slot == new_arena_brk) {
		/* Seems that brk(2) has failed. Sorry. */
		DBGPRINT("xmem_grow: brk() failed.\n");
		return 0;
	}
	if (new_brk != new_arena_brk) {
		DBGPRINT("xmem_grow: brk() returned an unexpected value.\n");
		return 0;
	}
	arena_brk = new_arena_brk;

	DBGPRINT("xmem_grow: arena break: %08x -> %08x.\n", new_slot, new_arena_brk);
	
	if (last_free) {
		/* Simply append to the last free slot. */
		last_free->next_slot = new_slot;
	} else {
		/* This is now the list head. */
		new_slot->next_slot = (struct slot *)new_arena_brk;
		new_slot->next_free = NULL;
		free_anchor = new_slot;
	}
	return 1;
}

void *rtl_alloc(size_t size)
{
	struct slot *curr_free, *prev_free;
	size_t units;
	
	/* Make up the total alloc size */
	units = ALIGN(size, UNIT_SIZE) / UNIT_SIZE + 1;

	DBGPRINT("rtl_alloc: 0x%x units (0x%x bytes)\n", units, units * UNIT_SIZE);
	
	/* Don't bother -- cannot allocate that much */
	if (units > 0xFFFFFFU) {
		return NULL;
	}
	
	/* See if we can allocate anything. */
	for(;;) {
		/* Walk the free list, see if anything useful in there. */
		prev_free = NULL;
		curr_free = free_anchor;
		DBGPRINT("rtl_alloc: c=%08x\n", curr_free);
		while (curr_free) {
			struct slot *new_slot = curr_free + units;
			/* Check for an overflow */
			if (new_slot <= curr_free) {
				return NULL;
			}
			DBGPRINT("rtl_alloc: c=%08x, n=%08x, nf=%08x\n", curr_free, curr_free->next_slot, curr_free->next_free);
			/* Too small. */
			if (curr_free->next_slot < new_slot) {
				/* Advance to the next item. */
				prev_free = curr_free;
				curr_free = curr_free->next_free;
				continue;
			}
			/* Is it bigger, so it can be split? */
			if (curr_free->next_slot > new_slot + 1 ) {
				DBGPRINT("rtl_alloc: split\n");
				/* Fill in the newly formed slot. */
				new_slot->next_slot = curr_free->next_slot;
				new_slot->next_free = curr_free->next_free;
				/* Update this slot. */
				curr_free->next_slot = new_slot;
				curr_free->next_free = new_slot;
				DBGPRINT("rtl_alloc: old=%08x, n=%08x, nf=%08x\n", curr_free, curr_free->next_slot, curr_free->next_free);
				DBGPRINT("rtl_alloc: new=%08x, n=%08x, nf=%08x\n", new_slot, new_slot->next_slot, new_slot->next_free);
			}
			/* Remove it from the list. */
			if (prev_free) {
				prev_free->next_free = curr_free->next_free;
			} else {
				free_anchor = curr_free->next_free;
			}
			/* Delivered. */
			curr_free->next_free = NULL;
			DBGPRINT("rtl_alloc: result=%08x\n", curr_free);
			return curr_free + 1;
		}
		
		/* Nothing useful was found. Try to allocate some memory from the system. */
		if (!xmem_grow(units, prev_free)) {
			break;
		}
	}
	
	return NULL;
}

void rtl_free(void *ptr)
{
	struct slot *this = (struct slot *)ptr - 1;
	struct slot *prev_free;
	struct slot *next_free;
	
	if (!ptr)
		return;
	
	/* See where to insert the newly freed block. */
	prev_free = NULL;
	next_free = free_anchor;
	while (next_free && next_free < this) {
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

void *rtl_realloc(void *ptr, size_t size)
{
	struct slot *this = (struct slot *)ptr - 1;
	size_t old_count;
	struct slot *new;
	struct slot *prev_free;
	struct slot *next_free;
	size_t units;
	
	if (!ptr) {
		return rtl_alloc(size);
	}
	if (!size) {
		rtl_free(ptr);
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
			rtl_free(new);
		}
		return ptr;
	}
	
	/* No. Allocate new one. */
	new = (struct slot *)rtl_alloc(size);
	if (new) {
		xmemcpy(new, ptr, old_count);
		rtl_free(ptr);
	}
	return new;
}

#ifdef DEBUG

void rtl_alloc_dump(void)
{
	struct slot *s = first_slot;

	rtl_print_fd(2, "Slots start: %08x\n", s);

	rtl_print_fd(2, "Slots dump: \n");
	while (s < (struct slot *)arena_brk) {
		rtl_print_fd(2, "%08x (%08x) nf=%08x\n", s, (s->next_slot - s) * UNIT_SIZE, s->next_free);
		//rtl_print_fd(2, "%08x (%08x)\n", s, (s->next_slot - s) * UNIT_SIZE);
		s = s->next_slot;
	}
	rtl_print_fd(2, "Slots end.\n");
}

#endif // DEBUG
