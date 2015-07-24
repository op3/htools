#ifndef HUTILS_BITMASK_H
#define HUTILS_BITMASK_H

#include <hutils/macros.h>

struct Bitmask;

void		bitmask_clear(struct Bitmask *, int);
struct Bitmask	*bitmask_create(int) FUNC_RETURNS;
void		bitmask_free(struct Bitmask **);
int		bitmask_get(struct Bitmask *, int) FUNC_RETURNS;
void		bitmask_one(struct Bitmask *);
void		bitmask_set(struct Bitmask *, int);
void		bitmask_zero(struct Bitmask *);

#endif