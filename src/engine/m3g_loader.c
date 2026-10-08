#include <stdint.h>
#include <stddef.h>
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

/* JSR-184 stores UInt16/UInt32/Float32 values little-endian. */
static uint32_t m3g_u32(const uint8_t *p)
{
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static uint16_t m3g_u16(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
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

int m3g_unpack_objects(const M3GSection *s, uint8_t *dst, size_t dst_size)
{
    if (!s || !dst) return -1;

    if (s->compression == 0) {
        if (dst_size < s->object_bytes) return -2;
        memcpy(dst, s->objects, s->object_bytes);
        return (int)s->object_bytes;
    }

    if (s->uncompressed_length > dst_size) return -3;

    uLongf n = (uLongf)dst_size;
    if (uncompress(dst, &n, s->objects, s->object_bytes) != Z_OK)
        return -4;
    if (n != s->uncompressed_length) return -5;
    return (int)n;
}

/* Walk an uncompressed M3G object section and count object records. */
int m3g_count_objects(const uint8_t *objects, size_t size, unsigned *counts, size_t count_len)
{
    size_t off = 0;
    unsigned total = 0;

    if (!objects || !counts || count_len < 256) return -1;
    memset(counts, 0, count_len * sizeof(counts[0]));

    while (off + 5 <= size) {
        uint8_t type = objects[off];
        uint32_t len = m3g_u32(objects + off + 1);
        off += 5;

        if ((size_t)len > size - off) return -2;
        if (type < count_len) counts[type]++;
        total++;
        off += len;
    }

    return (off == size) ? (int)total : -3;
}

int m3g_first_mesh_summary(const uint8_t *objects, size_t size,
                           unsigned *vertex_buffer_ref,
                           unsigned *index_buffer_ref,
                           unsigned *appearance_ref)
{
    size_t off = 0;

    if (!objects || !vertex_buffer_ref || !index_buffer_ref || !appearance_ref)
        return -1;

    while (off + 5 <= size) {
        uint8_t type = objects[off];
        uint32_t len = m3g_u32(objects + off + 1);
        const uint8_t *p = objects + off + 5;
        off += 5;

        if ((size_t)len > size - off) return -2;

        /*
         * Mesh object payload:
         * Object3D header (12), VertexBuffer ref (4),
         * submesh count (4), index buffer refs, appearance refs...
         * This routine only exposes the first references when present.
         */
        if (type == 14 && len >= 24) {
            *vertex_buffer_ref = m3g_u32(p + 12);
            *index_buffer_ref = m3g_u32(p + 20);
            *appearance_ref = (len >= 28) ? m3g_u32(p + 24) : 0;
            return 0;
        }

        off += len;
    }

    return -3;
}
