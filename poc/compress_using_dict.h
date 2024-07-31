#ifndef COMPRESS_DICT_H_INCLUDED
#define COMPRESS_DICT_H_INCLUDED
#define MAX_DATA 64
#define MAX_ROWS 10000
#define DICT_SIZE 110000

// Create a struct that mimics a record in a db
struct Record
{
    int id;
    char name[MAX_DATA];
    char email[MAX_DATA];
};

int create_train_dictionary(struct Record *data);
int compress_using_dictionary(struct Record *data, size_t dataSize);


#endif