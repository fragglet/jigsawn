
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

#ifndef JIGSAWN_PARSER_H
#define JIGSAWN_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/**
 * A handle for a JSON input source.
 */

typedef void *JSONInputSource;

/** 
 * Function pointer type for reading data from an input source.
 *
 * @param source       Handle for the input source to read from.
 * @param data         Pointer to a data buffer in which to store the data
 *                     read from the source.
 * @param data_len     Size of the data buffer, in bytes.
 * @return             Number of bytes read into the data buffer, or zero
 *                     if the end of file was reached.  If the value is 
 *                     negative, it indicates an error.
 */

typedef int (*JSONInputReadFunc)(JSONInputSource source, 
                                 unsigned char *data,
                                 size_t data_len);

/**
 * A JSON parser.
 */

typedef struct _JSONParser JSONParser;

/**
 * Create a new @param JSONParser.
 *
 * @param source        The source to read data from.
 * @param read_func     Callback function to invoke to read data from the
 *                      input source.
 * @return              A new @param JSONParser, or NULL if it was not
 *                      possible to create a new parser.
 */

JSONParser *json_parser_new(JSONInputSource source,
                            JSONInputReadFunc read_func);

/**
 * Free a @param JSONParser.
 *
 * @param parser        The parser.
 */

void json_parser_free(JSONParser *parser);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef JIGSAWN_PARSER_H */

