#include <zstd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "compress_simple.h"

// Compress array using dictionary
int compress_simple(struct Record *data, size_t dataSize, short nThreads, short cLevel)
{
    // Allocate buffer for compressed data
    size_t cBuffSize = ZSTD_compressBound(dataSize);
    void* cData = malloc(cBuffSize);
    if (!cData) {
        fprintf(stderr, "Failed to allocate memory for compressed data\n");
        return 1;
    }

    // Create a compression context
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    if (!cctx) {
        fprintf(stderr, "Failed to create compression context\n");
        return 1;
    }

    // Set the number of threads to use
    if (nThreads > 0) {
        size_t result = ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, nThreads);
        if (ZSTD_isError(result)) {
            fprintf(stderr, "Failed to set number of threads: %s\n", ZSTD_getErrorName(result));
            ZSTD_freeCCtx(cctx);
            // Don't return, just continue with single threaded support
        }
    }

    // Set the compression level
    size_t result2 = ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, cLevel);
    if (ZSTD_isError(result2)) {
        fprintf(stderr, "Failed to set compression level: %s\n", ZSTD_getErrorName(result2));
        ZSTD_freeCCtx(cctx);
        return 1;
    }

    // Compress the data using the dictionary
    size_t const cSize = ZSTD_compress2(cctx, cData, cBuffSize, data, dataSize);
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
    printf("Compressing (simple):\t%6u -> %7u, %.2f%%\n", (unsigned int)dataSize, (unsigned)cSize, (cSize * 100.0) / dataSize);

    // Clean up
    free(cData);
    fclose(cFile);

    return 0;
}