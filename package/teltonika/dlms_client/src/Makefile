TARGET = dlms_client

SRCDIR := ./src

SOURCES := $(wildcard $(SRCDIR)/*.c)

OBJS = $(SOURCES:.c=.o)

CFLAGS += -D_GNU_SOURCE -DCONFIG_MACHINE_$(TLT_PLATFORM_NAME)
CFLAGS += -Wall -Wextra -Werror -pedantic -fno-common -ffunction-sections -fdata-sections -Wundef

LDFLAGS += -lubus -lubox -lsqlite3 -ltlt_uci -luci -lgurux_dlms_c -ltlt_logger -lpthread
RM = rm -f

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(OBJS) $(TARGET)
