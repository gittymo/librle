/*	librle.h
		A (hopefully) small library that provides RLE compression.
		(C)2022 Morgan Evans */

#ifndef COM_PLUS_MEVANSPN_LIBRLE
#define COM_PLUS_MEVANSPN_LIBRLE

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define RLE_DEFAULT_BLOCK_COUNT 256

/*	_RLE_BLOCK structure:
	_RLE_BLOCK structures are designed to be private and should only be used internally by the library.
	Each _RLE_BLOCK structure is an individual entry stored within the (blocks) array of the RLE_DATA structure, and represents a part, either raw 
	(uncompressed, non-repeating) or compressed (repeating) characters.  In order to determine which type of data an _RLE_BLOCK represents, the (flag)
	member of this structure must be intepreted using the following guidance:
	(flag) - 	Is used to indicate the type and length of the compressed block.  The least significant bit of this value (bit 1) gives the type 
						(0 = raw, 1 = rle) and bits 2 to 8 give the length as an offset of 1 from (0-127) (i.e. length 24 = 25 characters).
	(data) - 	Depending on the value of the first bit of (flag), this member will contain an array of one or more characters.
						If (flag = 0), the data is raw, so data will point to an array of characters, the length of which is given by (flag) bits 2 to 8.
						If (flag = 1), the array will only hold one character value, the number of these is appears is given by (flag) bits 2 to 8. */
typedef struct {
	uint8_t flag;
	char * data;
} _RLE_BLOCK;

/*	RLE_DATA structure:
		This structure contains an array (blocks) of _RLE_BLOCK pointers (see _RLE_BLOCK above), which record the data passed to the RLE_Compress method as a 
		sequence of raw (uncompressed, non-repeating) and run-length encoded (repeating) characters.  The structure also has two other members:
		(_capacity) is used internally by the library and records the current capacity of the blocks array.  This value will be increased when the value of
								(size) = (_capacity).
		(size) 			determines the actual number of RLE_BLOCKs that have been created when compressing the data passed by RLE_Compress.*/
typedef struct {
	_RLE_BLOCK ** blocks;
	int32_t _capacity;
	int32_t size;
} RLE_DATA;

/* 	PUBLIC METHODS */
/*	RLE_Compress returns a pointer to an RLE_DATA structure.  See the notes for RLE_DATA above as to how the original data is stored.
		RLE_Compress takes two parameters, a pointer to the source data (data) and the length of the source data (data_length).
		If RLE_Compress fails to compress the data, a NULL pointer will be returned. */
RLE_DATA * RLE_Compress(const uint8_t * data, int32_t data_length);

/*	RLE_Decompress returns a block of uncompressed data when passed a pointer to an RLE_DATA structure (rle_data) and a reference to an int variable (size_ref).
		(rle_data) must be a pointer to a valid RLE_DATA data structure.
		(size_ref) must be a pointer to an 32 integer variable, which is usually defined by the stardard keyword 'int' but can be guaranteed by using a type of 'int32_t'. */
int8_t * RLE_Decompress(const RLE_DATA * rle_data, int32_t * size_ref);

/*	RLE_Save will attempt to save a valid compressed RLE_DATA data structure to disk, given a pointer to the data structure (rle_data) and a pointer to a string giving 
		the file path (filename). */
void RLE_Save(const RLE_DATA * rle_data, const char * filename);

/*	RLE_Load will attempt to load compressed RLE_DATA stored in the file given by path (filename) into memory.  If the load fails, the method 
		will return a NULL pointer. */
RLE_DATA * RLE_Load(const char * filename);

/*	Calling RLE_CompressedSize whilst passing a valid RLE_DATA structure will return the length of the compressed data in bytes. */
int32_t RLE_CompressedSize(const RLE_DATA * rle_data);

/* PRIVATE METHODS USED INTERNALLY BY THIS LIBRARY, DO NOT USE UNLESS YOU KNOW WHAT YOU'RE DOING! */
void _RLE_FreeBlock(_RLE_BLOCK * rle_block);

#endif