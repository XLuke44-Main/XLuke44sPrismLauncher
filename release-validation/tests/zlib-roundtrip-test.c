#include <stdio.h>
#include <string.h>
#include "zlib.h"

int main(void)
{
    static const char input[] = "XLuke44 PrismLauncher 1.0.3 zlib compression smoke test";
    unsigned char compressed[256];
    unsigned char output[256];
    uLongf compressed_size = sizeof(compressed);
    uLongf output_size = sizeof(output);
    const char *path = "/tmp/xluke44-prism-zlib-roundtrip.gz";
    gzFile file;
    int written;
    int read_count;

    if (compress(compressed, &compressed_size,
                 (const Bytef *)input, (uLong)(sizeof(input) - 1)) != Z_OK)
        return 1;
    if (uncompress(output, &output_size, compressed, compressed_size) != Z_OK)
        return 2;
    if (output_size != sizeof(input) - 1 || memcmp(output, input, output_size) != 0)
        return 3;

    file = gzopen(path, "wb");
    if (file == NULL)
        return 4;
    written = gzwrite(file, input, (unsigned)(sizeof(input) - 1));
    if (written != (int)(sizeof(input) - 1) || gzclose(file) != Z_OK)
        return 5;

    file = gzopen(path, "rb");
    if (file == NULL)
        return 6;
    read_count = gzread(file, output, (unsigned)sizeof(output));
    if (gzclose(file) != Z_OK)
        return 7;
    remove(path);
    if (read_count != (int)(sizeof(input) - 1) || memcmp(output, input, read_count) != 0)
        return 8;

    puts("PASS: compress/uncompress and gzwrite/gzread round-trips");
    return 0;
}
