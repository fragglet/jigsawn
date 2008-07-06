
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

#include <string.h>
#include "value.h"

static void json_mapping_init(JSONValue *value, const char *data)
{
        value->data.mapping.key = strdup(data);
}

const char *json_mapping_get_key(JSONValue *value)
{
        return value->data.mapping.key;
}

JSONValue *json_mapping_get_value(JSONValue *value)
{
        /* TODO */

        return NULL;
}

/* Value class for JSON_VALUE_MAPPING. */

JSONValueClass json_class_mapping = {
        JSON_VALUE_MAPPING,
        json_mapping_init,          /* init */
        NULL,                       /* has_more */
        NULL,                       /* read_next */
};

