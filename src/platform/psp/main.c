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

static uint32_t le32(const unsigned char *p)
{
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static int load_and_decode_m3g(const char *path)
{
    static const unsigned char id[12] =
        {0xAB,0x4A,0x53,0x52,0x31,0x38,0x34,0xBB,0x0D,0x0A,0x1A,0x0A};

    SceUID fd = sceIoOpen(path, PSP_O_RDONLY, 0);
    if (fd < 0) return -1;

    SceOff end = sceIoLseek(fd, 0, PSP_SEEK_END);
    if (end < 28 || end > 32 * 1024 * 1024) {
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

    /* First section begins immediately after the 28-byte file header. */
    const unsigned char *section = file + 28;
    size_t section_size = size - 28;
    M3GSection s;

    int rc = m3g_read_section(section, section_size, &s);
    if (rc != 0) {
        free(file);
        return -10 + rc;
    }

    pspDebugScreenPrintf("M3G size: %lu bytes\n", (unsigned long)size);
    pspDebugScreenPrintf("Section: %lu bytes, compression=%u\n",
                         (unsigned long)s.total_length, (unsigned)s.compression);

    size_t object_capacity = s.uncompressed_length;
    if (object_capacity == 0) object_capacity = s.object_bytes;

    unsigned char *objects = (unsigned char *)malloc(object_capacity);
    if (!objects) {
        free(file);
        return -20;
    }

    int unpacked = m3g_unpack_objects(&s, objects, object_capacity);
    if (unpacked < 0) {
        free(objects);
        free(file);
        return -30 + unpacked;
    }

    unsigned counts[256];
    int object_count = m3g_count_objects(objects, (size_t)unpacked,
                                         counts, 256);
    if (object_count < 0) {
        free(objects);
        free(file);
        return -40 + object_count;
    }

    pspDebugScreenPrintf("Decoded object bytes: %d\n", unpacked);
    pspDebugScreenPrintf("M3G objects: %d\n", object_count);
    pspDebugScreenPrintf("Meshes=%u Groups=%u VertexBuffers=%u\n",
                         counts[14], counts[9], counts[21]);
    pspDebugScreenPrintf("VertexArrays=%u IndexBuffers=%u\n",
                         counts[20], counts[11]);

    free(objects);
    free(file);
    return object_count;
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
