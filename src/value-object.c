
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

#include "value.h"

static void json_object_init(JSONValue *value, const char *data)
{
        value->data.collection.reached_end = 0;
}

static int json_object_has_more(JSONValue *value)
{
        /* TODO */
        return 1;
}

static JSONValue *json_object_read_next(JSONValue *value)
{
        /* TODO */
        return NULL;
}

/* Value class for JSON_VALUE_OBJECT. */

JSONValueClass json_class_object = {
        JSON_VALUE_OBJECT,
        json_object_init,           /* init */
        json_object_has_more,       /* has_more */
        json_object_read_next,      /* read_next */
};

