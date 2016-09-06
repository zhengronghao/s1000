#ifndef UINT64EMU_H_
#define UINT64EMU_H_

#if (defined(EM_AS3911_Module))
#ifdef MSPGCC
    // least significant word first
    struct uint64emu {
      short data[3];
    };
    typedef struct uint64emu * uint64emu_t;
    typedef struct uint64emu uint64emu_storage_t;
#else
    typedef unsigned long long * uint64emu_t;
    typedef unsigned long long uint64emu_storage_t;
#endif

void uint64emu_shl(uint64emu_t x, int n);
void uint64emu_shr(uint64emu_t x, int n);
int uint64emu_bit(const uint64emu_t x, int n);
void uint64emu_orbit(uint64emu_t x, int n, int val);
void uint64emu_assign(uint64emu_t x, unsigned int a, unsigned int b, unsigned int c);
unsigned char uint64emu_byte(const uint64emu_t x, int n);
void uint64emu_setbyte(uint64emu_t x, int n, unsigned char val);
void uint64emu_dump(const char * msg, const uint64emu_t);

#endif /* UINT64EMU_H_ */
#endif

