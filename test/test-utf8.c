
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "utf8.h"

/* ASCII lower case 'a', 0x61 */
#define EXPECTED_1  0x61
static const unsigned char one_byte_character[] = { "a" };

/* Pound sterling character, 0xA3 */
#define EXPECTED_2  0xa3
static const unsigned char two_byte_character[] = { 0xc2, 0xa3 };

/* Braille pattern, dots-1, 0x2801 */
#define EXPECTED_3  0x2801
static const unsigned char three_byte_character[] = { 0xe2, 0xa0, 0x81 };

/* Deseret capital letter long E, 0x10401 */
#define EXPECTED_4  0x10401
static const unsigned char four_byte_character[] = { 0xf0, 0x90, 0x90, 0x81 };

static void test_seq_length(void)
{
        assert(json_utf8_seq_length(one_byte_character[0]) == 1);
        assert(json_utf8_seq_length(two_byte_character[0]) == 2);
        assert(json_utf8_seq_length(three_byte_character[0]) == 3);
        assert(json_utf8_seq_length(four_byte_character[0]) == 4);
        
        /* Invalid */

        assert(json_utf8_seq_length(0xff) < 0);
}

static void test_decode(void)
{
        assert(json_utf8_decode(one_byte_character, 1) == EXPECTED_1);
        assert(json_utf8_decode(two_byte_character, 2) == EXPECTED_2);
        assert(json_utf8_decode(three_byte_character, 3) == EXPECTED_3);
        assert(json_utf8_decode(four_byte_character, 4) == EXPECTED_4);
}

static void test_encode(void)
{
        unsigned char buf[4];
        size_t length;

        json_utf8_encode(EXPECTED_1, buf, &length);
        assert(length == 1);
        assert(memcmp(buf, one_byte_character, 1) == 0);

        json_utf8_encode(EXPECTED_2, buf, &length);
        assert(length == 2);

        assert(memcmp(buf, two_byte_character, 2) == 0);

        json_utf8_encode(EXPECTED_3, buf, &length);
        assert(length == 3);
        assert(memcmp(buf, three_byte_character, 3) == 0);

        json_utf8_encode(EXPECTED_4, buf, &length);
        assert(length == 4);
        assert(memcmp(buf, four_byte_character, 1) == 0);
}

int main(int argc, char *argv[])
{
        test_seq_length();
        test_decode();
        test_encode();

        return 0;
}

