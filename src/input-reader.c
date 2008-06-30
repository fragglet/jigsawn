
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

#include "input-reader.h"

/* Read a character from the input stream. */

int json_input_read_char(JSONInputReader *reader)
{
        int bytes;
        int result;

        /* End of file? */ 

        if (reader->eof) {
                return -1;
        }
        /* Reached the end of the current block? Read the next one. */

        if (reader->input_buffer_pos >= reader->input_buffer_len) {
                bytes = reader->read_func(reader->source, reader->input_buffer,
                                         sizeof(reader->input_buffer));

                if (bytes == 0) {
                        reader->eof = 1;
                }

                if (bytes <= 0) {
                        return -1;
                } else {
                        reader->input_buffer_len = bytes;
                        reader->input_buffer_pos = 0;
                }

        }

        /* Return the next character */

        result = reader->input_buffer[reader->input_buffer_pos];
        ++reader->input_buffer_pos;

        return result;
}

/* Undo reading the last character read by json_input_read_char. */

void json_input_unread_char(JSONInputReader *reader)
{
        assert(reader->input_buffer_pos > 0);
        --reader->input_buffer_pos;
}

/* Initialise JSONInputReader structure. */

void json_input_reader_init(JSONInputReader *reader,
                            JSONInputSource source,
                            JSONInputReadFunc read_func)
{
        reader->input_buffer_len = 0;
        reader->input_buffer_pos = 0;
        reader->eof = 0;
        reader->source = source;
        reader->read_func = read_func;
}


