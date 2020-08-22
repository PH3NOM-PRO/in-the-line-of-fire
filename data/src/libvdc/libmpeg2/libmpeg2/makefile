#LibMPEG2 Dreamcast K:OS Makefile (c) PH3NOM 2011

KOS_CFLAGS += -I. -I../include/

OBJS += alloc.o
OBJS += cpu_accel.o
OBJS += cpu_state.o
OBJS += decode.o
OBJS += header.o
OBJS += idct.o
OBJS += idct_alpha.o
OBJS += idct_altivec.o
OBJS += idct_mmx.o
OBJS += motion_comp.o
OBJS += motion_comp_alpha.o
OBJS += motion_comp_altivec.o
OBJS += motion_comp_arm.o
OBJS += motion_comp_mmx.o
OBJS += motion_comp_vis.o
OBJS += slice.o
OBJS += convert/rgb.o
OBJS += convert/uyvy.o

INCS += -I. -DLITTLE_ENDIAN=1
CFLAGS += -O3 -m4-single-only -ffast-math

all: libmpeg2.a

libmpeg2.a: $(OBJS)
	$(KOS_AR) rcs libmpeg2.a $(OBJS)

include $(KOS_BASE)/Makefile.rules
