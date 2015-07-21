#include <hutils/bitmask.h>
#include <stdint.h>
#include <hutils/memory.h>

struct Bitmask {
	int	size;
	uint32_t	*data;
};

void
bitmask_clear(struct Bitmask *const a_bm, int const a_index)
{
	if (0 > a_index || a_bm->size <= a_index) {
		abort();
	}
	a_bm->data[a_index / 32] &= ~(1 << (31 & a_index));
}

struct Bitmask *
bitmask_create(int const a_size)
{
	struct Bitmask *bm;

	if (0 >= a_size) {
		abort();
	}
	CALLOC(bm, 1);
	bm->size = a_size;
	CALLOC(bm->data, (31 + a_size) / 32);
	return bm;
}

void
bitmask_free(struct Bitmask **const a_bm)
{
	struct Bitmask *bm;

	bm = *a_bm;
	if (NULL == bm) {
		return;
	}
	FREE(bm->data);
	FREE(*a_bm);
}

int
bitmask_get(struct Bitmask *const a_bm, int const a_index)
{
	if (0 > a_index || a_bm->size <= a_index) {
		abort();
	}
	return (1 << (31 & a_index)) & a_bm->data[a_index / 32];
}

void
bitmask_set(struct Bitmask *const a_bm, int const a_index)
{
	if (0 > a_index || a_bm->size <= a_index) {
		abort();
	}
	a_bm->data[a_index / 32] |= (1 << (31 & a_index));
}
