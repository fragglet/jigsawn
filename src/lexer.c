
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
#include "string-buffer.h"

struct _JSONLexer {
        
        /** Input source reader */

        JSONInputReader reader;

        /** 
         * String buffers in which we store token contents.  We read
         * one token ahead so that we can "peek" at the next token; 
         * to do this, we need two separate string buffer.  We switch
         * between which buffer we're using.
         */

        JSONStringBuffer string_buffers[2];

        /**
         * Buffer used for the last token returned by
         * @ref json_lexer_read_token.
         */

        int current_buffer;

        /**
         * The next token to be returned by
         * @ref json_lexer_read_token.  We read one token ahead.
         */

        JSONToken next_token;

        /** 
         * Non-zero when we have read the first token.
         */

        int read_first;
};

/* Returns EOF token if EOF was reached, otherwise generic error token. */

static JSONToken error_result(JSONInputReader *reader)
{
        if (json_input_is_eof(reader)) {
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

/* Read a unicode escape sequence from a string, saving the escaped
 * character into the provided buffer.  This assumes that the preceding
 * '\u' has already been read.  Returns zero for success,
 * or negative error code. */

static int read_unicode_escape(JSONInputReader *reader,
                               JSONStringBuffer *buffer)
{
        int i, j;
        int value;
        int c;

        /* Read four character hex sequence */

        value = 0;

        for (i=0; i<4; ++i) {
                c = json_input_read_char(reader);

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

        return json_string_buffer_put_char(buffer, value);
}

/* Read an escape character/sequence, saving the escaped character
 * into the provided buffer.  This assumes that the preceding
 * '\' has already been read.  Returns zero for success, or negative
 * error code. */

static int read_escape_char(JSONInputReader *reader, JSONStringBuffer *buffer)
{
        int c;

        /* Find what type of escape sequence */

        c = json_input_read_char(reader);

        if (c < 0) {
                return c;
        }

        switch (c) {
                case '\"':                 /* Quotes */
                        return json_string_buffer_put_char(buffer, '\"');
                case '\\':                 /* Backslash */
                        return json_string_buffer_put_char(buffer, '\\');
                case '/':                  /* Slash */
                        return json_string_buffer_put_char(buffer, '/');
                case 'b':                  /* Backspace */
                        return json_string_buffer_put_char(buffer, '\b');
                case 'f':                  /* Form feed */
                        return json_string_buffer_put_char(buffer, '\f');
                case 'n':                  /* New line */
                        return json_string_buffer_put_char(buffer, '\n');
                case 'r':                  /* Carriage return */
                        return json_string_buffer_put_char(buffer, '\r');
                case 't':                  /* Tab */
                        return json_string_buffer_put_char(buffer, '\t');
                case 'u':
                        return read_unicode_escape(reader, buffer);
                default:
                        /* Invalid escape sequence */

                        return JSON_ERROR_PARSE;
        }

        return JSON_ERROR_SUCCESS;
}

/* Read a string, saving the string contents into the provided
 * buffer.  Returns JSON_TOKEN_STRING if successful, or an
 * error token.  Assumes the opening " has already been read. */

static JSONToken read_string(JSONInputReader *reader,
                             JSONStringBuffer *buffer)
{
        int c;
        int err;

        for (;;) {

                /* Read the next character.  If we reach the end of file,
                 * this is always an error. */

                c = json_input_read_char(reader);

                if (c < 0) {
                        return JSON_TOKEN_ERROR;
                }

                /* If we get a \, this is an escape character.  A "
                 * marks the end of string.  Otherwise, this is just a
                 * normal character. */

                if (c == '\\') {
                        err = read_escape_char(reader, buffer);

                        if (err < 0) {
                                return JSON_TOKEN_ERROR;
                        }
                } else if (c == '\"') {
                        break;
                } else {
                        err = json_string_buffer_put_char(buffer, c);

                        if (err < 0) {
                                return JSON_TOKEN_ERROR;
                        }
                }
        }

        /* Terminate the string */

        json_string_buffer_put_char(buffer, '\0');

        return JSON_TOKEN_STRING;
}

/* Read a keyword, returning a token type if successfully matched,
 * or JSON_TOKEN_ERROR for failure.  Assumes the first character 
 * has already been read. */

static JSONToken read_keyword(JSONInputReader *reader,
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
                
                c = json_input_read_char(reader);

                if (c < 0 || c != *p) {
                        return JSON_TOKEN_ERROR;
                }
                
                ++p;
        }

        /* Read successfully. */

        return result;
}

/**
 * Internal function to read the next token.  The specified
 * buffer is used to store the token contents.
 *
 * @param lexer            The lexer to read from.
 * @param buffer           @ref JSONStringBuffer to store the token contents.
 * @return                 Token, or @ref JSON_TOKEN_ERROR.
 */

static JSONToken internal_read_token(JSONInputReader *reader,
                                     JSONStringBuffer *buffer)
{
        int c;

        /* Read from the input stream until we reach a non-whitespace
         * character. */

        do {
                /* Read the character beginning the token */

                c = json_input_read_char(reader);

                if (c < 0) {
                        return error_result(reader);
                }
        } while (isspace(c));

        /* Start with an empty buffer. */

        json_string_buffer_reset(buffer);

        /* Try to determine the token type. */

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
                        return read_string(reader, buffer);
                case 't':
                        return read_keyword(reader, "true", JSON_TOKEN_TRUE);
                case 'f':
                        return read_keyword(reader, "false", JSON_TOKEN_FALSE);
                case 'n':
                        return read_keyword(reader, "null", JSON_TOKEN_NULL);
                case ':':
                        return JSON_TOKEN_COLON;
                case ',':
                        return JSON_TOKEN_COMMA;
        }

        return JSON_TOKEN_ERROR;
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
        json_string_buffer_init(&lexer->string_buffers[0]);
        json_string_buffer_init(&lexer->string_buffers[1]);

        lexer->current_buffer = 0;
        lexer->read_first = 0;

        return lexer;
}

void json_lexer_free(JSONLexer *lexer)
{
        json_string_buffer_free(&lexer->string_buffers[0]);
        json_string_buffer_free(&lexer->string_buffers[1]);
        free(lexer);
}

/* Check that we have read the first token, and read it if necessary. */

static void json_lexer_check_first(JSONLexer *lexer)
{
        /* Have we read the first token yet?  If not, we need to. */

        if (!lexer->read_first) {
                lexer->next_token
                        = internal_read_token(&lexer->reader,
                                              &lexer->string_buffers[0]);
                lexer->read_first = 1;
        }
}

/* Peek at the next token, but leaving it waiting in the queue to be
 * returned by @ref json_lexer_read_token. */

JSONToken json_lexer_peek_token(JSONLexer *lexer)
{
        /* Check we have read the first token */

        json_lexer_check_first(lexer);

        return lexer->next_token;
}

/* Read a token. */

JSONToken json_lexer_read_token(JSONLexer *lexer)
{
        JSONToken result;
        JSONStringBuffer *next_buffer;
        int new_current_buffer;

        /* Check we have read the first token */

        json_lexer_check_first(lexer);

        /* Once we reach an error, stop. */

        if (lexer->next_token == JSON_TOKEN_ERROR) {
                return JSON_TOKEN_ERROR;
        }

        /* The next token is waiting in the next_token field, and is
         * using the opposite buffer to the current one. */

        result = lexer->next_token;
        new_current_buffer = 1 - lexer->current_buffer;

        /* Retrieve the next token, overwriting the last token and its
         * buffer. */

        next_buffer = &lexer->string_buffers[lexer->current_buffer];
        lexer->next_token = internal_read_token(&lexer->reader, next_buffer);

        /* Update current_buffer and return the token */

        lexer->current_buffer = new_current_buffer;

        return result;
}

const char *json_lexer_get_buffer(JSONLexer *lexer)
{
        JSONStringBuffer *current_buffer;

        current_buffer = &lexer->string_buffers[lexer->current_buffer];

        return json_string_buffer_get(current_buffer);
}

