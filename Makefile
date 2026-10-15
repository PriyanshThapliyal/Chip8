CXX ?= g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

BIN_DIR = bin
SRCS = main.cpp chip8.cpp display.cpp
OBJS = $(patsubst %.cpp, $(BIN_DIR)/%.o, $(SRCS))

# -------------------------------------------------------------
# OS Detection & Platform-Specific Settings
# -------------------------------------------------------------
ifeq ($(OS),Windows_NT)
    # --- WINDOWS (MinGW / MSYS2) ---
    TARGET = chip8.exe
    LDFLAGS = -lmingw32 -lSDL2main -lSDL2
    MKDIR = if not exist $(BIN_DIR) mkdir $(BIN_DIR)
    RM = del /Q /F
    RMDIR = if exist $(BIN_DIR) rmdir /Q /S $(BIN_DIR)
else
    # --- UNIX (Linux / macOS) ---
    TARGET = chip8
    MKDIR = mkdir -p $(BIN_DIR)
    RM = rm -f
    RMDIR = rm -rf $(BIN_DIR)

    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        # --- macOS ---
        CXXFLAGS += $(shell sdl2-config --cflags 2>/dev/null || pkg-config --cflags sdl2 2>/dev/null || echo -I/opt/homebrew/include)
        LDFLAGS += $(shell sdl2-config --libs 2>/dev/null || pkg-config --libs sdl2 2>/dev/null || echo -L/opt/homebrew/lib -lSDL2)
    else
        # --- Linux / BSD ---
        CXXFLAGS += $(shell pkg-config --cflags sdl2 2>/dev/null || sdl2-config --cflags 2>/dev/null)
        LDFLAGS += $(shell pkg-config --libs sdl2 2>/dev/null || sdl2-config --libs 2>/dev/null || echo -lSDL2)
    endif
endif

# -------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(TARGET)

$(BIN_DIR)/%.o: %.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR):
	$(MKDIR)

clean:
	$(RM) $(TARGET) *.o
	$(RMDIR)

.PHONY: all clean
