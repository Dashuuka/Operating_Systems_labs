# Makefile for your-project

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pthread

SRC_DIR := src
SRCS    := $(wildcard $(SRC_DIR)/*.cpp)
PROGS   := $(patsubst $(SRC_DIR)/%.cpp,%,$(SRCS))

.PHONY: all clean

all: $(PROGS)

$(PROGS): %: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(PROGS)
