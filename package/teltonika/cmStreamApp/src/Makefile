APP_SOURCES = $(filter-out src/modem.cc, $(wildcard src/*.cc))
APP_TARGET = cmStreamApp
CFLAGS += -Wall -Wextra
LDFLAGS =-lsera -luci -lpthread -lmnfinfo -lubus

ifneq (,$(findstring MOBILE_SUPPORT,$(CFLAGS)))
	LDFLAGS += -lgsm_utils -lgsm
	APP_SOURCES += src/modem.cc
endif

RM = rm -f

all:
	$(CXX) -std=c++11 $(APP_SOURCES) $(CFLAGS) $(LDFLAGS) -o $(APP_TARGET)

clean:
	$(RM) $(APP_TARGET)
