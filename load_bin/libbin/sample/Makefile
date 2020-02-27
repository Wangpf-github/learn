export SDK_DIR = /home/wang/Hi3559AV100_SDK_V2.0.3.0/
export HIARCH ?=Hi3559AV100

ifeq ($(HIARCH),Hi3559AV100)
	export CROSS = aarch64-himix100-linux-
	MPP_DIR = $(SDK_DIR)/mpp/out/linux/multi-core
endif

export SRC_DIR = $(shell pwd)
export INC_DIR = $(shell pwd)/../include
export BIN_LIB_DIR = $(shell pwd)/../release

TARGET := load_bin

INC := 
INC += -I$(INC_DIR)
INC += -I$(MPP_DIR)//include

LIBS :=
LIBS+= $(BIN_LIB_DIR)/libbin.a

LIBS+= $(MPP_DIR)/lib/libisp.a
LIBS+= $(MPP_DIR)/lib/lib_hicalcflicker.a
LIBS+= $(MPP_DIR)/lib/libmpi.a
LIBS+= $(MPP_DIR)/lib/libupvqe.a
LIBS+= $(MPP_DIR)/lib/libdnvqe.a
LIBS+= $(MPP_DIR)/lib/libVoiceEngine.a
LIBS+= $(MPP_DIR)/lib/lib_hidehaze.a
LIBS+= $(MPP_DIR)/lib/libtde.a
LIBS+= $(MPP_DIR)/lib/lib_hidrc.a
LIBS+= $(MPP_DIR)/lib/lib_hildci.a
LIBS+= $(MPP_DIR)/lib/libsecurec.a

CC = $(CROSS)gcc

SRC := $(SRC_DIR)/load_pq_bin.c
OBJ := $(SRC:%.c=%.o)

CFLAGS := -fPIC -Wall -lm -ldl -lpthread

.PHONY: all $(TARGET) clean

all : $(TARGET)

$(TARGET) : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
$(OBJ) : $(SRC)
	$(CC) $(CFLAGS) -o $@ -c $< $(INC)
	
clean:
	@rm -f $(TARGET)
	@rm -f *.o
