TAG = GCC

#CONFIGURE BUILD SYSTEM
TARGET_LIB = libhpcUtil.a
BUILD_DIR  = ./$(TAG)
SRC_DIR    = ./src
DOC_DIR    = ./doc
MAKE_DIR   = ./
MKD        = $(PERL) ext/perl/mkd2html.pl
Q         ?= @

#DO NOT EDIT BELOW
include $(MAKE_DIR)/include_$(TAG).mk
INCLUDES  += -I./src/includes
PINLIB_PT  = libptoverride_pt.so
PINLIB_OMP = libptoverride_omp.so

VPATH     = $(SRC_DIR) $(DOC_DIR)/mkd
OBJ       = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))
MKD_SRC   = $(patsubst $(DOC_DIR)/mkd/%.mkd, $(DOC_DIR)/%.html,$(wildcard $(DOC_DIR)/mkd/*.mkd))

CPPFLAGS := $(CPPFLAGS) $(DEFINES) $(INCLUDES) 

all: perfCtr cpuFeatures cpuTopology  $(TARGET_LIB)  $(PINLIB_PT)  $(PINLIB_OMP) 

perfCtr: $(BUILD_DIR) $(OBJ) $(SRC_DIR)/PerfCtr/perfCtrMain.c
	@echo "===>  LINKING  $@"
	$(Q)${CC} $(CFLAGS) $(CPPFLAGS) ${LFLAGS} -o $@ $(SRC_DIR)/PerfCtr/perfCtrMain.c $(OBJ) $(LIBS)

cpuFeatures: $(BUILD_DIR) $(OBJ) $(SRC_DIR)/CpuFeatures/cpuFeaturesMain.c
	@echo "===>  LINKING  $@"
	$(Q)${CC} $(CFLAGS) $(CPPFLAGS) ${LFLAGS} -o $@ $(SRC_DIR)/CpuFeatures/cpuFeaturesMain.c $(OBJ) $(LIBS)

cpuTopology: $(BUILD_DIR) $(OBJ) $(SRC_DIR)/CpuTopology/cpuTopologyMain.c
	@echo "===>  LINKING  $@"
	$(Q)${CC} $(CFLAGS) $(CPPFLAGS) ${LFLAGS} -o $@ $(SRC_DIR)/CpuTopology/cpuTopologyMain.c $(OBJ) $(LIBS)

$(TARGET_LIB): $(BUILD_DIR) $(OBJ)
	@echo "===>  CREATE LIB  $(TARGET_LIB)"
	$(Q)${AR} -cq $(TARGET_LIB) $(filter-out $(BUILD_DIR)/main.o,$(OBJ))

doc: doxygen $(MKD_SRC)

doxygen:
	@echo "===>  BUILD DOXYGEN"
	$(Q)$(DOXYGEN) $(DOC_DIR)/Doxyfile

$(BUILD_DIR):
	@mkdir $(BUILD_DIR)

$(PINLIB_PT): 
	@echo "===>  BUILD    $(PINLIB_PT)"
	$(Q)$(MAKE) -s -C src/PinLib/ $(PINLIB_PT) 

$(PINLIB_OMP): 
	@echo "===>  BUILD    $(PINLIB_OMP)"
	$(Q)$(MAKE) -s -C src/PinLib/ $(PINLIB_OMP) 

#PATTERN RULES
$(BUILD_DIR)/%.o:  %.c
	@echo "===>  COMPILE  $@"
	$(Q)$(CC) -c  $(CFLAGS) $(CPPFLAGS) $< -o $@
	$(Q)$(CC) $(CPPFLAGS) -MT $(@:.d=.o) -MM  $< > $(BUILD_DIR)/$*.d

$(DOC_DIR)/%.html:  %.mkd
	@echo "===>  BUILD DOC  $@"
	$(Q)$(MKD)  $<  $@ test



ifeq ($(findstring $(MAKECMDGOALS),clean),)
-include $(OBJ:.o=.d)
endif

.PHONY: clean distclean

clean:
	@echo "===>  CLEAN"
	@rm -rf $(BUILD_DIR)

distclean:
	@echo "===>  DIST CLEAN"
	@rm -rf $(BUILD_DIR)
	@rm -f perfCtr
	@rm -f cpuFeatures
	@rm -rf $(DOC_DIR)/doxygen
	@rm -f $(DOC_DIR)/*.html
	@rm -f $(DOC_DIR)/doxygen.log
	@rm -f $(TARGET_LIB)
	@rm -f $(PINLIB_PT)
	@rm -f $(PINLIB_OMP)

