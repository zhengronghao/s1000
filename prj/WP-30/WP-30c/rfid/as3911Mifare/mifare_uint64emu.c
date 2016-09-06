//#include "mifare_uint64emu.h"
//#include "ams_types.h"
#include "wp30_ctrl.h"
#include "mifare_uint64emu.h"
#if (defined(EM_AS3911_Module))

#ifdef MSPGCC
void uint64emu_shl(uint64emu_t x, int n) {
  while (n--) {
    asm("clrc\n\t"
	"rlc %[x]\n\t"
	"rlc %[y]\n\t"
	"rlc %[z]\n\t"
	: [x] "=r" (x->data[0]),
	  [y] "=r" (x->data[1]),
	  [z] "=r" (x->data[2])
	: "[x]" (x->data[0]),
	"[y]" (x->data[1]),
	"[z]" (x->data[2])
	);
  }
}

void uint64emu_shr(uint64emu_t x, int n) {
  while (n--) {
    asm("clrc\n\t"
	"rrc %[x]\n\t"
	"rrc %[y]\n\t"
	"rrc %[z]\n\t"
	: [x] "=r" (x->data[2]),
        [y] "=r" (x->data[1]),
	[z] "=r" (x->data[0])
	: "[x]" (x->data[2]),
	  "[y]" (x->data[1]),
	  "[z]" (x->data[0])
	);
  }
}

int uint64emu_bit(const uint64emu_t x, int n) {
  return (x->data[ n / 16 ] >> (n % 16)) & 1;
}

void uint64emu_orbit(uint64emu_t x, int n, int val) {
  x->data[ n / 16 ] |= val << (n % 16);
}

void uint64emu_assign(uint64emu_t x, unsigned int a, unsigned int b, unsigned int c) {
  x->data[0] = a; x->data[1] = b; x->data[2] = c;
}

unsigned char uint64emu_byte(const uint64emu_t x, int n) {
  return (x->data[ n / 2 ] >> ((n % 2) * 8)) & 0xff;
}

void uint64emu_setbyte(uint64emu_t x, int n, unsigned char val) {
  x->data[ n / 2 ] &= 0xff << (((n % 2)?0:8));
  x->data[ n / 2 ] |= val << ((n % 2) * 8);
}


#else

void uint64emu_setbyte(uint64emu_t x, int n, unsigned char val)
{
    ((u8*) x)[n] = val;
}

unsigned char uint64emu_byte(const uint64emu_t x, int n)
{
  return ((u8*) x)[n];
}

void uint64emu_assign(uint64emu_t x, unsigned int a, unsigned int b, unsigned int c)
{
    ((u16*) x)[2] = c;
    ((u16*) x)[1] = b;
    ((u16*) x)[0] = a;
}

void uint64emu_orbit(uint64emu_t x, int n, int val)
{
    uint64emu_storage_t v = val;
    v <<= n;
    *x |= v;
}

int uint64emu_bit(uint64emu_t x, int n)
{
    return (*x >> n) & 1;
}

void uint64emu_shl(uint64emu_t x, int n)
{
    *x = (uint64emu_storage_t)(*x << n);
}

void uint64emu_shr(uint64emu_t x, int n)
{
    *x = *x >> n;
}

#endif
#endif

