
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

#ifndef JIGSAWN_INPUT_READER_H
#define JIGSAWN_INPUT_READER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jigsawn/parser.h"

typedef struct _JSONInputReader JSONInputReader;

struct _JSONInputReader {

        /**
         * Input encoding format.
         */

        JSONInputEncoding encoding;
        
        /**
         * Input buffer. Data is read from the input stream into this 
         * buffer in blocks, and read out a character at a time.  When
         * the buffer is empty, it is refilled.
         */
        
        char input_buffer[256];

        /** Number of bytes in input_buffer.  */

        size_t input_buffer_len;

        /** Position we have reached reading data out of input_buffer.  */

        size_t input_buffer_pos;

        /** If true, the end of file has been reached. */

        int eof;

        /** Input source. */

        JSONInputSource *source;

        /** Callback function to read more data from the source. */

        JSONInputReadFunc read_func;
};

/**
 * Initialise a @ref JSONInputReader structure.
 *
 * @param reader           Pointer to the structure to initialise.
 * @param source           Handle for source to read data from.
 * @param read_func        Callback function to invoke to read more data.
 * @return                 Zero for success, or error value. 
 */

int json_input_reader_init(JSONInputReader *reader,
                           JSONInputSource source,
                           JSONInputReadFunc read_func);

/**
 * Read a character from a @ref JSONInputReader.
 *
 * @param reader           The reader to read from.
 * @return                 Character value, or negative error value.
 */

int json_input_read_char(JSONInputReader *reader);

/**
 * Skip back one character, undoing reading the last character returned
 * from @ref json_input_read_char.  The character previously returned 
 * will be returned again at the next call.
 *
 * @param reader           The reader.
 */

void json_input_unread_char(JSONInputReader *reader);

/**
 * Query if an input stream has reached the end of file. 
 *
 * @param reader           The reader.
 * @return                 Non-zero if end of file has been reached.
 */

int json_input_is_eof(JSONInputReader *reader);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef JIGSAWN_INPUT_READER_H */

