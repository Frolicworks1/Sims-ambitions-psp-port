#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <stdint.h>

PSP_MODULE_INFO("SimsAmbitionsPSP", 0, 0, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

static uint32_t be32(const unsigned char *p)
{
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static int inspect_m3g(const char *path)
{
    unsigned char h[32];
    SceUID fd = sceIoOpen(path, PSP_O_RDONLY, 0);
    if (fd < 0)
        return -1;

    int n = sceIoRead(fd, h, sizeof(h));
    sceIoClose(fd);
    if (n < 28)
        return -2;

    /* M3G identifier: AB JSR184 BB 0D 0A 1A 0A */
    static const unsigned char id[12] =
        {0xAB,0x4A,0x53,0x52,0x31,0x38,0x34,0xBB,0x0D,0x0A,0x1A,0x0A};

    int i;
    for (i = 0; i < 12; ++i)
        if (h[i] != id[i])
            return -3;

    pspDebugScreenPrintf("M3G: valid Java ME 3D file\n");
    pspDebugScreenPrintf("Compression: %u\n", (unsigned)h[12]);
    pspDebugScreenPrintf("Section size: %lu\n", (unsigned long)be32(&h[13]));
    pspDebugScreenPrintf("Version: %u.%u\n", (unsigned)h[26], (unsigned)h[27]);
    return 0;
}

int main(void)
{
    pspDebugScreenInit();
    pspDebugScreenPrintf("The Sims 3: Ambitions - PSP engine bring-up\n");
    pspDebugScreenPrintf("-------------------------------------------\n");
    pspDebugScreenPrintf("PSP MIPS runtime: OK\n");

    int rc = inspect_m3g("ms0:/PSP/GAME/SIMSAMB/DATA/scene_town_map_dp.m3g");
    if (rc == 0) {
        pspDebugScreenPrintf("Reference asset pipeline: OK\n");
        pspDebugScreenPrintf("Next: M3G object decoding + PSP GU renderer\n");
    } else {
        pspDebugScreenPrintf("Reference M3G not found/invalid (rc=%d)\n", rc);
        pspDebugScreenPrintf("Install DATA/scene_town_map_dp.m3g beside EBOOT.PBP\n");
    }

    pspDebugScreenPrintf("\nThis build is an engine bring-up, not yet the full game.\n");
    sceDisplayWaitVblankStart();
    sceKernelSleepThread();
    return 0;
}
