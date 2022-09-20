/*	test.c
		A small test program for librle
		(C)2022 Morgan Evans */

#include "librle.h"

int main(int argc, char ** argv)
{
	// Create a bit of text to compress.
	char * text = "ababbbbbbccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccadddeeffff";

	// Compress the text and store a reference to the compressed data structure in the variable 'rle_data'.
	RLE_DATA * rle_data = RLE_Compress(text,strlen(text));

	// Make sure we have a valid pointer (if it's NULL, the compression operation didn't work).
	if (rle_data) {
		// We do, so output various information about the text and the compressed data.
		printf("Compressed the text '%s' (%lu bytes) into %i blocks (%i bytes):\n", text, strlen(text), rle_data->size, RLE_CompressedSize(rle_data));
		// Show a break-down of each section of the compressed data.
		for (int32_t i = 0; i < rle_data->size; i++) {
			switch (rle_data->blocks[i]->flag & 1) {
				case 0 : {
					printf("\tRAW block of %d chars '%s'.\n", (rle_data->blocks[i]->flag & 254) >> 1, rle_data->blocks[i]->data);
				} break;
				default : {
					printf("\tRLE block of %d copies of the character '%c'.\n", (rle_data->blocks[i]->flag & 254) >> 1, rle_data->blocks[i]->data[0]);
				}
			}
		}
		// Save the compressed data to a file on disk.
		RLE_Save(rle_data, "test.rle");
		// Free any memory allocated to the compressed data.
		RLE_Free(rle_data);
	} else {
			// We don't have a valid pointer, so report the error.
			printf("Failed to create RLE data from input source.\n");
	}

	// Create a new RLE_DATA structure based on the date in the file that was saved to disk.
	rle_data = RLE_Load("test.rle");
	// Make sure we've got a valid pointer (if it's NULL, the load operation didn't work).
	if (rle_data) {
		// We do have a valid pointer, so decompress the data.
		int32_t data_size = 0;
		uint8_t * data = RLE_Decompress(rle_data, &data_size);
		// Show the decompressed data (which is the text originally in the variable 'text').
		printf("Decompressed data loaded from file is '%s'.\n", data);
		// Release any memory allocated to the RLE_DATA structure.
		RLE_Free(rle_data);
	} else {
		// We don't have a valid pointer, so report the error.
		printf("Failed to load RLE data from disk.\n");
	}

	// Exit the application.
	exit(EXIT_SUCCESS);
}