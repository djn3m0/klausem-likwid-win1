TAG = GCC

#CONFIGURE BUILD SYSTEM
TARGET	   = hpcUtil
TARGET_LIB = libhpcUtil.a
BUILD_DIR  = ./$(TAG)
SRC_DIR    = ./src
MAKE_DIR   = ./
Q         ?= @

#DO NOT EDIT BELOW
include $(MAKE_DIR)/include_$(TAG).mk
INCLUDES  += -I./src/includes

VPATH     = $(SRC_DIR)
OBJ       = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))

CPPFLAGS := $(CPPFLAGS) $(DEFINES) $(INCLUDES) 

all: $(TARGET)  $(TARGET_LIB)

$(TARGET): $(BUILD_DIR) $(OBJ)
	@echo "===>  LINKING  $(TARGET)"
	$(Q)${CC} ${LFLAGS} -o $(TARGET) $(OBJ) $(LIBS)

$(TARGET_LIB): $(BUILD_DIR) $(OBJ)
	@echo "===>  CREATE LIB  $(TARGET_LIB)"
	$(Q)${AR} -cq $(TARGET_LIB) $(filter-out $(BUILD_DIR)/main.o,$(OBJ))

$(BUILD_DIR)/%.o:  %.c
	@echo "===>  COMPILE  $@"
	$(Q)$(CC) -c  $(CFLAGS) $(CPPFLAGS) $< -o $@
	$(Q)$(CC) $(CPPFLAGS) -MT $(@:.d=.o) -MM  $< > $(BUILD_DIR)/$*.d


$(BUILD_DIR):
	@mkdir $(BUILD_DIR)


ifeq ($(findstring $(MAKECMDGOALS),clean),)
-include $(OBJ:.o=.d)
endif

.PHONY: clean dist-clean

clean:
	@echo "===>  CLEAN"
	@rm -rf $(BUILD_DIR)

dist-clean:
	@echo "===>  DIST CLEAN"
	@rm -rf $(BUILD_DIR)
	@rm -f $(TARGET)
	@rm -f $(TARGET_LIB)

