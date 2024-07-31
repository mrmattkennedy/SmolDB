#ifndef COMPRESS_COMMON
#define COMPRESS_COMMON

#define MAX_DATA 64
#define MAX_ROWS 100000
#define DICT_SIZE 110000

// Create a struct that mimics a record in a db
struct Record
{
    int id;
    char name[MAX_DATA];
    char email[MAX_DATA];
};

#endif