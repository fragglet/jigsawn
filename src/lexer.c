
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
#include "lexer.h"

struct _JSONLexer {
        
        /** Input source reader */

        JSONInputReader reader;

        /** Pointer to buffer to contain token contents. */

        unsigned char *token_buffer;

        /** Allocated size of token_buffer. */

        size_t token_buffer_allocated;

        /** Number of characters in token_buffer. */

        size_t token_buffer_len;
};

/* Increase the size of token_buffer.  Returns zero for success, or
 * negative error code. */

static int json_lexer_enlarge_token_buffer(JSONLexer *lexer)
{
        char *new_buffer;
        size_t new_size;

        /* Increase by 32 characters each time. */

        new_size = lexer->token_buffer_allocated + 32;
        new_buffer = realloc(lexer->token_buffer, new_size);

        if (new_buffer == NULL) {
                return JSON_ERROR_OUT_OF_MEMORY;
        }

        lexer->token_buffer = new_buffer;
        lexer->token_buffer_allocated = new_size;

        return JSON_ERROR_SUCCESS;
}

/* Add a byte to token_buffer.  Returns zero for success, or error code. */

static int json_lexer_put_byte(JSONLexer *lexer, unsigned char c)
{
        int err;

        /* Enlarge buffer if necessary */

        if (lexer->token_buffer_len + 1 > lexer->token_buffer_allocated) {
                err = json_lexer_enlarge_token_buffer(lexer);

                if (err < 0) {
                        return err;
                }
        }

        /* Add the character */

        lexer->token_buffer[lexer->token_buffer_len] = c;
        ++lexer->token_buffer_len;

        return JSON_ERROR_SUCCESS;
}

/* Add a character to token_buffer, performing encoding to UTF-8. 
 * Returns zero for success, or negative error code. */

static int json_lexer_put_char(JSONLexer *lexer, int c)
{
        unsigned char buf[4];
        int length;
        int err;
        int i;

        /* Encode into the buffer */

        json_utf8_encode(c, buf, &length);

        /* Add each byte to the buffer. */

        for (i=0; i<length; ++i) {
                err = json_lexer_put_byte(lexer, buf[i]);

                if (err < 0) {
                        return err;
                }
        }

        return JSON_ERROR_SUCCESS;
}

/* Returns EOF token if EOF was reached, otherwise generic error token. */

static JSONToken json_lexer_error_result(JSONLexer *lexer)
{
        if (json_input_is_eof(&lexer->reader)) {
                return JSON_TOKEN_EOF;
        } else {
                return JSON_TOKEN_ERROR;
        }
}

/* Parse a hexadecimal character to a 0-15 value.  Returns negative
 * error code if this is not a hexadecimal character */

static int hex_to_i(int c)
{
        c = tolower(c);

        if (c >= '0' && c <= '9') {
                return c - '0';
        } else if (c >= 'a' && c <= 'f') {
                return 10 + c - 'a';
        } else {
                return JSON_ERROR_PARSE;
        }
}

/* Read a unicode escape sequence.  This assumes that the preceding
 * '\u' has already been read.  Returns zero for success,
 * or negative error code. */

static int json_lexer_read_unicode(JSONLexer *lexer)
{
        int i, j;
        int value;
        int c;

        /* Read four character hex sequence */

        value = 0;

        for (i=0; i<4; ++i) {
                c = json_input_read_char(&lexer->reader);

                if (c < 0) {
                        return c;
                }

                /* Parse hexadecimal character */

                j = hex_to_i(c);

                if (j < 0) {
                        return j;
                }

                /* Add to sequence */

                value = (value << 4) | j;
        }

        /* Add to string buffer */

        return json_lexer_put_char(lexer, value);
}

/* Read an escape character/sequence.  This assumes that the preceding
 * '\' has already been read.  Returns zero for success, or negative
 * error code. */

static int json_lexer_read_escape_char(JSONLexer *lexer)
{
        int c;

        /* Find what type of escape sequence */

        c = json_input_read_char(&lexer->reader);

        if (c < 0) {
                return c;
        }

        switch (c) {
                case '\"':                 /* Quotes */
                        return json_lexer_put_char(lexer, '\"');
                case '\\':                 /* Backslash */
                        return json_lexer_put_char(lexer, '\\');
                case '/':                  /* Slash */
                        return json_lexer_put_char(lexer, '/');
                case 'b':                  /* Backspace */
                        return json_lexer_put_char(lexer, '\b');
                case 'f':                  /* Form feed */
                        return json_lexer_put_char(lexer, '\f');
                case 'n':                  /* New line */
                        return json_lexer_put_char(lexer, '\n');
                case 'r':                  /* Carriage return */
                        return json_lexer_put_char(lexer, '\r');
                case 't':                  /* Tab */
                        return json_lexer_put_char(lexer, '\t');
                case 'u':
                        return json_lexer_read_unicode(lexer);
                default:
                        /* Invalid escape sequence */

                        return JSON_ERROR_PARSE;
        }

        return JSON_ERROR_SUCCESS;
}

/* Read a string.  Returns JSON_TOKEN_STRING if successful, or an
 * error token.  Assumes the opening " has already been read. */

static JSONToken json_lexer_read_string(JSONLexer *lexer)
{
        int c;
        int err;

        for (;;) {

                /* Read the next character.  If we reach the end of file,
                 * this is always an error. */

                c = json_input_read_char(&lexer->reader);

                if (c < 0) {
                        return JSON_TOKEN_ERROR;
                }

                /* If we get a \, this is an escape character.  A "
                 * marks the end of string.  Otherwise, this is just a
                 * normal character. */

                if (c == '\\') {
                        err = json_lexer_read_escape_char(lexer);

                        if (err < 0) {
                                return JSON_TOKEN_ERROR;
                        }
                } else if (c == '\"') {
                        break;
                } else {
                        err = json_lexer_put_char(lexer, c);

                        if (err < 0) {
                                return JSON_TOKEN_ERROR;
                        }
                }
        }

        return JSON_TOKEN_STRING;
}

/* Read a keyword, returning a token type if successfully matched,
 * or JSON_TOKEN_ERROR for failure.  Assumes the first character 
 * has already been read. */

static JSONToken json_lexer_read_keyword(JSONLexer *lexer,
                                         const char *keyword,
                                         JSONToken result)
{
        const char *p;
        int c;
      
        /* The first character of the keyword was already 
         * succesfully read. */

        p = keyword + 1;

        /* Check each character in turn */

        while (*p != '\0') {
                
                c = json_input_read_char(&lexer->reader);

                if (c < 0 || c != *p) {
                        return JSON_TOKEN_ERROR;
                }
                
                ++p;
        }

        /* Read successfully. */

        return result;
}

JSONLexer *json_lexer_new(JSONInputSource source,
                          JSONInputReadFunc read_func)
{
        JSONLexer *lexer;

        lexer = malloc(sizeof(JSONLexer));

        if (lexer == NULL) {
                return NULL;
        }

        json_input_reader_init(&lexer->reader, source, read_func);

        lexer->token_buffer = NULL;
        lexer->token_buffer_allocated = 0;

        return lexer;
}

void json_lexer_free(JSONLexer *lexer)
{
        free(lexer->token_buffer);
        free(lexer);
}

/* Read a token. */

JSONToken json_lexer_read_token(JSONLexer *lexer)
{
        int c;

        /* Read from the input stream until we reach a non-whitespace
         * character. */

        do {
                /* Read the character beginning the token */

                c = json_input_read_char(&lexer->reader);

                if (c < 0) {
                        return json_lexer_error_result(lexer);
                }
        } while (isspace(c));

        /* Default to empty buffer. */

        lexer->token_buffer_len = 0;

        /* Try to determine the token type */

        switch (c) {
                case '[':
                        return JSON_TOKEN_BEGIN_ARRAY;
                case ']':
                        return JSON_TOKEN_END_ARRAY;
                case '{':
                        return JSON_TOKEN_BEGIN_OBJECT;
                case '}':
                        return JSON_TOKEN_BEGIN_OBJECT;
                case '"':
                        return json_lexer_read_string(lexer);
                case 't':
                        return json_lexer_read_keyword(lexer,
                                                       "true",
                                                       JSON_TOKEN_TRUE);
                case 'f':
                        return json_lexer_read_keyword(lexer,
                                                       "false",
                                                       JSON_TOKEN_FALSE);
                case 'n':
                        return json_lexer_read_keyword(lexer,
                                                       "null",
                                                       JSON_TOKEN_NULL);
                case ':':
                        return JSON_TOKEN_COLON;
                case ',':
                        return JSON_TOKEN_COMMA;
        }

        return JSON_TOKEN_ERROR;
}

const char *json_lexer_get_buffer(JSONLexer *lexer)
{
        /* If the last token read had a specific value (string or number
         * value), return the token buffer.  Otherwise, return NULL. */

        if (lexer->token_buffer_len > 0) {
                return lexer->token_buffer;
        } else {
                return NULL;
        }
}

