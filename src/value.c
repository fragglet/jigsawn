
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

extern JSONValueClass json_class_null;
extern JSONValueClass json_class_array;
extern JSONValueClass json_class_object;
extern JSONValueClass json_class_string;
extern JSONValueClass json_class_mapping;
extern JSONValueClass json_class_int;
extern JSONValueClass json_class_float;
extern JSONValueClass json_class_boolean;

static JSONValueClass *value_classes[] = {
        &json_class_null,             /* JSON_VALUE_NULL */
        &json_class_array,            /* JSON_VALUE_ARRAY */
        &json_class_object,           /* JSON_VALUE_OBJECT */
        &json_class_string,           /* JSON_VALUE_STRING */
        &json_class_mapping,          /* JSON_VALUE_MAPPING */
        &json_class_int,              /* JSON_VALUE_INT */
        &json_class_float,            /* JSON_VALUE_FLOAT */
        &json_class_boolean,          /* JSON_VALUE_BOOLEAN */
};

JSONValue *json_value_new(JSONValueType value_type, const char *data)
{
        JSONValue *value;
        JSONValueClass *value_class;

        /* Allocate the new value */

        value = malloc(sizeof(*value));

        if (value == NULL) {
                return NULL;
        }

        value_class = value_classes[value_type];

        /* Call initialisation function if required */

        if (value_class->init != NULL) {
                value_class->init(value, data);
        }

        return value;
}

JSONValueType json_value_get_type(JSONValue *value)
{
        return value->value_class->value_type;
}

int json_value_has_more(JSONValue *value)
{
        if (value->value_class->has_more != NULL) {
                return value->value_class->has_more(value);
        } else {
                return 0;
        }
}

JSONValue *json_value_read_next(JSONValue *value)
{
        if (value->value_class->read_next != NULL) {
                return value->value_class->read_next(value);
        } else {
                return 0;
        }
}

