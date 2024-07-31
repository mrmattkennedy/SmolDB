#include <zstd.h>
#include <zdict.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_DATA 64
#define MAX_ROWS 10000
#define DICT_SIZE 110000 // Size of the dictionary

// Create a struct that mimics a record in a dict_train_data
struct Record
{
    int id;
    char name[MAX_DATA];
    char email[MAX_DATA];
};

void die(const char *message)
{
    if (errno)
    {
        perror(message);
    }
    else
    {
        printf("ERROR: %s\n", message);
    };
    exit(1);
}

static char *rand_string(char *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

// Write data to dict_train_data struct
void write_to_array(struct Record *array,  int id, char *s1, char *s2) {
    array[id].id = id;

    char *res = strncpy(array[id].name, s1, MAX_DATA);
    // demonstrate the strncpy bug
    if (!res)
        die("Name copy failed!");

    res = strncpy(array[id].email, s2, MAX_DATA);
    if (!res)
        die("Email copy failed!");
}

void create_random_data(struct Record *array) 
{
    // Fill the struct with random data
    int min = 3;
    for (int i = 0; i < MAX_ROWS; i++) {
        // Define random string size
        int str1_size = rand() % (MAX_DATA + 1 - min) + min;
        int str2_size = rand() % (MAX_DATA + 1 - min) + min;
        // Create s1 and s2 for 2 varchar fields
        char *s1 = malloc(str1_size + 1);
        char *s2 = malloc(str2_size + 1);

        // Create random string and put into s1
        if (s1) {
            rand_string(s1, str1_size);
        }
        // Create random string and put into s2
        if (s2) {
            rand_string(s2, str2_size);
        }
        // Set data in dict_train_data
        write_to_array(array, i, s1, s2);

        // Free s1 and s2
        free(s1);
        free(s2);
    }

    // Print item 5000 to see
    printf("%d, %s, %s\n", array[5000].id, array[5000].name, array[5000].email);
}


// Compress array using dictionary
int compress(struct Record *data)
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
    
    // Get size of data to compress
    size_t data_size = sizeof(data);

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
    size_t cBuffSize = ZSTD_compressBound(sizeof(data));
    void* compressed_data = malloc(cBuffSize);
    if (!compressed_data) {
        fprintf(stderr, "Failed to allocate memory for compressed data\n");
        ZSTD_freeCDict(cdict);
        ZSTD_freeCCtx(cctx);
        return 1;
    }

    // Compress the data using the dictionary
    size_t compressed_size = ZSTD_compress_usingCDict(cctx, compressed_data, cBuffSize, data, data_size, cdict);
    if (ZSTD_isError(compressed_size)) {
        fprintf(stderr, "Compression failed: %s\n", ZSTD_getErrorName(compressed_size));
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
    fwrite(compressed_data, 1, compressed_size, cFile);
    fclose(dict_file);

    printf("Compression successful!\nCompressed size: %zu\n", sizeof(compressed_data));

    // Clean up
    free(compressed_data);
    ZSTD_freeCDict(cdict);
    ZSTD_freeCCtx(cctx);

    return 0;
}

int main() {
    // Create struct of a pseudo database with MAX_ROWS rows
    struct Record dict_train_data[MAX_ROWS];
    struct Record test_data[MAX_ROWS];

    // Fill with random data
    create_random_data(dict_train_data);
    create_random_data(test_data);

    // Get sample sizes for trainFromBuffer
    size_t sample_sizes[MAX_ROWS];
    for (int i = 0; i < MAX_ROWS; ++i) {
        sample_sizes[i] = sizeof(dict_train_data[i]);
    }

    // Train the dictionary
    void* dict_buffer = malloc(DICT_SIZE);
    size_t dict_size = ZDICT_trainFromBuffer(dict_buffer, DICT_SIZE, dict_train_data, sample_sizes, MAX_DATA);
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

    // Compress
    compress(dict_train_data);

    printf("Normal size: %zu\n", sizeof(test_data));
    return 0;
}