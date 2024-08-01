#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "compress_using_dict.h"
#include "compress_simple.h"


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
        str[size-1] = '\0';
    }
    return str;
}

// Write data to train_data struct
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
        int str1_size = rand() % (MAX_DATA-3 + 1 - min) + min;
        int str2_size = rand() % (MAX_DATA-3 + 1 - min) + min;
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
        // Set data in train_data
        write_to_array(array, i, s1, s2);

        // Free s1 and s2
        free(s1);
        free(s2);
    }

    // Print item 5000 to see
    // printf("%d, %s, %s\n", array[5000].id, array[5000].name, array[5000].email);
}


int main(int argc, char** argv) {
    // Create struct of a pseudo database with MAX_ROWS rows
    size_t data_size = MAX_ROWS * sizeof(struct Record);
    struct Record *train_data = (struct Record*)malloc(data_size);
    struct Record *test_data = (struct Record*)malloc(data_size);

    // Fill with random data
    create_random_data(train_data);
    create_random_data(test_data);

    // Compress with dictionary
    create_train_dictionary(train_data);
    free(train_data);

    // Get nThreads and compression level
    short nThreads = 0;
    short cLevel = 1;
    if (argc == 3) {
        nThreads = atoi(argv[1]);
        cLevel = atoi(argv[2]);
    }

    // Time using threads
    clock_t begin = clock();
    compress_using_dictionary(test_data, data_size, nThreads, cLevel);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%f\n", time_spent);

    // Compress normal
    begin = clock();
    compress_simple(test_data, data_size, nThreads, cLevel);
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%f\n", time_spent);
    free(test_data);
    return 0;
}