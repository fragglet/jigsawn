
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

#ifndef JIGSAWN_INTERNAL_VALUE_H
#define JIGSAWN_INTERNAL_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jigsawn/value.h"
#include "parser.h"

typedef struct _JSONValueClass JSONValueClass;

struct _JSONValueClass {

        /** Type of this class. */

        JSONValueType value_type;

        /**
         * Initialise a new value.
         *
         * @param value              The value to initialise.
         * @param data               String data to initialise the value.
         */

        void (*init)(JSONValue *value, const char *data);

        /**
         * Query whether more values can be read from an array or object.
         *
         * @param value              The array or object.
         * @return                   Zero if the end of the array or object
         *                           has been reached, or non-zero if the
         *                           end has not been reached.
         */

        int (*has_more)(JSONValue *value);

        /**
         * Read the next value from an array or object.
         *
         * @param value              The array or object to read from.
         * @return                   Pointer to the next value read, or NULL if
         *                           the end of the array or object has been
         *                           reached, or an error occurred.
         *                           If this is an object, the values
         *                           returned are of type
         *                           @ref JSON_VALUE_MAPPING.
         */

        JSONValue *(*read_next)(JSONValue *value);
};

struct _JSONValue {

        /** Value type */

        JSONValueClass *value_class;
        
        /** The parser that this value was read from */

        JSONParser *parser;

        /** Value-specific data. */

        union {
                /** Structure used for arrays and object. */

                struct {
                        /** 
                         * Non-zero if we have read to the end of the
                         * structure. 
                         */
                        int reached_end;
                } collection;

                /** Structure used for mappings (@ref JSON_VALUE_MAPPING) */

                struct {
                        /** String used for key */

                        const char *key;

                        /** Value */

                        JSONValue *value;
                } mapping;

                /** For strings (@ref JSON_VALUE_STRING) */

                const char *strval;

                /** For integers (@ref JSON_VALUE_INT) */

                int intval;

                /** For floating point numbers (@ref JSON_VALUE_FLOAT) */

                double floatval;

                /** For boolean values (@ref JSON_VALUE_BOOLEAN) */

                int boolval;
        } data;
};

/**
 * Allocate a new @ref JSONValue of the specified type.
 *
 * @param type               @ref JSONValueType of the new value.
 * @param data               Extra string data to initialise the new value.
 * @return                   Pointer to a new @ref JSONValue, or NULL if
 *                           out of memory.
 */

JSONValue *json_value_new(JSONValueType value_type, const char *data);

/**
 * Free a @ref JSONValue structure.
 *
 * @param value              The value to free.
 */

void json_value_free(JSONValue *value);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef JIGSAWN_INTERNAL_VALUE_H */

