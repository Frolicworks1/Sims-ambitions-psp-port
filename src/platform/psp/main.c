#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../../engine/m3g_loader.h"

PSP_MODULE_INFO("SimsAmbitionsPSP", 0, 0, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

static int load_and_decode_m3g(const char *path)
{
    static const unsigned char id[12] =
        {0xAB,0x4A,0x53,0x52,0x31,0x38,0x34,0xBB,0x0D,0x0A,0x1A,0x0A};

    SceUID fd = sceIoOpen(path, PSP_O_RDONLY, 0);
    if (fd < 0) return -1;

    SceOff end = sceIoLseek(fd, 0, PSP_SEEK_END);
    if (end < 29 || end > 32 * 1024 * 1024) {
        sceIoClose(fd);
        return -2;
    }
    if (sceIoLseek(fd, 0, PSP_SEEK_SET) < 0) {
        sceIoClose(fd);
        return -3;
    }

    size_t size = (size_t)end;
    unsigned char *file = (unsigned char *)malloc(size);
    if (!file) {
        sceIoClose(fd);
        return -4;
    }

    int got = sceIoRead(fd, file, size);
    sceIoClose(fd);
    if (got != (int)size) {
        free(file);
        return -5;
    }

    if (memcmp(file, id, sizeof(id)) != 0) {
        free(file);
        return -6;
    }

    /*
     * M3G Section 0 begins immediately after the 12-byte file identifier.
     * The first section is the mandatory uncompressed header object.
     */
    size_t off = 12;
    unsigned section_count = 0;
    unsigned total_objects = 0;
    unsigned mesh_count = 0;
    unsigned group_count = 0;
    unsigned vertex_buffer_count = 0;
    unsigned vertex_array_count = 0;
    unsigned index_buffer_count = 0;

    while (off + 17 <= size) {
        M3GSection s;
        int rc = m3g_read_section(file + off, size - off, &s);
        if (rc != 0 || s.total_length == 0) {
            free(file);
            return -10 + rc;
        }

        size_t object_capacity = s.uncompressed_length;
        if (object_capacity == 0)
            object_capacity = s.object_bytes;

        if (object_capacity > 24 * 1024 * 1024) {
            free(file);
            return -20;
        }

        unsigned char *objects = (unsigned char *)malloc(object_capacity);
        if (!objects) {
            free(file);
            return -21;
        }

        int unpacked = m3g_unpack_objects(&s, objects, object_capacity);
        if (unpacked < 0) {
            free(objects);
            free(file);
            return -30 + unpacked;
        }

        unsigned counts[256];
        int object_count = m3g_count_objects(
            objects, (size_t)unpacked, counts, 256);
        if (object_count < 0) {
            free(objects);
            free(file);
            return -40 + object_count;
        }

        section_count++;
        total_objects += (unsigned)object_count;
        mesh_count += counts[14];
        group_count += counts[9];
        vertex_buffer_count += counts[21];
        vertex_array_count += counts[20];
        index_buffer_count += counts[11];

        if (section_count == 1 && s.compression != 0) {
            free(objects);
            free(file);
            return -50;
        }

        free(objects);
        off += s.total_length;

        if (off > size) {
            free(file);
            return -51;
        }
    }

    if (off != size || section_count < 2) {
        free(file);
        return -52;
    }

    pspDebugScreenPrintf("M3G size: %lu bytes\n", (unsigned long)size);
    pspDebugScreenPrintf("Sections: %u\n", section_count);
    pspDebugScreenPrintf("Objects: %u\n", total_objects);
    pspDebugScreenPrintf("Meshes=%u Groups=%u\n",
                         mesh_count, group_count);
    pspDebugScreenPrintf("VertexBuffers=%u VertexArrays=%u\n",
                         vertex_buffer_count, vertex_array_count);
    pspDebugScreenPrintf("TriangleStrips=%u\n", index_buffer_count);

    free(file);
    return (int)total_objects;
}

int main(void)
{
    pspDebugScreenInit();
    pspDebugScreenPrintf("The Sims 3: Ambitions - PSP engine bring-up\n");
    pspDebugScreenPrintf("-------------------------------------------\n");
    pspDebugScreenPrintf("PSP MIPS runtime: OK\n");

    int rc = load_and_decode_m3g(
        "ms0:/PSP/GAME/SIMSAMB/DATA/scene_town_map_dp.m3g");

    if (rc >= 0) {
        pspDebugScreenPrintf("M3G decode pipeline: OK\n");
        pspDebugScreenPrintf("Next: VertexBuffer/TriangleStrip -> PSP GU\n");
    } else {
        pspDebugScreenPrintf("M3G load/decode failed (rc=%d)\n", rc);
        pspDebugScreenPrintf(
            "Install DATA/scene_town_map_dp.m3g beside EBOOT.PBP\n");
    }

    sceDisplayWaitVblankStart();
    sceKernelSleepThread();
    return 0;
}
