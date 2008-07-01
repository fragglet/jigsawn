
/*

Copyright (c) 2008, Simon Howard 

Permission to use, copy, modify, and/or distribute this software 
for any purpose with or without fee is hereby granted, provided 
that the above copyright notice and this permission notice appear 
in all copies. 

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL 
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE 
AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR 
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 


 */

#ifndef JIGSAWN_UTF8_H
#define JIGSAWN_UTF8_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/* Start of a 1 byte sequence:      0UTF8xxx */

#define JSON_UTF8_START_1       0x00      
#define JSON_UTF8_MASK_1        0x80

/* Start of a 2 byte sequence:      110UTF8x */

#define JSON_UTF8_START_2       0xc0
#define JSON_UTF8_MASK_2        0xe0

/* Start of a 3 byte sequence:      1110UTF8 */

#define JSON_UTF8_START_3       0xe0
#define JSON_UTF8_MASK_3        0xf0

/* Start of a 4 byte sequence:      11110xxx */

#define JSON_UTF8_START_4       0xf0
#define JSON_UTF8_MASK_4        0xf8

/* A byte following a start byte:   10UTF8xx */

#define JSON_UTF8_SEQ_BYTE      0x80
#define JSON_UTF8_SEQ_MASK      0xc0

/* Threshold unicode values for each sequence length. */

#define JSON_UTF8_THRESHOLD_2   0x80
#define JSON_UTF8_THRESHOLD_3   0x800
#define JSON_UTF8_THRESHOLD_4   0x10000

/**
 * Calculate the length of a UTF-8 sequence from its first byte.
 *
 * @param start_byte       The first byte in the sequence.
 * @return                 Length of the sequence in bytes, or -1 if
 *                         the sequence length could not be determined.
 */

int json_utf8_seq_length(unsigned char start_byte);

/**
 * Decode a UTF-8 sequence to a Unicode character.
 *
 * @param buf              Pointer to byte array containing data to be
 *                         decoded.
 * @param length           Length of the sequence, in bytes.
 */

int json_utf8_decode(unsigned char *buf, int length);

/**
 * Encode a unicode character to a UTF-8 sequence.
 *
 * @param value            The unicode character to encode.
 * @param buf              Pointer to the buffer to store the result.
 *                         The buffer must be at least four bytes in size.
 * @param length           Pointer to a variable to store the length of the
 *                         resulting buffer.
 */

void json_utf8_encode(int value, unsigned char *buf, size_t *length);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef JIGSAWN_UTF8_H */

