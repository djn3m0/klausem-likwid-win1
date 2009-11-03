TAG = GCC

#CONFIGURE BUILD SYSTEM
TARGET_LIB = libhpcUtil.a
BUILD_DIR  = ./$(TAG)
SRC_DIR    = ./src
MAKE_DIR   = ./
Q         ?= @

#DO NOT EDIT BELOW
include $(MAKE_DIR)/include_$(TAG).mk
INCLUDES  += -I./src/includes
PINLIB  = libptoverride.so

VPATH     = $(SRC_DIR) $(DOC_DIR)/mkd
OBJ       = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))

CPPFLAGS := $(CPPFLAGS) $(DEFINES) $(INCLUDES) 

all: likwid-perfCtr likwid-features likwid-topology likwid-pin  $(TARGET_LIB)  $(PINLIB) 

tags:
	@echo "===>  GENERATE  TAGS"
	$(Q)ctags -R

likwid-perfCtr: $(BUILD_DIR) $(OBJ) $(SRC_DIR)/perfCtr/perfCtrMain.c
	@echo "===>  LINKING  $@"
	$(Q)${CC} $(CFLAGS) $(CPPFLAGS) ${LFLAGS} -o $@ $(SRC_DIR)/perfCtr/perfCtrMain.c $(OBJ) $(LIBS)

likwid-features: $(BUILD_DIR) $(OBJ) $(SRC_DIR)/features/cpuFeaturesMain.c
	@echo "===>  LINKING  $@"
	$(Q)${CC} $(CFLAGS) $(CPPFLAGS) ${LFLAGS} -o $@ $(SRC_DIR)/features/cpuFeaturesMain.c $(OBJ) $(LIBS)

likwid-topology: $(BUILD_DIR) $(OBJ) $(SRC_DIR)/topology/cpuTopologyMain.c
	@echo "===>  LINKING  $@"
	$(Q)${CC} $(CFLAGS) -std=c99 $(CPPFLAGS) ${LFLAGS} -o $@ $(SRC_DIR)/topology/cpuTopologyMain.c $(OBJ) $(LIBS)

likwid-pin: $(BUILD_DIR) $(OBJ) $(SRC_DIR)/pin/applicationPinMain.c
	@echo "===>  LINKING  $@"
	$(Q)${CC} $(CFLAGS) -std=c99 $(CPPFLAGS) -DCOLOR ${LFLAGS} -o $@ $(SRC_DIR)/pin/applicationPinMain.c $(OBJ) $(LIBS)

$(TARGET_LIB): $(BUILD_DIR) $(OBJ)
	@echo "===>  CREATE LIB  $(TARGET_LIB)"
	$(Q)${AR} -cq $(TARGET_LIB) $(filter-out $(BUILD_DIR)/main.o,$(OBJ))

$(BUILD_DIR):
	@mkdir $(BUILD_DIR)

$(PINLIB): 
	@echo "===>  BUILD    $(PINLIB)"
	$(Q)$(MAKE) -s -C src/pthread-overload/ $(PINLIB) 

#PATTERN RULES
$(BUILD_DIR)/%.o:  %.c
	@echo "===>  COMPILE  $@"
	$(Q)$(CC) -c  $(CFLAGS) $(CPPFLAGS) $< -o $@
	$(Q)$(CC) $(CPPFLAGS) -MT $(@:.d=.o) -MM  $< > $(BUILD_DIR)/$*.d


ifeq ($(findstring $(MAKECMDGOALS),clean),)
-include $(OBJ:.o=.d)
endif

.PHONY: clean distclean

clean:
	@echo "===>  CLEAN"
	@rm -rf $(BUILD_DIR)

distclean: clean
	@echo "===>  DIST CLEAN"
	@rm -f likwid-*
	@rm -f $(TARGET_LIB)
	@rm -f $(PINLIB)

