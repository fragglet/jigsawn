
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

#ifndef JIGSAWN_INTERNAL_STRING_BUFFER_H
#define JIGSAWN_INTERNAL_STRING_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

typedef struct _JSONStringBuffer JSONStringBuffer;

struct _JSONStringBuffer {

        /** Data buffer containing the string */

        unsigned char *buffer;

        /** Buffer length */

        size_t buffer_len;

        /** Allocated size */

        size_t buffer_allocated;
};

/**
 * Initialise a @ref JSONStringBuffer.
 *
 * @param buffer            Pointer to the buffer structure to initialise.
 */

void json_string_buffer_init(JSONStringBuffer *buffer);

/** 
 * Free all resources used by a @ref JSONStringBuffer.
 *
 * @param buffer            Pointer to the buffer to free.
 */

void json_string_buffer_free(JSONStringBuffer *buffer);

/**
 * Get the buffer contents.
 *
 * @param buffer            Pointer to the buffer.
 * @return                  Pointer to the byte data containing the buffer
 *                          contents, or NULL if the buffer is empty
 *                          (not even a terminating NUL).
 */

unsigned char *json_string_buffer_get(JSONStringBuffer *buffer);

/**
 * Reset the write pointer to the start of the buffer.
 *
 * @param buffer            Pointer to the buffer.
 */

void json_string_buffer_reset(JSONStringBuffer *buffer);

/**
 * Add a unicode character to a buffer.
 *
 * @param buffer            Pointer to the buffer.
 * @param c                 Unicode character to add.
 * @return                  Zero if successful, or non-zero error code.
 */

int json_string_buffer_put_char(JSONStringBuffer *buffer, int c);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef JIGSAWN_INTERNAL_STRING_BUFFER_H */

