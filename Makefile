CC=gcc
CFLAGS=-Isrc/ -Iinclude/ -std=c99 -Wall -MMD -MP -O0 -g
LFLAGS=-lm
BUILD=build
BIN=$(BUILD)/imgc

SOURCES=src/main.c src/blend.c src/image.c src/impl.c
OBJECTS=$(SOURCES:.c=.c.o)
OBJECTS:=$(patsubst %,$(BUILD)/%,$(OBJECTS))
DEPENDS=$(OBJECTS:.o=.d)

-include $(DEPENDS)

all: $(BIN)

$(BUILD)/%.c.o: %.c
	@echo "CC -" $<
	@mkdir -p "$$(dirname "$@")"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJECTS)
	@echo "LD -" $@
	@$(CXX) $(LFLAGS) $(OBJECTS) -o $@

.PHONY: clean

clean:
	@$(RM) -rv $(BIN) $(BUILD)

run: all
	@$(BIN)

