/*	librle.c
		A small RLE compression library
		©2022 Morgan Evans */

#include "librle.h"

void _RLE_FreeBlock(_RLE_BLOCK * rle_block)
{
	if (rle_block) {
		if (rle_block->data) {
			free(rle_block->data);
			rle_block->data = NULL;
		}
		rle_block->flag = 0;
		free(rle_block);
	}
}

void RLE_Free(RLE_DATA * rle_data)
{
	if (rle_data) {
		if (rle_data->blocks) {
			for (int32_t i = 0; i < rle_data->_capacity; i++) {
				if (rle_data->blocks[i]) {
					_RLE_FreeBlock(rle_data->blocks[i]);
					rle_data->blocks[i] = NULL;
				}
			}
			free(rle_data->blocks);
		}
		rle_data->blocks = NULL;
		rle_data->_capacity = 0;
		free(rle_data);
	}
}

RLE_DATA * RLE_Compress(const uint8_t * data, int32_t data_length)
{
	RLE_DATA * rle_data = (RLE_DATA *) malloc(sizeof(RLE_DATA));
	bool error = false;
	if (data && data_length > 0 && rle_data) {
		rle_data->blocks = (_RLE_BLOCK **) malloc(sizeof(_RLE_BLOCK *) * RLE_DEFAULT_BLOCK_COUNT);
		rle_data->_capacity = RLE_DEFAULT_BLOCK_COUNT;
		rle_data->size = 0;
		if (rle_data->blocks) {
			int32_t i = 0;
			while (i < data_length && !error) {
				_RLE_BLOCK * rle_block = (_RLE_BLOCK *) malloc(sizeof(_RLE_BLOCK));
				if (rle_block) {
					// Get count of mixed characters.
					uint8_t last_char = data[i];
					int32_t j = i + 1;
					while (j < data_length && j < i + 127 && data[j] != last_char) last_char = data[j++];
					uint32_t count_of_mixed_chars = j - i;
					if (j < data_length) count_of_mixed_chars--;
					if (count_of_mixed_chars > 0) {
						rle_block->flag = count_of_mixed_chars << 1;
						rle_block->data = (uint8_t *) malloc(sizeof(uint8_t) * count_of_mixed_chars);
						memcpy(rle_block->data, data + i, count_of_mixed_chars);
						i += count_of_mixed_chars;
					} else {
						// Get count of repeated character.
						while (j < data_length && j < i + 127 && data[j] == last_char) last_char = data[j++];
						int32_t count_of_repeated_chars = j - i;
						rle_block->flag = 1 + (count_of_repeated_chars << 1);
						rle_block->data = (uint8_t *) malloc(sizeof(uint8_t));
						rle_block->data[0] = last_char;
						i = j;
					}
					rle_data->blocks[rle_data->size++] = rle_block;
					if (rle_data->size == rle_data->_capacity) {
						rle_data->_capacity += RLE_DEFAULT_BLOCK_COUNT;
						_RLE_BLOCK ** new_blocks_array = (_RLE_BLOCK **) realloc(rle_data->blocks, sizeof(_RLE_BLOCK *) * rle_data->_capacity);
						if (new_blocks_array) rle_data->blocks = new_blocks_array;
						else {
							rle_data->_capacity -= RLE_DEFAULT_BLOCK_COUNT;
							error = true;
						}
					}
				} else error = true;
			}
		} else error = true;
	} else error = true;

	if (error) {
		RLE_Free(rle_data);
		rle_data = NULL;
	}

	return rle_data;
}

int32_t RLE_CompressedSize(const RLE_DATA * rle_data)
{
	int32_t compressed_size = 0;
	if (rle_data && rle_data->size > 0) {
		for (int32_t i = 0; i < rle_data->size; i++) {
			switch (rle_data->blocks[i]->flag & 1) {
				case 0 : compressed_size += 1 + ((rle_data->blocks[i]->flag & 254) >> 1); break;
				default : compressed_size += 2;
			} 
		}
	}
	return compressed_size;
}