/* zran.c -- example of zlib/gzip stream indexing and random access
 * Copyright (C) 2005, 2012, 2018 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 * Version 1.2  14 Oct 2018  Mark Adler */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zlib.h"
#include "zran.h"

#define WINSIZE 32768U       
#define CHUNK 16384          

struct point {
    off_t out;           
    off_t in;            
    int bits;            
    unsigned char window[WINSIZE];   
};

void deflate_index_free(struct deflate_index *index)
{
    if (index != NULL) {
        free(index->list);
        free(index);
    }
}

static struct deflate_index *addpoint(struct deflate_index *index, int bits,
                                      off_t in, off_t out, unsigned left,
                                      unsigned char *window)
{
    struct point *next;

    if (index == NULL) {
        index = malloc(sizeof(struct deflate_index));
        if (index == NULL) return NULL;
        index->list = malloc(sizeof(struct point) << 3);
        if (index->list == NULL) {
            free(index);
            return NULL;
        }
        index->gzip = 8;
        index->have = 0;
    }

    else if (index->have == index->gzip) {
        index->gzip <<= 1;
        next = realloc(index->list, sizeof(struct point) * index->gzip);
        if (next == NULL) {
            deflate_index_free(index);
            return NULL;
        }
        index->list = next;
    }

    next = (struct point *)(index->list) + index->have;
    next->bits = bits;
    next->in = in;
    next->out = out;
    if (left)
        memcpy(next->window, window + WINSIZE - left, left);
    if (left < WINSIZE)
        memcpy(next->window + left, window, WINSIZE - left);
    index->have++;

    return index;
}

int deflate_index_build(FILE *in, off_t span, struct deflate_index **built)
{
    int ret;
    int gzip = 0;                
    off_t totin, totout;         
    off_t last;                  
    struct deflate_index *index;     
    z_stream strm;
    unsigned char input[CHUNK];
    unsigned char window[WINSIZE];

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, 47);       
    if (ret != Z_OK)
        return ret;

    totin = totout = last = 0;
    index = NULL;                
    strm.avail_out = 0;
    do {

        strm.avail_in = fread(input, 1, CHUNK, in);
        if (ferror(in)) {
            ret = Z_ERRNO;
            goto deflate_index_build_error;
        }
        if (strm.avail_in == 0) {
            ret = Z_DATA_ERROR;
            goto deflate_index_build_error;
        }
        strm.next_in = input;

        if (totin == 0 && strm.avail_in >= 3 &&
            input[0] == 31 && input[1] == 139 && input[2] == 8)
            gzip = 1;

        do {

            if (strm.avail_out == 0) {
                strm.avail_out = WINSIZE;
                strm.next_out = window;
            }

            totin += strm.avail_in;
            totout += strm.avail_out;
            ret = inflate(&strm, Z_BLOCK);       
            totin -= strm.avail_in;
            totout -= strm.avail_out;
            if (ret == Z_NEED_DICT)
                ret = Z_DATA_ERROR;
            if (ret == Z_MEM_ERROR || ret == Z_DATA_ERROR)
                goto deflate_index_build_error;
            if (ret == Z_STREAM_END) {
                if (gzip &&
                    (strm.avail_in || ungetc(getc(in), in) != EOF)) {
                    ret = inflateReset(&strm);
                    if (ret != Z_OK)
                        goto deflate_index_build_error;
                    continue;
                }
                break;
            }

            if ((strm.data_type & 128) && !(strm.data_type & 64) &&
                (totout == 0 || totout - last > span)) {
                index = addpoint(index, strm.data_type & 7, totin,
                                 totout, strm.avail_out, window);
                if (index == NULL) {
                    ret = Z_MEM_ERROR;
                    goto deflate_index_build_error;
                }
                last = totout;
            }
        } while (strm.avail_in != 0);
    } while (ret != Z_STREAM_END);

    (void)inflateEnd(&strm);
    index->list = realloc(index->list, sizeof(struct point) * index->have);
    index->gzip = gzip;
    index->length = totout;
    *built = index;
    return index->have;

  deflate_index_build_error:
    (void)inflateEnd(&strm);
    deflate_index_free(index);
    return ret;
}

int deflate_index_extract(FILE *in, struct deflate_index *index, off_t offset,
                          unsigned char *buf, int len)
{
    int ret, skip;
    z_stream strm;
    struct point *here;
    unsigned char input[CHUNK];
    unsigned char discard[WINSIZE];

    if (len < 0)
        return 0;

    here = index->list;
    ret = index->have;
    while (--ret && here[1].out <= offset)
        here++;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, -15);          
    if (ret != Z_OK)
        return ret;
    ret = fseeko(in, here->in - (here->bits ? 1 : 0), SEEK_SET);
    if (ret == -1)
        goto deflate_index_extract_ret;
    if (here->bits) {
        ret = getc(in);
        if (ret == -1) {
            ret = ferror(in) ? Z_ERRNO : Z_DATA_ERROR;
            goto deflate_index_extract_ret;
        }
        (void)inflatePrime(&strm, here->bits, ret >> (8 - here->bits));
    }
    (void)inflateSetDictionary(&strm, here->window, WINSIZE);

    offset -= here->out;
    strm.avail_in = 0;
    skip = 1;                                
    do {

        if (offset > WINSIZE) {              
            strm.avail_out = WINSIZE;
            strm.next_out = discard;
            offset -= WINSIZE;
        }
        else if (offset > 0) {               
            strm.avail_out = (unsigned)offset;
            strm.next_out = discard;
            offset = 0;
        }
        else if (skip) {                     
            strm.avail_out = len;
            strm.next_out = buf;
            skip = 0;                        
        }

        do {
            if (strm.avail_in == 0) {
                strm.avail_in = fread(input, 1, CHUNK, in);
                if (ferror(in)) {
                    ret = Z_ERRNO;
                    goto deflate_index_extract_ret;
                }
                if (strm.avail_in == 0) {
                    ret = Z_DATA_ERROR;
                    goto deflate_index_extract_ret;
                }
                strm.next_in = input;
            }
            ret = inflate(&strm, Z_NO_FLUSH);        
            if (ret == Z_NEED_DICT)
                ret = Z_DATA_ERROR;
            if (ret == Z_MEM_ERROR || ret == Z_DATA_ERROR)
                goto deflate_index_extract_ret;
            if (ret == Z_STREAM_END) {

                if (index->gzip == 0)

                    break;

                if (strm.avail_in < 8) {
                    fseeko(in, 8 - strm.avail_in, SEEK_CUR);
                    strm.avail_in = 0;
                }
                else {
                    strm.avail_in -= 8;
                    strm.next_in += 8;
                }
                if (strm.avail_in == 0 && ungetc(getc(in), in) == EOF)

                    break;

                ret = inflateReset2(&strm, 31);
                if (ret != Z_OK)
                    goto deflate_index_extract_ret;
                do {
                    if (strm.avail_in == 0) {
                        strm.avail_in = fread(input, 1, CHUNK, in);
                        if (ferror(in)) {
                            ret = Z_ERRNO;
                            goto deflate_index_extract_ret;
                        }
                        if (strm.avail_in == 0) {
                            ret = Z_DATA_ERROR;
                            goto deflate_index_extract_ret;
                        }
                        strm.next_in = input;
                    }
                    ret = inflate(&strm, Z_BLOCK);
                    if (ret == Z_MEM_ERROR || ret == Z_DATA_ERROR)
                        goto deflate_index_extract_ret;
                } while ((strm.data_type & 128) == 0);

                ret = inflateReset2(&strm, -15);
                if (ret != Z_OK)
                    goto deflate_index_extract_ret;
            }

        } while (strm.avail_out != 0);

        if (ret == Z_STREAM_END)

            break;

    } while (skip);

    ret = skip ? 0 : len - strm.avail_out;

  deflate_index_extract_ret:
    (void)inflateEnd(&strm);
    return ret;
}

#ifdef TEST

#define SPAN 1048576L        
#define LEN 16384            

int main(int argc, char **argv)
{
    int len;
    off_t offset = -1;
    FILE *in;
    struct deflate_index *index = NULL;
    unsigned char buf[LEN];

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "usage: zran file.gz [offset]\n");
        return 1;
    }
    in = fopen(argv[1], "rb");
    if (in == NULL) {
        fprintf(stderr, "zran: could not open %s for reading\n", argv[1]);
        return 1;
    }

    if (argc == 3) {
        char *end;
        offset = strtoll(argv[2], &end, 10);
        if (*end || offset < 0) {
            fprintf(stderr, "zran: %s is not a valid offset\n", argv[2]);
            return 1;
        }
    }

    len = deflate_index_build(in, SPAN, &index);
    if (len < 0) {
        fclose(in);
        switch (len) {
        case Z_MEM_ERROR:
            fprintf(stderr, "zran: out of memory\n");
            break;
        case Z_DATA_ERROR:
            fprintf(stderr, "zran: compressed data error in %s\n", argv[1]);
            break;
        case Z_ERRNO:
            fprintf(stderr, "zran: read error on %s\n", argv[1]);
            break;
        default:
            fprintf(stderr, "zran: error %d while building index\n", len);
        }
        return 1;
    }
    fprintf(stderr, "zran: built index with %d access points\n", len);

    if (offset == -1)
        offset = (index->length << 1) / 3;
    len = deflate_index_extract(in, index, offset, buf, LEN);
    if (len < 0)
        fprintf(stderr, "zran: extraction failed: %s error\n",
                len == Z_MEM_ERROR ? "out of memory" : "input corrupted");
    else {
        fwrite(buf, 1, len, stdout);
        fprintf(stderr, "zran: extracted %d bytes at %llu\n", len, offset);
    }

    deflate_index_free(index);
    fclose(in);
    return 0;
}

#endif
