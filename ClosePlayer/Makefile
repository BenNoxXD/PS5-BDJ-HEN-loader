# Copyright (c) 2025 BenNox_XD
# This file is part of PS5-BDJ-HEN-loader and is licensed under the MIT License.
# See the LICENSE file in the root of the project for full license information.

PS5_HOST ?= ps5
PS5_PORT ?= 9021

ifdef PS5_PAYLOAD_SDK
    include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk
else
    $(error PS5_PAYLOAD_SDK is undefined)
endif

ELF := ClosePlayer.elf

CFLAGS := -Wall -Werror -g
LDFLAGS := -lSceSystemService

SRCS = main.c $(wildcard ../HENloader_C_part/src/*.c)
OBJS = $(SRCS:.c=.o)

all: $(ELF)

$(ELF): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(ELF)

test: $(ELF)
	$(PS5_DEPLOY) -h $(PS5_HOST) -p $(PS5_PORT) $^