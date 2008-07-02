
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

#include "jigsawn/error.h"
#include "jigsawn/parser.h"

#include "input-reader.h"

#define ARRLEN(x) (sizeof(x) / sizeof(*x))

/* UTF-8 */

static const unsigned char utf8_test_input[] = {
        0x61, 
        0xc2, 0xa3,
        0xe2, 0xa0, 0x81,
        0xf0, 0x90, 0x90, 0x81
};

static const int utf8_test_expected[] = {
        0x61, 0xa3, 0x2801, 0x10401
};

/* UTF-16 (little endian) */

static const unsigned char utf16le_test_input[] = {
        0x61, 0x00,
        0xa3, 0x00,
        0x01, 0x28,
        0xaf, 0x7c
};

static const int utf16le_test_expected[] = {
        0x61, 0xa3, 0x2801, 0x7caf
};

/* UTF-16 (big endian) */

static const unsigned char utf16be_test_input[] = {
        0x00, 0x61,
        0x00, 0xa3,
        0x28, 0x01,
        0x7c, 0xaf
};

static const int utf16be_test_expected[] = {
        0x61, 0xa3, 0x2801, 0x7caf
};

/* UTF-32 (little endian) */

static const unsigned char utf32le_test_input[] = {
        0x61, 0x00, 0x00, 0x00,
        0xa3, 0x00, 0x00, 0x00,
        0x01, 0x28, 0x00, 0x00,
        0x01, 0x04, 0x01, 0x00,
        0x89, 0xab, 0xcd, 0x0f
};

static const int utf32le_test_expected[] = {
        0x61, 0xa3, 0x2801, 0x10401, 0xfcdab89
};

/* UTF-32 (big endian) */

static const unsigned char utf32be_test_input[] = {
        0x00, 0x00, 0x00, 0x61,
        0x00, 0x00, 0x00, 0xa3, 
        0x00, 0x00, 0x28, 0x01, 
        0x00, 0x01, 0x04, 0x01,
        0x0f, 0xcd, 0xab, 0x89 
};

static const int utf32be_test_expected[] = {
        0x61, 0xa3, 0x2801, 0x10401, 0xfcdab89
};

/* Code to read from a buffer */

typedef struct {
        const unsigned char *buffer;
        size_t offset;
        size_t length;
} ByteStream;

static void byte_stream_init(ByteStream *stream,
                             const unsigned char *buffer,
                             size_t length)
{
        stream->buffer = buffer;
        stream->length = length;
        stream->offset = 0;
}

static int byte_stream_read(void *src, unsigned char *buf, size_t buf_len)
{
        ByteStream *stream;

        stream = src;

        if (stream->offset < stream->length) {
                buf[0] = stream->buffer[stream->offset];
                ++stream->offset;
                return 1;
        } else {
                return 0;
        }
}

/* Test output reading from a specified input stream matches the provided
 * output. */

static void test_output(const unsigned char *input, size_t input_len,
                        const int *output, int output_len)
{
        ByteStream stream;
        JSONInputReader reader;
        int c;
        int i;

        byte_stream_init(&stream, input, input_len);
        json_input_reader_init(&reader, &stream, byte_stream_read);

        for (i=0; i<output_len; ++i) {

                assert(!json_input_is_eof(&reader));

                c = json_input_read_char(&reader);

                assert(c == output[i]);
        }

        assert(json_input_is_eof(&reader));
        assert(json_input_read_char(&reader) == JSON_ERROR_END_OF_FILE);
}

/* Test UTF-8 input */

static void test_utf8(void)
{
        test_output(utf8_test_input, ARRLEN(utf8_test_input),
                    utf8_test_expected, ARRLEN(utf8_test_expected));
}

/* Test UTF-16 (little endian) input */

static void test_utf16le(void)
{
        test_output(utf16le_test_input, ARRLEN(utf16le_test_input),
                    utf16le_test_expected, ARRLEN(utf16le_test_expected));
}

/* Test UTF-16 (big endian) input */

static void test_utf16be(void)
{
        test_output(utf16be_test_input, ARRLEN(utf16be_test_input),
                    utf16be_test_expected, ARRLEN(utf16be_test_expected));
}

/* Test UTF-32 (little endian) input */

static void test_utf32le(void)
{
        test_output(utf32le_test_input, ARRLEN(utf32le_test_input),
                    utf32le_test_expected, ARRLEN(utf32le_test_expected));
}

/* Test UTF-32 (big endian) input */

static void test_utf32be(void)
{
        test_output(utf32be_test_input, ARRLEN(utf32be_test_input),
                    utf32be_test_expected, ARRLEN(utf32be_test_expected));
}

int main(int argc, char *argv[])
{
        test_utf8();
        test_utf16le();
        test_utf16be();
        test_utf32le();
        test_utf32be();

        return 0;
}

