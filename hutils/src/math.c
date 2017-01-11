#include <hutils/math.h>

/* TODO: These are really crude... Do IEEE754 properly! */

uint16_t
double2half(double a_d)
{
	union {
		double	d;
		uint64_t	u64;
	} u;
	uint32_t u32;
	int sign, exp_, mantissa;

	if (0.0 == a_d) {
		return 0;
	}
	u.d = a_d;
	u32 = u.u64 >> 32;
	sign = (0x80000000 & u32) >> 31;
	exp_ = (0x7ff00000 & u32) >> 20;
	mantissa = 0xfffff & u32;
	return sign << 15 | (exp_ - 1023 + 15) << 10 | mantissa >> 10;
}

double
half2double(uint16_t a_u16)
{
	union {
		double	d;
		uint64_t	u64;
	} u;
	uint32_t u32;
	uint32_t sign, exp_, mantissa;

	if (0 == a_u16) {
		return 0.0;
	}
	u.u64 = 0;
	sign = (0x8000 & a_u16) >> 15;
	exp_ = (0x7c00 & a_u16) >> 10;
	mantissa = 0x3ff & a_u16;
	u32 = sign << 31 | (exp_ - 15 + 1023) << 20 | mantissa << 10;
	u.u64 = (uint64_t)u32 << 32;
	return u.d;
}
