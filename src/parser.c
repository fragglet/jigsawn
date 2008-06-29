
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

#include "jigsawn/parser.h"

#include "lexer.h"

struct _JSONParser {
        JSONLexer *lexer;
};

JSONParser *json_parser_new(JSONInputSource *source, 
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

