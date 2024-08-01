#ifndef COMPRESS_DICT_H_INCLUDED
#define COMPRESS_DICT_H_INCLUDED
#include "common.h"

int create_train_dictionary(struct Record *data);
int compress_using_dictionary(struct Record *data, size_t dataSize, short nThreads, short cLevel);

#endif