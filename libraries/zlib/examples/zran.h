/* zran.h -- example of zlib/gzip stream indexing and random access
 * Copyright (C) 2005, 2012, 2018 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 * Version 1.2  14 Oct 2018  Mark Adler */

#include <stdio.h>
#include "zlib.h"

struct deflate_index {
    int have;            
    int gzip;            

    off_t length;        
    void *list;          
};

int deflate_index_build(FILE *in, off_t span, struct deflate_index **built);

void deflate_index_free(struct deflate_index *index);

int deflate_index_extract(FILE *in, struct deflate_index *index, off_t offset,
                          unsigned char *buf, int len);
