#include <zstd.h>
#include <zdict.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "compress_using_dict.h"

// Create dictionary
int create_train_dictionary(struct Record *data) {
    // Get sample sizes for trainFromBuffer
    size_t sampleSizes[MAX_ROWS];
    for (int i = 0; i < MAX_ROWS; ++i) {
        sampleSizes[i] = sizeof(data[i]);
    }

    // Train the dictionary
    void* dict_buffer = malloc(DICT_SIZE);
    size_t dict_size = ZDICT_trainFromBuffer(dict_buffer, DICT_SIZE, data, sampleSizes, MAX_ROWS);
    if (ZDICT_isError(dict_size)) {
        fprintf(stderr, "ZDICT_trainFromBuffer error: %s\n", ZDICT_getErrorName(dict_size));
        free(dict_buffer);
        return 1;
    }

    // Write the dictionary to a file
    FILE* dict_file = fopen("dictionary.zstd", "wb");
    if (dict_file == NULL) {
        fprintf(stderr, "Failed to open dictionary file for writing\n");
        free(dict_buffer);
        return 1;
    }

    fwrite(dict_buffer, 1, dict_size, dict_file);
    fclose(dict_file);
    free(dict_buffer);

    return 0;
}

// Compress array using dictionary
int compress_using_dictionary(struct Record *data, size_t dataSize)
{
    // Load the dictionary from a file
    FILE* dict_file = fopen("dictionary.zstd", "rb");
    if (!dict_file) {
        fprintf(stderr, "Failed to open dictionary file\n");
        return 1;
    }
    void* dict_buffer = malloc(DICT_SIZE);
    size_t dict_size = fread(dict_buffer, 1, DICT_SIZE, dict_file);
    fclose(dict_file);
    if (dict_size == 0) {
        fprintf(stderr, "Failed to read dictionary\n");
        return 1;
    }

    // Create a compression context
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    if (!cctx) {
        fprintf(stderr, "Failed to create compression context\n");
        free(dict_buffer);
        return 1;
    }

    // Load the dictionary into the compression context
    ZSTD_CDict* cdict = ZSTD_createCDict(dict_buffer, dict_size, 1); // 1 for default compression level
    free(dict_buffer);
    if (!cdict) {
        fprintf(stderr, "Failed to create compression dictionary\n");
        ZSTD_freeCCtx(cctx);
        return 1;
    }

    // Allocate buffer for compressed data
    size_t cBuffSize = ZSTD_compressBound(dataSize);
    void* compressed_data = malloc(cBuffSize);
    printf("%zu\t%zu\n", cBuffSize, dataSize);
    if (!compressed_data) {
        fprintf(stderr, "Failed to allocate memory for compressed data\n");
        ZSTD_freeCDict(cdict);
        ZSTD_freeCCtx(cctx);
        return 1;
    }

    // Compress the data using the dictionary
    size_t const cSize = ZSTD_compress_usingCDict(cctx, compressed_data, cBuffSize, data, dataSize, cdict);
    if (ZSTD_isError(cSize)) {
        fprintf(stderr, "Compression failed: %s\n", ZSTD_getErrorName(cSize));
        free(compressed_data);
        ZSTD_freeCDict(cdict);
        ZSTD_freeCCtx(cctx);
        return 1;
    }

    // Write to file
    FILE* cFile = fopen("compressed.zstd", "wb");
    if (cFile == NULL) {
        fprintf(stderr, "Failed to open compression file for writing\n");
        free(compressed_data);
        ZSTD_freeCDict(cdict);
        ZSTD_freeCCtx(cctx);
        return 1;
    }
    fwrite(compressed_data, 1, cSize, cFile);
    fclose(dict_file);

    // Wi
    printf("%6u -> %7u, %.2f%%\n", dataSize, (unsigned)cSize, (cSize * 100.0) / dataSize);

    // Clean up
    free(compressed_data);
    ZSTD_freeCDict(cdict);
    ZSTD_freeCCtx(cctx);

    return 0;
}