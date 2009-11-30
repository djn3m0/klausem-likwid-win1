TAG = GCC
VERSION=0.1

#CONFIGURE BUILD SYSTEM
TARGET_LIB = liblikwid.a
BUILD_DIR  = ./$(TAG)
SRC_DIR    = ./src
DOC_DIR    = ./doc
MAKE_DIR   = ./
Q         ?= @

#DO NOT EDIT BELOW
include $(MAKE_DIR)/include_$(TAG).mk
include $(MAKE_DIR)/config.mk
INCLUDES  += -I./src/includes
PINLIB  = liblikwidpin.so

VPATH     = $(SRC_DIR)
OBJ       = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))

CPPFLAGS := $(CPPFLAGS) $(DEFINES) $(INCLUDES) 
LINKING =   @echo "===>  LINKING  $@"; \
			${CC} $(CFLAGS) $(CPPFLAGS) ${LFLAGS} -o $@  $< $(OBJ)

all: $(BUILD_DIR) $(OBJ) likwid-perfCtr likwid-features likwid-topology likwid-pin  $(TARGET_LIB)  $(PINLIB) 

tags:
	@echo "===>  GENERATE  TAGS"
	$(Q)ctags -R

likwid-perfCtr:  $(SRC_DIR)/perfCtr/perfCtrMain.c
	$(LINKING)

likwid-features:  $(SRC_DIR)/features/cpuFeaturesMain.c
	$(LINKING)

likwid-topology:  $(SRC_DIR)/topology/cpuTopologyMain.c
	$(LINKING)

likwid-pin:  $(SRC_DIR)/pin/applicationPinMain.c
	$(LINKING)

$(TARGET_LIB):
	@echo "===>  CREATE LIB  $(TARGET_LIB)"
	$(Q)${AR} -cq $(TARGET_LIB) $(filter-out $(BUILD_DIR)/main.o,$(OBJ))

$(BUILD_DIR):
	@mkdir $(BUILD_DIR)

$(PINLIB): 
	@echo "===>  CREATE LIB  $(PINLIB)"
	$(Q)$(MAKE) -s -C src/pthread-overload/ $(PINLIB) 

#PATTERN RULES
$(BUILD_DIR)/%.o:  %.c
	@echo "===>  COMPILE  $@"
	$(Q)$(CC) -c  $(CFLAGS) $(ANSI_CFLAGS) $(CPPFLAGS) $< -o $@
	$(Q)$(CC) $(CPPFLAGS) -MT $(@:.d=.o) -MM  $< > $(BUILD_DIR)/$*.d


ifeq ($(findstring $(MAKECMDGOALS),clean),)
-include $(OBJ:.o=.d)
endif

.PHONY: clean distclean install uninstall

clean:
	@echo "===>  CLEAN"
	@rm -rf $(BUILD_DIR)

distclean: clean
	@echo "===>  DIST CLEAN"
	@rm -f likwid-*
	@rm -f $(TARGET_LIB)
	@rm -f $(PINLIB)

install:
	@echo "===>INSTALL applications to $(PREFIX)/bin"
	@mkdir -p $(PREFIX)/bin
	@cp -f likwid-*  $(PREFIX)/bin
	@chmod 755 $(PREFIX)/bin/likwid-*
	@echo "===> INSTALL man pages to $(MANPREFIX)/man1"
	@mkdir -p $(MANPREFIX)/man1
	@sed -e "s/<VERSION>/$(VERSION)/g" -e "s/<DATE>/$(DATE)/g" < $(DOC_DIR)/likwid-topology.1 > $(MANPREFIX)/man1/likwid-topology.1
	@chmod 644 $(MANPREFIX)/man1/likwid-*
	@echo "===> INSTALL libraries to $(PREFIX)/lib"
	@mkdir -p $(PREFIX)/lib
	@cp -f liblikwid-*  $(PREFIX)/lib

	



