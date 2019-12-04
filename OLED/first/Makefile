TARGET=OLED
OBJS=oled.o
FLAGS=arm-hisiv400-linux-
CC=$(FLAGS)gcc

$(TARGET):$(OBJS)
	$(CC) $^ -o $@
.o:.c
	$(CC) -c $<
clean:
	rm *.o $(TARGET)
install:
	cp $(TARGET) /source/rootfs/
