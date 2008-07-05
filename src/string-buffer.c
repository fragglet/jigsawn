
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

#include "string-buffer.h" 
#include "utf8.h"

void json_string_buffer_init(JSONStringBuffer *buffer)
{
        buffer->buffer = NULL;
        buffer->buffer_len = 0;
        buffer->buffer_allocated = 0;
}

void json_string_buffer_free(JSONStringBuffer *buffer)
{
        free(buffer->buffer);
}

unsigned char *json_string_buffer_get(JSONStringBuffer *buffer)
{
        if (buffer->buffer_len == 0) {
                return buffer->buffer;
        } else {
                return NULL;
        }
}

void json_string_buffer_reset(JSONStringBuffer *buffer)
{
        buffer->buffer_len = 0;
}

/* Increase the size of the buffer.  Returns zero for success, or
 * negative error code. */

static int json_string_buffer_enlarge(JSONStringBuffer *buffer)
{
        char *new_buffer;
        size_t new_size;

        /* Increase by 32 characters each time. */

        new_size = buffer->buffer_len + 32;
        new_buffer = realloc(buffer->buffer, new_size);

        if (new_buffer == NULL) {
                return JSON_ERROR_OUT_OF_MEMORY;
        }

        buffer->buffer = new_buffer;
        buffer->buffer_allocated = new_size;

        return JSON_ERROR_SUCCESS;
}

/* Add a byte to the buffer.  Returns zero for success, or negative
 * error code. */

static int json_string_buffer_put_byte(JSONStringBuffer *buffer,
                                       unsigned char c)
{
        int err;

        /* Enlarge buffer if necessary */

        if (buffer->buffer_len + 1 > buffer->buffer_allocated) {
                err = json_string_buffer_enlarge(buffer);

                if (err < 0) {
                        return err;
                }
        }

        /* Add the character */

        buffer->buffer[buffer->buffer_len] = c;
        ++buffer->buffer_len;

        return JSON_ERROR_SUCCESS;
}

/* Add a character to token_buffer, performing encoding to UTF-8. 
 * Returns zero for success, or negative error code. */

int json_string_buffer_put_char(JSONStringBuffer *buffer, int c)
{
        unsigned char buf[4];
        int length;
        int err;
        int i;

        /* Encode into the buffer */

        json_utf8_encode(c, buf, &length);

        /* Add each byte to the buffer. */

        for (i=0; i<length; ++i) {
                err = json_string_buffer_put_byte(buffer, buf[i]);

                if (err < 0) {
                        return err;
                }
        }

        return JSON_ERROR_SUCCESS;
}

