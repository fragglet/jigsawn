
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
#include "lexer.h"

struct _JSONLexer {
        
        /** Input source reader */

        JSONInputReader reader;

        /** Pointer to buffer to contain token contents. */

        char *token_buffer;

        /** Allocated size of token_buffer. */

        size_t token_buffer_allocated;

        /** Number of characters in token_buffer. */

        size_t token_buffer_len;
};

/* Increase the size of token_buffer.  Returns zero for failure. */

static int json_lexer_enlarge_token_buffer(JSONLexer *lexer)
{
        char *new_buffer;
        size_t new_size;

        /* Increase by 32 characters each time. */

        new_size = lexer->token_buffer_allocated + 32;
        new_buffer = realloc(lexer->token_buffer, new_size);

        if (new_buffer == NULL) {
                return 0;
        }

        lexer->token_buffer = new_buffer;
        lexer->token_buffer_allocated = new_size;

        return 1;
}

/* Add a character to token_buffer.  Returns zero for failure. */

static int json_lexer_put_char(JSONLexer *lexer, char c)
{
        /* Enlarge buffer if necessary */

        if (lexer->token_buffer_len + 1 > lexer->token_buffer_allocated) {
                if (!json_lexer_enlarge_token_buffer(lexer)) {
                        return 0;
                }
        }

        /* Add the character */

        lexer->token_buffer[lexer->token_buffer_len] = c;
        ++lexer->token_buffer_len;

        return 1;
}

/* Read through the input stream until we run out of whitespace.  Returns
 * non-zero for success, zero if an error occurred. */

static int json_lexer_skip_whitespace(JSONLexer *lexer)
{
        int c;
        
        /* Read characters until we find a non-whitespace character. */

        do {
                c = json_lexer_read_char(lexer);

                if (c < 0) {
                        /* Error */
                        return 0;
                }
        } while (isspace(c));

        /* Skip back one character so we read the last (non-whitespace)
         * character again. */

        json_lexer_unread_char(lexer);

        return 1;
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

/* Read a unicode escape sequence.  This assumes that the preceding
 * '\u' has already been read.  Returns zero for failure. */

static int json_lexer_read_unicode(JSONLexer *lexer)
{
        return 1;
}

/* Read an escape character/sequence.  This assumes that the preceding
 * '\' has already been read.  Returns zero for failure. */

static int json_lexer_read_escape_char(JSONLexer *lexer)
{
        int c;

        /* Find what type of escape sequence */

        c = json_lexer_read_char(lexer);

        if (c < 0) {
                return 0;
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

                        return 0;
        }

        return 1;
}

/* Read a string.  Returns JSON_TOKEN_STRING if successful, or an
 * error token.  Assumes the opening " has already been read. */

static JSONToken json_lexer_read_string(JSONLexer *lexer)
{
        int c;

        for (;;) {

                /* Read the next character.  If we reach the end of file,
                 * this is always an error. */

                c = json_lexer_read_char(lexer);

                if (c < 0) {
                        return JSON_TOKEN_ERROR;
                }

                /* If we get a \, this is an escape character.  A "
                 * marks the end of string.  Otherwise, this is just a
                 * normal character. */

                if (c == '\\') {
                        if (!json_lexer_read_escape_char(lexer)) {
                                return JSON_TOKEN_ERROR;
                        }
                } else if (c == '\"') {
                        break;
                } else {
                        if (!json_lexer_put_char(lexer, c)) {
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
                
                c = json_lexer_read_char(lexer);

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

        if (!json_input_reader_init(&lexer->reader, source, read_func)) {
                free(lexer);
                return NULL;
        }

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

        /* Skip any whitespace preceding the token */

        if (!json_lexer_skip_whitespace(lexer)) {
                return json_lexer_error_result(lexer);
        }

        /* Read the character beginning the token */

        c = json_lexer_read_char(lexer);

        if (c < 0) {
                return json_lexer_error_result(lexer);
        }

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

