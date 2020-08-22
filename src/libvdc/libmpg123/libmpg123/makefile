# LibMPG123 v.1.13.1
# Dreamcast KOS Makefile by PH3NOM 2011

KOS_CFLAGS += -I. -I../

OBJS= \
	intsym.h \
	compat.o \
	compat.h \
	parse.o \
	parse.h \
	frame.o \
	format.o \
	frame.h \
	reader.h \
	debug.h \
	decode.h \
	sample.h \
	dct64_sh4.o \
	synth.h \
	synth_mono.h \
	synth_ntom.h \
	synth_8bit.h \
	synths.h \
	equalizer.o \
	huffman.h \
	icy.h \
	icy2utf8.h \
	id3.h \
	id3.o \
	true.h \
	getbits.h \
	optimize.h \
	optimize.o \
	readers.o \
	tabinit.o \
	libmpg123.o \
	mpg123lib_intern.h \
	mangle.h \
	getcpuflags.h \
	index.h \
	index.o

OBJS += \
	lfs_alias.o \
	icy.o \
	icy2utf8.o \
	l2tables.h \
	layer1.o \
	layer2.o \
	layer3.o \
	dither.h \
	dither.o \
	feature.o \
	ntom.o \
	synth_8bit.o \
	synth_8bit.o \
	stringbuf.o \
	l12_integer_tables.h \
	l3_integer_tables.h \

INCS += -I. -DLITTLE_ENDIAN=1
CFLAGS += -m4-single-only -ffast-math

all: libmpg123_1.13.1.a

libmpg123_1.13.1.a: $(OBJS)
	$(KOS_AR) rcs libmpg123_1.13.1.a $(OBJS)

include $(KOS_BASE)/Makefile.rules
