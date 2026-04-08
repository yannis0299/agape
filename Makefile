SRC_DIRS   := src
VCS_DIR    := .git
BUILD_DIR  := target
EXECUTABLE := agapec

SRCS := $(shell find $(SRC_DIRS) -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS  := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CC      := clang-20
CFLAGS  := $(INC_FLAGS) -MMD -MP
CFLAGS  += -Wall -std=c23 -march=native
LDFLAGS := -lc

build: $(BUILD_DIR)/$(EXECUTABLE)

run: build
	@$(BUILD_DIR)/$(EXECUTABLE) $(ARGS)

# link executable
$(BUILD_DIR)/$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)
	@chmod +x $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	@$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# assembly source
$(BUILD_DIR)/%.s.o: %.s
	@$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P := mkdir -p
