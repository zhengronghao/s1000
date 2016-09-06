/*
 * Philips/NXP Mifare Crypto-1 implementation v1.0
 *
 * By Karsten Nohl, Henryk Ploetz, Sean O'Neil
 *
 */

#include "mifare_crypto1.h"
#if (defined(EM_AS3911_Module))

/**
 * Create a new cipher instance of either card or reader side.
 *
 * @param state Pointer to the new cipher instance.
 * @param ciphter_type Type of the cipher.
 * @param implementation Implementation type of the cipher.
 * @return 1 if the specified cipher could be created, otherwise 0 is returned.
 */
int crypto1_new(crypto1_state *state, enum crypto1_cipher_type cipher_type, enum crypto1_cipher_implementation implementation)
{
  //	memset(state, 0, sizeof(*state));
	
    if(cipher_type == CRYPTO1_CIPHER_CARD)
	    state->is_card = 1;
    else
        state->is_card = 0;
	
	switch(implementation) {
	case CRYPTO1_IMPLEMENTATION_CLEAN:
		return _crypto1_new_clean(state);
	case CRYPTO1_IMPLEMENTATION_OPTIMIZED:
		/* return _crypto1_new_opt(state);
         * No optimized version implemented */
		return 0;
	}
	
	return 0;
}


/**
 * Initialize a cipher instance with secret key.
 *
 * @param state Cipher instance.
 * @param key MiFare key.
 */
void crypto1_init(crypto1_state *state, uint64emu_t key)
{
	state->ops->init(state, key);
}

/**
 * First stage of mutual authentication given a card's UID.
 * card_challenge is the card nonce as an integer
 */
void crypto1_mutual_1(crypto1_state *state, uint32_t uid, uint32_t card_challenge)
{
	state->ops->mutual_1(state, uid, card_challenge);
}

/**
 * Second stage of mutual authentication.
 * If this is the reader side, then the first 4 bytes of reader_response must
 * be preloaded with the reader nonce (and parity) and all 8 bytes will be
 * computed to be the correct reader response to the card challenge.
 * If this is the card side, then the response to the card challenge will be
 * checked.
 */
int crypto1_mutual_2(crypto1_state *state, parity_data_t *reader_response)
{
	if(state->is_card) {
		return state->ops->mutual_2_card(state, reader_response);
	} else {
		state->ops->mutual_2_reader(state, reader_response);
		return 1;
	}
}

/**
 * Third stage of mutual authentication.
 * If this is the reader side, then the card response to the reader
 * challenge will be checked.
 * If this is the card side, then the card response to the reader
 * challenge will be computed.
 */
int  crypto1_mutual_3(crypto1_state *state, parity_data_t *card_response)
{
	if(state->is_card) {
		state->ops->mutual_3_card(state, card_response);
		return 1;
	} else {
		return state->ops->mutual_3_reader(state, card_response);
	}
}

/**
 * Perform the Crypto-1 encryption or decryption operation on 'length' bytes
 * of data with associated parity bits.
 */
void crypto1_transcrypt(crypto1_state *state, parity_data_t *data, size_t length)
{
	crypto1_transcrypt_bits(state, data, length, 0);
}

/**
 * Perform the Crypto-1 encryption or decryption operation on 'bytes' bytes
 * of data with associated parity bits. 
 * The additional parameter 'bits' allows processing incomplete bytes after the
 * last byte. That is, if bits > 0 then data should contain (bytes+1) bytes where
 * the last byte is incomplete. 
 */
void crypto1_transcrypt_bits(crypto1_state *state, parity_data_t *data, size_t bytes, size_t bits)
{
	state->ops->transcrypt_bits(state, data, bytes, bits);
}

#endif

