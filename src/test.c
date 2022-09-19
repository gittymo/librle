/*	test.c
		A small test program for librle
		(C)2022 Morgan Evans */

#include "librle.h"

int main(int argc, char ** argv)
{
	char * text = "ababbbbbbccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccadddeeffff";
	RLE_DATA * rle_data = RLE_Compress(text,strlen(text));
	if (rle_data) {
		printf("Compressed the text '%s' (%lu bytes) into %i blocks (%i bytes):\n", text, strlen(text), rle_data->size, RLE_CompressedSize(rle_data));
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
		RLE_Save(rle_data, "test.rle");
		free(rle_data);

		rle_data = RLE_Load("test.rle");
		if (rle_data) {
			int32_t data_size = 0;
			uint8_t * data = RLE_Decompress(rle_data, &data_size);
			printf("Decompressed data loaded from file is '%s'.\n", data);
		} else {
			printf("Failed to load RLE data from disk.\n");
		}
	} else printf("Failed to create RLE data from input source.\n");
	exit(EXIT_SUCCESS);
}