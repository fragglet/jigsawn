
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

#ifndef JIGSAWN_UTF8_H
#define JIGSAWN_UTF8_H

#ifdef __cplusplus
extern "C" {
#endif

/* Start of a 1 byte sequence:      0UTF8xxx */

#define JSON_UTF8_START_1  0x00      
#define JSON_UTF8_MASK_1   0x80

/* Start of a 2 byte sequence:      110UTF8x */

#define JSON_UTF8_START_2  0xc0
#define JSON_UTF8_MASK_2   0xe0

/* Start of a 3 byte sequence:      1110UTF8 */

#define JSON_UTF8_START_3  0xe0
#define JSON_UTF8_MASK_3   0xf0

/* Start of a 4 byte sequence:      11110xxx */

#define JSON_UTF8_START_4  0xf0
#define JSON_UTF8_MASK_4   0xf8

/* A byte following a start byte:   10UTF8xx */

#define JSON_UTF8_SEQ_BYTE 0x80
#define JSON_UTF8_SEQ_MASK 0xc0

#ifdef __cplusplus
}
#endif

#endif /* #ifndef JIGSAWN_UTF8_H */

