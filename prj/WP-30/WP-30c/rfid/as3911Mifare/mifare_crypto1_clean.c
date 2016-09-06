/*
 * Philips/NXP Mifare Crypto-1 implementation v1.0
 *
 * By Karsten Nohl, Henryk Ploetz, Sean O'Neil
 *
 */

#include "mifare_crypto1.h"
#if (defined(EM_AS3911_Module))

/* IMPLEMENTATION section ==================== */
/* == PRNG function ========================== */

/**
 * Clock the prng register by n steps and return the new state, don't
 * update the register. 
 * Note: returns a 32 bit value, even if the register is only 16 bit wide.
 * This return value is only valid, when the register was clocked at least
 * 16 times.
 */
static uint32_t prng_next(const crypto1_state * const state, const size_t n)
{
	uint32_t prng = state->prng;
	int i;
	
	/* The register is stored and returned in reverse bit order, this way, even
	 * if we cast the returned 32 bit value to a 16 bit value, the necessary
	 * state will be retained. */
	prng = rev32(prng);
	for (i = 0; i < n; i++) 
		prng = (prng<<1) | ( ((prng>>15)^(prng>>13)^(prng>>12)^(prng>>10)) & 1 );
	return rev32(prng);
}

/* == keystream generating filter function === */
/**
 * This macro selects the four bits at offset a, b, c and d from the value x and returns the
 * concatenated bitstring  x_d || x_c || x_b || x_a  as an integer
 */
#define i4(x,a,b,c,d) ((uint32_t)(				\
				  uint64emu_bit(x, a)<<0	\
				  | uint64emu_bit(x, b)<<1	\
				  | uint64emu_bit(x, c)<<2	\
				  | uint64emu_bit(x, d)<<3	\
				  ))

/**
 * These macros are linearized boolean tables for the output filter functions. 
 * E.g. fa(0,1,0,1) is  (mf2_f4a >> 0x5)&1
 */
static const uint32_t mf2_f4a = 0x9E98;
static const uint32_t mf2_f4b = 0xB48E;
static const uint32_t mf2_f5c = 0xEC57E80A;

/**
 * Return one bit of non-linear filter function output for 48 bits of state input
 */
static uint32_t mf20 (const uint64emu_t x)
{
	const uint32_t d = 2;	/* number of cycles between when key stream is produced 
	                         * and when key stream is used.
							 * Irrelevant for software implmentations, but important
							 * to consider in side-channel attacks */
							 

	const uint32_t i5 = ((mf2_f4b >> i4 (x, 7+d, 9+d,11+d,13+d)) & 1)<<0
                       | ((mf2_f4a >> i4 (x,15+d,17+d,19+d,21+d)) & 1)<<1
                       | ((mf2_f4a >> i4 (x,23+d,25+d,27+d,29+d)) & 1)<<2
                       | ((mf2_f4b >> i4 (x,31+d,33+d,35+d,37+d)) & 1)<<3
                       | ((mf2_f4a >> i4 (x,39+d,41+d,43+d,45+d)) & 1)<<4;	

	return (mf2_f5c >> i5) & 1;
}

/* == LFSR state update functions ============ */
/**
 * Updates the 48-bit LFSR in state using the mifare taps, optionally
 * XORing in 1 bit of additional input, optionally XORing in 1 bit of 
 * cipher stream output (e.g. feeding back the output).
 * Return current cipher stream output bit.
 */
static uint8_t mifare_update (struct _crypto1_state * const state, 
		const uint8_t injection, const uint8_t feedback)
{
	const uint64emu_t x = & state->lfsr;
	const uint8_t ks = mf20(&state->lfsr);
	
	//	uint64emu_dump("update", &state->lfsr);
	int b = uint64emu_bit(x, 0) ^
	  uint64emu_bit(x, 5) ^
	  uint64emu_bit(x, 9) ^
	  uint64emu_bit(x, 10) ^
	  uint64emu_bit(x, 12) ^
	  uint64emu_bit(x, 14) ^
	  uint64emu_bit(x, 15) ^
	  uint64emu_bit(x, 17) ^
	  uint64emu_bit(x, 19) ^
	  uint64emu_bit(x, 24) ^
	  uint64emu_bit(x, 25) ^
	  uint64emu_bit(x, 27) ^
	  uint64emu_bit(x, 29) ^
	  uint64emu_bit(x, 35) ^
	  uint64emu_bit(x, 39) ^
	  uint64emu_bit(x, 41) ^
	  uint64emu_bit(x, 42) ^
	  uint64emu_bit(x, 43);
	b ^= injection ^ (feedback ? ks : 0);
	
	uint64emu_shr(&state->lfsr, 1);
	uint64emu_orbit(&state->lfsr, 47, b);

	return ks;
}

/**
 * Update the 48-bit LFSR in state using the mifare taps 8 times, optionally
 * XORing in 1 bit of additional input per step (LSBit first).
 * Return corresponding cipher stream.
 */ 
static uint8_t mifare_update_byte (struct _crypto1_state * const state, 
		const uint8_t injection, const uint8_t feedback)
{
	uint8_t ret = 0;
	int i;
	for(i = 0; i < 8; i++)
		ret |= mifare_update(state, bit(injection, i), feedback) << i;
	return ret;
}

/**
 * Update the 48-bit LFSR in state using the mifare taps 32 times, optionally
 * XORing in 1 byte of additional input per step (MSByte first).
 * Return the corresponding cipher stream.
 */
static uint32_t mifare_update_word (struct _crypto1_state * const state, 
		const uint32_t injection, const uint8_t feedback)
{
	uint32_t ret = 0;
	int i;
	for(i = 3; i >= 0; i--)
		ret |= (uint32_t)mifare_update_byte(state, (injection >> (i*8)) & 0xff, feedback) << (i*8);
	return ret;
}



/* API section =============================== */
/**
 * Initialize the LFSR with the key
 */ 
static void crypto1_clean_init(struct _crypto1_state * const state,
		const uint64emu_t key)
{
  uint64emu_assign(&state->lfsr, 0, 0, 0);
  state->prng = 0;
	
  /* Shift in keybytes in reverse order */

  int i;
  for(i=0; i<6; i++) {
    uint64emu_setbyte(&state->lfsr, i,
		      uint64emu_byte(key, 5 - i));
  }
}

/**
 * Shift UID xor card_nonce into the LFSR without active cipher stream feedback
 */
static void crypto1_clean_mutual_1(struct _crypto1_state * const state, 
		const uint32_t uid, const uint32_t card_challenge)
{
 	const uint32_t IV = uid ^ card_challenge;
 	
	/* Go through the IV bytes in MSByte first, LSBit first order */
 	mifare_update_word(state, IV, 0);
 	
 	state->prng = card_challenge; /* Load the card's PRNG state into our PRNG */
 	return;
}

/**
 * Mutual authentication step 1 in the case a reauthentication is performed.
 */ 
void crypto1_clean_mutual_1_2(struct _crypto1_state * const state, const uint32_t uid, const uint32_t card_challange)
{
    /* feed card challange xor uid into the cipher with feedback */
    uint32_t key_stream = mifare_update_word(state, card_challange ^ uid, 1);

    uint32_t decrypted_card_challange = key_stream ^ card_challange;
    state->prng = decrypted_card_challange;
    return;
}

/**
 * Shift in the reader nonce to generate the reader challenge, then generate the reader response
 */
static void crypto1_clean_mutual_2_reader(struct _crypto1_state * const state, 
		parity_data_t * const reader_response)
{
	/* Unencrypted reader nonce */
	const uint32_t reader_nonce = ARRAY_TO_UINT32(reader_response);
	int i;
	
	/* Feed the reader nonce into the state and simultaneously encrypt it */
	for(i = 3; i >= 0; i--) { /* Same as in mifare_update_word, but with added parity */
		reader_response[3-i] = reader_response[3-i] ^ mifare_update_byte(state, (reader_nonce >> (i*8)) & 0xff, 0);
		reader_response[3-i] ^= mf20(&state->lfsr)<<8;
	}
	
	/* Unencrypted reader response */
	const uint32_t RR = prng_next(state, 64);
	UINT32_TO_ARRAY_WITH_PARITY(RR, reader_response+4);
	
	/* Encrypt the reader response */
	crypto1_transcrypt(state, reader_response+4, 4);
}

/**
 * Generate the expected card response and compare it to the actual card response
 */
static int  crypto1_clean_mutual_3_reader(struct _crypto1_state * const state, 
		const parity_data_t * const card_response)
{
	const uint32_t TR_is = ARRAY_TO_UINT32(card_response);
	const uint32_t TR_should = prng_next(state, 96) ^ mifare_update_word(state, 0, 0);
	
	return TR_is == TR_should;
}

/**
 * Shift in the reader challenge into the state, generate expected reader response and compare
 * it to actual reader response.
 */
static int  crypto1_clean_mutual_2_card(struct _crypto1_state * const state, 
		const parity_data_t * const reader_response)
{
	/* Reader challenge/Encrypted reader nonce */
	const uint32_t RC = ARRAY_TO_UINT32(reader_response);
	/* Encrypted reader response */
	const uint32_t RR_is = ARRAY_TO_UINT32(reader_response+4);
	
	/* Shift in reader challenge */
	const uint32_t keystream = mifare_update_word(state, RC, 1);
#ifdef PRINT_CORRECT_READER_CHALLENGE 
	printf("%016LX\n", RC ^ keystream);
#else
	(void)keystream;
#endif
	
	/* Generate expected reader response */
	const uint32_t RR_should = prng_next(state, 64) ^ mifare_update_word(state, 0, 0);
	
	return RR_should == RR_is;
}

/**
 * Output the card response
 */
static void crypto1_clean_mutual_3_card(struct _crypto1_state * const state, 
		parity_data_t * const card_response)
{
	/* Unencrypted tag response */
	const uint32_t TR = prng_next(state, 96);
	UINT32_TO_ARRAY_WITH_PARITY(TR, card_response);
	
	/* Encrypt the response */
	crypto1_transcrypt(state, card_response, 4);
}

/**
 * Encrypt or decrypt a number of bytes
 */
static void crypto1_clean_transcrypt_bits(struct _crypto1_state * const state, 
		parity_data_t * const data, const size_t bytes, const size_t bits)
{
	int i;
	for(i = 0; i< bytes; i++) {
		data[i] = data[i] ^ mifare_update_byte(state, 0, 0);
		data[i] = data[i] ^ (mf20(&state->lfsr) << 8);
	}
	for(i = 0; i< bits; i++) {
		data[bytes] ^= mifare_update(state, 0, 0) << i;
	}
}

static const struct _crypto1_ops crypto1_clean_ops = {
		crypto1_clean_init,
		crypto1_clean_mutual_1,
		crypto1_clean_mutual_2_reader,
		crypto1_clean_mutual_3_reader,
		crypto1_clean_mutual_2_card,
		crypto1_clean_mutual_3_card,
		crypto1_clean_transcrypt_bits
};

int _crypto1_new_clean(crypto1_state * const state)
{
	state->ops = &crypto1_clean_ops;
	return 1;
}

#endif

