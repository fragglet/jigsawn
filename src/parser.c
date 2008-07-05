
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

#include "parser.h"
#include "lexer.h"
#include "value.h"

JSONParser *json_parser_new(JSONInputSource source, 
                            JSONInputReadFunc read_func)
{
        JSONParser *parser;
        JSONLexer *lexer;

        /* Allocate the parser */

        parser = malloc(sizeof(JSONParser));

        if (parser == NULL) {
                return NULL;
        }

        /* Create the lexer */

        lexer = json_lexer_new(source, read_func);

        if (lexer == NULL) {
                free(parser);
                return NULL;
        }

        parser->lexer = lexer;

        return parser;
}

void json_parser_free(JSONParser *parser)
{
        json_lexer_free(parser->lexer);

        free(parser);
}

JSONValue *json_parser_read_value(JSONParser *parser)
{
        JSONToken token;
        const char *token_data;

        token = json_lexer_read_token(parser->lexer);
        token_data = json_lexer_get_buffer(parser->lexer);

        switch (token) {
                case JSON_TOKEN_BEGIN_ARRAY:
                        /* Start of an array */
                        return json_value_new(JSON_VALUE_ARRAY, NULL);

                case JSON_TOKEN_BEGIN_OBJECT:
                        /* Start of an object */
                        return json_value_new(JSON_VALUE_OBJECT, NULL);

                case JSON_TOKEN_INTEGER:
                        /* Integer */
                        return json_value_new(JSON_VALUE_INT, token_data);

                case JSON_TOKEN_FLOAT:
                        /* Floating point value */
                        return json_value_new(JSON_VALUE_FLOAT, token_data);

                case JSON_TOKEN_STRING:
                        /* String value */
                        return json_value_new(JSON_VALUE_STRING, token_data);

                case JSON_TOKEN_TRUE:
                        /* Boolean */
                        return json_value_new(JSON_VALUE_BOOLEAN, "1");

                case JSON_TOKEN_FALSE:
                        /* Boolean */
                        return json_value_new(JSON_VALUE_BOOLEAN, "0");

                case JSON_TOKEN_NULL:
                        /* Null */
                        return json_value_new(JSON_VALUE_NULL, NULL);

                default:
                        /* Anything else is an error; this is not the 
                         * start of a value. */

                        return NULL;
        }
}

