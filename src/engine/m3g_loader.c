#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>

typedef struct {
    uint8_t compression;
    uint32_t total_length;
    uint32_t uncompressed_length;
    uint32_t checksum;
    const uint8_t *objects;
    uint32_t object_bytes;
} M3GSection;

static uint32_t m3g_u32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | p[3];
}

int m3g_read_section(const uint8_t *data, size_t size, M3GSection *out)
{
    if (!data || !out || size < 17) return -1;
    out->compression = data[0];
    out->total_length = m3g_u32(data + 1);
    out->uncompressed_length = m3g_u32(data + 5);

    if (out->total_length < 17 || out->total_length > size) return -2;
    if (out->compression > 1) return -3;

    out->object_bytes = out->total_length - 13 - 4;
    out->objects = data + 9;
    out->checksum = m3g_u32(data + out->total_length - 4);
    return 0;
}

/*
 * Decompress only the object payload. The M3G checksum is Adler32 over
 * the section header plus the compressed object bytes, as specified by
 * JSR-184.
 */
int m3g_unpack_objects(const M3GSection *s, uint8_t *dst, size_t dst_size)
{
    if (!s || !dst) return -1;
    if (s->compression == 0) {
        if (dst_size < s->object_bytes) return -2;
        memcpy(dst, s->objects, s->object_bytes);
        return (int)s->object_bytes;
    }

    uLongf n = (uLongf)dst_size;
    if (s->uncompressed_length > dst_size) return -3;
    if (uncompress(dst, &n, s->objects, s->object_bytes) != Z_OK)
        return -4;
    if (n != s->uncompressed_length) return -5;
    return (int)n;
}
