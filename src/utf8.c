
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

#include "jigsawn/error.h"

#include "utf8.h"

/* Masks for the first byte, depending on sequence length.  Note
 * that because this is zero-indexed, the index for the desired mask
 * is length - 1. */

static const int utf8_masks[] = {
        JSON_UTF8_MASK_1,
        JSON_UTF8_MASK_2,
        JSON_UTF8_MASK_3,
        JSON_UTF8_MASK_4,
};

/* Get the length of a UTF-8 sequence from its first byte */

int json_utf8_seq_length(unsigned char start_byte)
{
        if ((start_byte & JSON_UTF8_MASK_1) == JSON_UTF8_START_1) {
                return 1;
        } else if ((start_byte & JSON_UTF8_MASK_2) == JSON_UTF8_START_2) {
                return 2;
        } else if ((start_byte & JSON_UTF8_MASK_3) == JSON_UTF8_START_3) {
                return 3;
        } else if ((start_byte & JSON_UTF8_MASK_4) == JSON_UTF8_START_4) {
                return 4;
        } else {
                return JSON_ERROR_ENCODING;
        }
}

/* Decode a UTF-8 sequence */

int json_utf8_decode(const unsigned char *buf, int length) 
{
        int result;
        int start_mask;
        int c;
        int i;

        /* Mask out the first byte to get the initial bits of the value */

        start_mask = utf8_masks[length - 1];
        result = buf[0] & ~start_mask;

        /* Read each following character in sequence.  We have already
         * read the first byte. */

        for (i=1; i<length; ++i) {

                /* Sanity check the sequence bytes */

                c = buf[i];

                if ((c & JSON_UTF8_SEQ_MASK) != JSON_UTF8_SEQ_BYTE) {
                        return JSON_ERROR_ENCODING;
                }

                /* Each sequence byte has another 6 bits of data. */

                result = (result << 6) | (c & ~JSON_UTF8_SEQ_MASK);
        }

        return result;
}

void json_utf8_encode(int value, unsigned char *buf, size_t *length)
{
        int start, start_mask;
        int v;
        int i;
        int l;

        /* Work out how long the sequence must be */

        if (value >= JSON_UTF8_THRESHOLD_4) {
                start = JSON_UTF8_START_4;
                start_mask = JSON_UTF8_MASK_4;
                l = 4;
        } else if (value >= JSON_UTF8_THRESHOLD_3) {
                start = JSON_UTF8_START_3;
                start_mask = JSON_UTF8_MASK_3;
                l = 3;
        } else if (value >= JSON_UTF8_THRESHOLD_2) {
                start = JSON_UTF8_START_2;
                start_mask = JSON_UTF8_MASK_2;
                l = 2;
        } else {
                start = JSON_UTF8_START_1;
                start_mask = JSON_UTF8_MASK_1;
                l = 1;
        }

        /* Encode the bytes in the sequence backwards, starting from
         * the last byte. */

        v = value;

        for (i = l - 1; i > 0; --i) {
                buf[i] = (v & ~JSON_UTF8_SEQ_MASK) | JSON_UTF8_SEQ_BYTE;
                v >>= 6;
        }
        
        /* Write the starting byte */

        buf[0] = (v & ~start_mask) | start;

        /* Save the sequence length */

        *length = l;
}

