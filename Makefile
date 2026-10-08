TARGET = simsambitions
OBJS = src/platform/psp/main.o

CFLAGS = -O2 -G0 -Wall -Wextra
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
LIBS = -lpspdebug -lpspdisplay -lpspctrl -lpspiofilemgr

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = The Sims 3: Ambitions PSP Engine Bring-up
PSP_FW_VERSION = 600

PSPSDK = $(shell psp-config --pspsdk-path)

include $(PSPSDK)/lib/build.mak

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET)_strip.elf PARAM.SFO EBOOT.PBP
