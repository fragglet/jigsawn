
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

#ifndef JIGSAWN_INTERNAL_LEXER_H
#define JIGSAWN_INTERNAL_LEXER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jigsawn/parser.h"

/*
 * JSON lexer, used to parse a stream of JSON data into tokens.
 */

typedef struct _JSONLexer JSONLexer;

/**
 * A JSON token type.
 */

typedef enum {
        JSON_TOKEN_BEGIN_ARRAY,           /* [ */
        JSON_TOKEN_END_ARRAY,             /* ] */
        JSON_TOKEN_BEGIN_OBJECT,          /* { */
        JSON_TOKEN_END_OBJECT,            /* } */
        JSON_TOKEN_INTEGER,               /* [0-9]+ */
        JSON_TOKEN_FLOAT,                 /* [0-9]+.[0-9]+ */
        JSON_TOKEN_STRING,                /* "text" */
        JSON_TOKEN_TRUE,                  /* true */
        JSON_TOKEN_FALSE,                 /* false */
        JSON_TOKEN_NULL,                  /* null */
        JSON_TOKEN_COMMA,                 /* , */
        JSON_TOKEN_COLON,                 /* : */
        JSON_TOKEN_EOF,                   /* End of file reached */
        JSON_TOKEN_ERROR,                 /* An error occurred */
} JSONToken;

/**
 * Create a new @ref JSONLexer.
 *
 * @param source            Source to read the JSON data from.
 * @param read_func         Callback function to invoke to read data.
 * @return                  A new @ref JSONLexer, or NULL if it was not 
 *                          possible to initialise the new lexer.
 */

JSONLexer *json_lexer_new(JSONInputSource source,
                          JSONInputReadFunc read_func);

/**
 * Free a @ref JSONLexer.
 *
 * @param lexer             The lexer to free.
 */

void json_lexer_free(JSONLexer *lexer);

/**
 * Read the next token from the input stream. 
 *
 * @param lexer             The lexer.
 * @return                  Type of the token read, or @ref JSON_TOKEN_ERROR
 *                          if an error occurred while reading.
 */

JSONToken json_lexer_read_token(JSONLexer *lexer);

/**
 * Get the contents of the token buffer used to store the contents of
 * the last token that was read.  This is only valid for some token types -
 * the string and number types.
 *
 * @param lexer             The lexer.
 * @return                  Pointer to the token buffer.
 */

const char *json_lexer_get_buffer(JSONLexer *lexer);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef JIGSAWN_INTERNAL_LEXER_H */

