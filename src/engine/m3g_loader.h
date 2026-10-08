#ifndef M3G_LOADER_H
#define M3G_LOADER_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t compression;
    uint32_t total_length;
    uint32_t uncompressed_length;
    uint32_t checksum;
    const uint8_t *objects;
    uint32_t object_bytes;
} M3GSection;

int m3g_read_section(const uint8_t *data, size_t size, M3GSection *out);
int m3g_unpack_objects(const M3GSection *s, uint8_t *dst, size_t dst_size);
int m3g_count_objects(const uint8_t *objects, size_t size, unsigned *counts, size_t count_len);
int m3g_first_mesh_summary(const uint8_t *objects, size_t size,
                           unsigned *vertex_buffer_ref,
                           unsigned *index_buffer_ref,
                           unsigned *appearance_ref);

#endif
