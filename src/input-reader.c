
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

#include <stdlib.h>

#include "jigsawn/error.h"

#include "input-reader.h"
#include "utf8.h"

/* Templates for determining encoding type */

static const int encoding_templates[NUM_JSON_ENCODINGS][4] = {
        { 1, 1, 1, 1 },       /* JSON_ENCODING_UTF8 */
        { 1, 0, 1, 0 },       /* JSON_ENCODING_16LE */
        { 0, 1, 0, 1 },       /* JSON_ENCODING_16BE */
        { 1, 0, 0, 0 },       /* JSON_ENCODING_32LE */
        { 0, 0, 0, 1 },       /* JSON_ENCODING_32BE */
};

/* Character lengths for each of the encoding types */

static const int encoding_lengths[] = {
        1,                    /* JSON_ENCODING_UTF8, unused */
        2,                    /* JSON_ENCODING_16LE */
        2,                    /* JSON_ENCODING_16BE */
        4,                    /* JSON_ENCODING_32LE */
        4,                    /* JSON_ENCODING_32BE */
};

/* Fill the input buffer.  Returns zero for success, or error code. */

static int json_input_buffer_fill(JSONInputReader *reader)
{
        char *buffer;
        int bytes;
        int remaining = sizeof(reader->input_buffer);
         
        /* Read as many bytes as possible until the buffer becomes 
         * full or we reach the end of file */

        buffer = reader->input_buffer;

        while (remaining > 0) {
                bytes = reader->read_func(reader->source, 
                                          buffer,
                                          remaining);

                if (bytes == 0) {
                        reader->eof = 1;
                        break;
                } else if (bytes < 0) {
                        return JSON_ERROR_INPUT_STREAM;
                }

                buffer += bytes;
                remaining -= bytes;
        }

        return JSON_ERROR_SUCCESS;
}

/* Check if a four byte sequence matches an encoding template */

static int check_template(char *p, const int *template)
{
        int i;

        for (i=0; i<4; ++i) {
                if ((p[i] != 0) != template[i]) {
                        return 0;
                }
        }

        return 1;
}

/* Determine the encoding of the input data.  Returns zero for success,
 * or negative error code. */

static int json_input_find_encoding(JSONInputReader *reader)
{
        int i, j;
        int err;

        err = json_input_buffer_fill(reader);

        if (err < 0) {
                return err;
        }

        /* If less than four bytes, fall back to UTF8 as a default. */

        if (reader->input_buffer_len < 4) {
                reader->encoding = JSON_ENCODING_UTF8;
                return JSON_ERROR_SUCCESS;
        }

        /* Check the first four bytes against encoding templates */

        for (i=0; i<NUM_JSON_ENCODINGS; ++i) {
                if (check_template(reader->input_buffer, 
                                   encoding_templates[i])) {
                        reader->encoding = i;
                        return JSON_ERROR_SUCCESS;
                }
        }

        /* Failed to find an encoding type that we recognise.  Fall back
         * to UTF-8.*/

        return JSON_ERROR_UNKNOWN_ENCODING;
}

/* Read a byte from the input stream.  Returns the byte value or 
 * negative error code. */

static int json_input_read_byte(JSONInputReader *reader)
{
        int result;
        int err;

        /* Reached the end of the current block? Read the next one. */

        if (reader->input_buffer_pos >= reader->input_buffer_len) {

                /* End of file? */ 

                if (reader->eof) {
                        return JSON_ERROR_END_OF_FILE;
                }

                err = json_input_buffer_fill(reader);

                if (err < 0) {
                        return err;
                }

                reader->input_buffer_pos = 0;
        }

        /* End of file? */ 

        if (reader->eof) {
                return JSON_ERROR_END_OF_FILE;
        }

        /* Return the next character */

        result = reader->input_buffer[reader->input_buffer_pos];
        ++reader->input_buffer_pos;

        return result;
}

/* Read a UTF-8 encoded character.  Returns negative error code if
 * an error occurs.*/

static int json_input_read_utf8(JSONInputReader *reader)
{
        unsigned char buf[4];
        int seq_length;
        int c;
        int i;

        /* Read the first byte */

        c = json_input_read_byte(reader);

        if (c < 0) {
                return c;
        }

        /* Determine the sequence length */

        seq_length = json_utf8_seq_length(c);

        /* Read extra bytes in the sequence. */

        buf[0] = c;

        for (i=1; i<seq_length; ++i) {
                c = json_input_read_byte(reader);

                if (c < 0) {
                        return c;
                }

                buf[i] = c;
        }

        /* Decode the character */

        return json_utf8_decode(buf, seq_length);
}

/* Read a character. */

int json_input_read_char(JSONInputReader *reader)
{
        unsigned char charbuf[4];
        int len;
        int err;
        int c;
        int i;

        /* If we have not yet determined the Unicode encoding type,
         * determine it now. */

        if (reader->encoding == JSON_ENCODING_UNKNOWN) {
                err = json_input_find_encoding(reader);

                if (err < 0) {
                        return err;
                }
        }

        /* UTF-8 is a special case */

        if (reader->encoding == JSON_ENCODING_UTF8) {
                return json_input_read_utf8(reader);
        }

        /* Read bytes needed for the character */

        len = encoding_lengths[i];

        for (i=0; i<len; ++i) {
                c = json_input_read_byte(reader);

                if (c < 0) {
                        return c;
                }

                charbuf[i] = c;
        }

        /* Decode the character */

        switch (reader->encoding) {
                case JSON_ENCODING_16LE:
                        return (charbuf[1] << 8) | charbuf[0];
                case JSON_ENCODING_16BE:
                        return (charbuf[0] << 8) | charbuf[1];
                case JSON_ENCODING_32LE:
                        return (charbuf[3] << 24)
                             | (charbuf[2] << 16)
                             | (charbuf[1] << 8)
                             | charbuf[0];
                case JSON_ENCODING_32BE:
                        return (charbuf[0] << 24)
                             | (charbuf[1] << 16)
                             | (charbuf[2] << 8)
                             | charbuf[3];
                default:
                        return JSON_ERROR_ENCODING;
        }
}

/* Returns non-zero if end of file has been reached. */

int json_input_is_eof(JSONInputReader *reader)
{
        return reader->eof
            && reader->input_buffer_pos >= reader->input_buffer_len;
}

/* Initialise JSONInputReader structure. */

void json_input_reader_init(JSONInputReader *reader,
                            JSONInputSource source,
                            JSONInputReadFunc read_func)
{
        reader->encoding = JSON_ENCODING_UNKNOWN;
        reader->input_buffer_len = 0;
        reader->input_buffer_pos = 0;
        reader->eof = 0;
        reader->source = source;
        reader->read_func = read_func;
}

