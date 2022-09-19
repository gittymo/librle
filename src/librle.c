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

int8_t * RLE_Decompress(const RLE_DATA * rle_data, int32_t * size_ref)
{
	uint8_t * uncompressed_data = NULL;
	if (size_ref) *size_ref = 0;
	bool error = false;
	if (rle_data && rle_data->blocks && rle_data->size > 0) {
		// Get the uncompressed data size.
		uint32_t uncompressed_data_size = 0;
		for (int32_t i = 0; i < rle_data->size; i++) uncompressed_data_size += rle_data->blocks[i] ? (rle_data->blocks[i]->flag & 254) >> 1 : 0;
		// Try to reserve enough memory for the uncompressed data.
		uncompressed_data = (uint8_t *) malloc(sizeof(uint8_t) * uncompressed_data_size);
		if (uncompressed_data) {
			int32_t o = 0;
			for (int32_t i = 0; i < rle_data->size && !error; i++) {
				if (rle_data->blocks[i]) {
					const int32_t char_count = (rle_data->blocks[i]->flag & 254) >> 1;
					switch (rle_data->blocks[i]->flag & 1) {
						case 0 : {
							memcpy(uncompressed_data + o, rle_data->blocks[i]->data, char_count); 
							o += char_count;
						} break;
						default: {
							for (int32_t j = 0; j < char_count; j++) uncompressed_data[o++] = rle_data->blocks[i]->data[0];
						}
					}
				} else error = true;
			}
			if (!error) *size_ref = uncompressed_data_size;
		}
	} else error = true;

	if (error) {
		if (uncompressed_data) {
			free(uncompressed_data);
			uncompressed_data = NULL;
		}
	}
	return uncompressed_data;
}

void RLE_Save(const RLE_DATA * rle_data, const char * filename)
{
	if (rle_data && rle_data->blocks && rle_data->size > 0 && filename && filename[0] > 0) {
		FILE * outfile = fopen(filename, "wb");
		if (outfile) {
			// Write how many blocks are used for the compressed data.
			fwrite(&rle_data->size, sizeof(int32_t), 1, outfile);
			// Write each block to file.
			for (int32_t i = 0; i < rle_data->size; i++) {
				if (rle_data->blocks[i]) {
					// Write the block header.
					fwrite(&rle_data->blocks[i]->flag, sizeof(uint8_t), 1, outfile);
					switch (rle_data->blocks[i]->flag & 1) {
						case 0 : {
							const uint32_t block_size = (rle_data->blocks[i]->flag & 254) >> 1;
							fwrite(rle_data->blocks[i]->data, sizeof(uint8_t), block_size, outfile);
						} break;
						default : {
							fwrite(rle_data->blocks[i]->data, sizeof(uint8_t), 1, outfile);
						}
					}
				}
			}
			fclose(outfile);
		}
	}
}

RLE_DATA * RLE_Load(const char * filename)
{
	RLE_DATA * rle_data = (RLE_DATA *) malloc(sizeof(RLE_DATA));
	bool error = false;
	if (filename && filename[0] > 0 && rle_data) {
		FILE * infile = fopen(filename, "rb");
		if (infile) {
			uint8_t flag = 0;
			fread(&rle_data->size, sizeof(int32_t), 1, infile);
			if (rle_data->size > 0) {
				rle_data->blocks = (_RLE_BLOCK **) malloc(sizeof(_RLE_BLOCK *) * rle_data->size);
				if (rle_data->blocks) {
					rle_data->_capacity = rle_data->size;
					for (int32_t i = 0; i < rle_data->size && !error; i++) {
						fread(&flag, sizeof(uint8_t), 1, infile);
						rle_data->blocks[i]->flag = flag;
						const uint32_t block_size = (flag & 254) >> 1;
						switch (flag & 1) {
							case 0 : {
								rle_data->blocks[i]->data = (uint8_t *) malloc(sizeof(uint8_t) * block_size);
								if (rle_data->blocks[i]->data) {
									fread(rle_data->blocks[i]->data, sizeof(uint8_t), block_size, infile);
								} else error = true;
							} break;
							default : {
								rle_data->blocks[i]->data = (uint8_t *) malloc(sizeof(uint8_t));
								fread(rle_data->blocks[i]->data, sizeof(uint8_t), 1, infile);
							}
						}
					}
				} else {
					rle_data->size = rle_data->_capacity = 0;
					error = true;
				}
			} else error = true;
		} else error = true;
	}

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