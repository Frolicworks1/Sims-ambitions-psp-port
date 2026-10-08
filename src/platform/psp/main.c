#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>

PSP_MODULE_INFO("SimsAmbitionsPSP", 0, 0, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

int main(void)
{
    pspDebugScreenInit();
    pspDebugScreenPrintf("The Sims 3: Ambitions - PSP port bootstrap\n");
    pspDebugScreenPrintf("PSP runtime scaffold is alive.\n");
    pspDebugScreenPrintf("Game engine integration is not yet linked.\n");

    sceDisplayWaitVblankStart();
    sceKernelSleepThread();
    return 0;
}
