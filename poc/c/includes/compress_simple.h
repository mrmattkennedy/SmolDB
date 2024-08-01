#ifndef COMPRESS_INCLUDED
#define COMPRESS_INCLUDED
#include "common.h"

// Create a struct that mimics a record in a db
int compress_simple(struct Record *data, size_t dataSize, short nThreads, short cLevel);

#endif