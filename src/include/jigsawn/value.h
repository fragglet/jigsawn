
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

#ifndef JIGSAWN_VALUE_H
#define JIGSAWN_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The type of a @ref JSONValue. 
 */

typedef enum {
        JSON_VALUE_NULL,
        JSON_VALUE_ARRAY,
        JSON_VALUE_OBJECT,
        JSON_VALUE_STRING,
        JSON_VALUE_MAPPING,
        JSON_VALUE_INT,
        JSON_VALUE_FLOAT,
        JSON_VALUE_BOOLEAN
} JSONValueType;

/**
 * A value read from an input stream.
 */

typedef struct _JSONValue JSONValue;

/**
 * Get the type of a JSONValue.
 *
 * @param value              The value.
 * @return                   The type of the value.
 */

JSONValueType json_value_get_type(JSONValue *value);

/**
 * Query whether more values can be read from an array or object.
 *
 * @param value              The array or object.
 * @return                   Non-zero if the end of the array or object
 *                           has been reached, or zero if the end has 
 *                           been reached.
 */

int json_value_has_more(JSONValue *value);

/**
 * Read the next value from an array or object.
 *
 * @param value              The array or object to read from.
 * @return                   Pointer to the next value read, or NULL if
 *                           the end of the array or object has been
 *                           reached, or an error occurred.
 *                           If this is an object, the values
 *                           returned are of type @ref JSON_VALUE_MAPPING.
 */

JSONValue *json_value_read_next(JSONValue *value);

/**
 * Get the value of a string @ref JSONValue.
 *
 * @param value              The value.
 * @return                   The string data, encoded in UTF-8 format.
 */

const char *json_string_get_value(JSONValue *value);

/**
 * Get the value of an integer @ref JSONValue (@ref JSON_VALUE_INT).
 *
 * @param value              The value.
 * @return                   The integer value.
 */

int json_int_get_value(JSONValue *value);

/**
 * Get the value of a floating-point @ref JSONValue
 * (@ref JSON_VALUE_FLOAT).
 *
 * @param value              The value.
 * @return                   The floating-point value.
 */

double json_float_get_value(JSONValue *value);

/**
 * Get the value of a boolean @ref JSONValue 
 * (@ref JSON_VALUE_BOOLEAN).
 *
 * @param value              The value.
 * @return                   Non-zero for true, zero for false.
 */

int json_boolean_get_value(JSONValue *value);

/**
 * Get the key for an object mapping (@ref JSONValue of type
 * @ref JSON_VALUE_MAPPING).
 *
 * @param value              The mapping.
 * @return                   Key for the mapping.
 *
 * @sa json_mapping_get_value
 */

const char *json_mapping_get_key(JSONValue *value);

/**
 * Get the value for an object mapping (@ref JSONValue of type
 * @ref JSON_VALUE_MAPPING).
 *
 * @param value              The mapping.
 * @return                   The value that the mapping maps to.
 *
 * @sa json_mapping_get_key
 */

JSONValue *json_mapping_get_value(JSONValue *value);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef JIGSAWN_VALUE_H */

