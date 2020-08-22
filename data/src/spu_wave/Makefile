# SPU WAVE STREAM
# (C) PH3NOM 2011

TARGET = spu_wave.elf
OBJS = spu_wave.o main.o

all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(TARGET) $(OBJS) 

rm-elf:
	-rm -f $(TARGET) 

$(TARGET): $(OBJS) 
	$(KOS_CCPLUS) $(KOS_CFLAGS) $(KOS_CPPFLAGS) $(KOS_LDFLAGS) -o $(TARGET) $(KOS_START) \
		$(OBJS) $(OBJEXTRA) $(KOS_LIBS)

