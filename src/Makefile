#
# DCE3D Dreamcast 3D Engine (c)2013-2016 Josh PH3NOM Pearson
#

#Comipled .elf File Name
TARGET = dce.elf

#Compiler Optimization Flags
KOS_CFLAGS += -ffast-math -O3 -fomit-frame-pointer -falign-loops=4 -falign-labels=4 -falign-functions=32 -falign-jumps=4 -Iinclude

#Engine Include Directory
KOS_CFLAGS += -I. -Idce/include/

#LDFLAGS=-Wl,-Map=output.map

#Engine C Source Code Files
OBJS = dce/dce-main.o dce/dce-input.o dce/dce-vector.o dce/dce-menu.o
OBJS += dce/dce-texture.o dce/dce-render.o
OBJS += dce/dce-collision.o
OBJS += dce/dce-animation.o dce/dce-timer.o dce/dce-decals.o dce/dce-sfx.o
OBJS += dce/dce-bbox.o dce/dce-init.o
OBJS += dce/dce-player.o dce/dce-gfx.o dce/dce-shader.o
OBJS += dce/dce-string.o dce/dce-rgb.o dce/dce-mat.o dce/dce-player-model.o dce/dce-rbo.o dce/dce-cbo.o
OBJS += dce/dce-enemy.o dce/dce-camera.o dce/dce-door.o dce/dce-trigger.o dce/dce-hud.o

#Engine Assembly Source Code Files
OBJS += dce/dce-clip-asm.o dce/dce-render-asm.o dce/dce-render-shadow.o dce/dce-collision-asm.o dce/dce-render-cull.o
OBJS += dce/dce-cull.o

#LibMPEG2 Decoder
KOS_CFLAGS += -I. -Ilibvdc/libmpeg2/include/

#LibMPG123 Decoder
KOS_CFLAGS += -I. -Ilibvdc/libmpg123/src/libmpg123/
OBJS += libvdc/snddrv.o
OBJS += libvdc/LibMPG123.o

#LibMPEG - MPEG Format Support
KOS_CFLAGS += -I. -Ilibvdc/
KOS_CFLAGS += -I. -Ilibvdc/libmpeg/
OBJS += libvdc/libmpeg/LibMPEG.o
OBJS += libvdc/LibMPEG2-Play.o
OBJS += libvdc/StreamBuffer.o libvdc/AudioStreamBuffer.o libvdc/StreamRender.o libvdc/GetTime.o


# Engine Font Utilities
KOS_CFLAGS += -I. -Ifont/
OBJS += font/font.o

# Quake 2 MD2 Interface
KOS_CFLAGS += -I. -Imd2/
OBJS += md2/md2.o

#Compile TARGET Elf
all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(TARGET) $(OBJS)

rm-elf:
	-rm -f $(TARGET)

$(TARGET): $(OBJS)
	$(KOS_CC) $(KOS_CFLAGS) $(LDFLAGS) $(KOS_LDFLAGS) -o $(TARGET) $(KOS_START) \
		$(OBJS) $(OBJEXTRA) -L$(KOS_BASE)/lib -ljpeg -lmpeg2 -lmpg123_1.13.1 -lm $(KOS_LIBS)

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist:
	rm -f $(OBJS)
	$(KOS_STRIP) $(TARGET)

