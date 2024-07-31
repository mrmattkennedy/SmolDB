#include <zstd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "compress_simple.h"

// Compress array using dictionary
int compress_simple(struct Record *data, size_t dataSize)
{
    // Allocate buffer for compressed data
    size_t cBuffSize = ZSTD_compressBound(dataSize);
    void* cData = malloc(cBuffSize);
    if (!cData) {
        fprintf(stderr, "Failed to allocate memory for compressed data\n");
        return 1;
    }

    // Compress the data using the dictionary
    size_t const cSize = ZSTD_compress(cData, cBuffSize, data, dataSize, 1);
    if (ZSTD_isError(cSize)) {
        fprintf(stderr, "Compression failed: %s\n", ZSTD_getErrorName(cSize));
        free(cData);
        return 1;
    }

    // Write to file
    FILE* cFile = fopen("compressed2.zstd", "wb");
    if (cFile == NULL) {
        fprintf(stderr, "Failed to open compression file for writing\n");
        free(cData);
        return 1;
    }
    fwrite(cData, 1, cSize, cFile);

    // Write out the original size, new size, and what % thaat is
    printf("Compressing (simple):\t%6u -> %7u, %.2f%%\n", dataSize, (unsigned)cSize, (cSize * 100.0) / dataSize);

    // Clean up
    free(cData);
    fclose(cFile);

    return 0;
}