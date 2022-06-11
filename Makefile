OBJ_DIR=obj
BUILD_DIR=build
SOURCE_DIR=source
INCLUDE_DIR=include

CXX=g++-11
CXXFLAGS=-std=c++20 -I$(INCLUDE_DIR) -Ilak/inc -Wno-abi -Wfatal-errors -Wno-attributes

all: $(BUILD_DIR)/fasm2bit
.PHONY: all

lak/meson.build:
	git submodule update --init lak
lak/inc/lak/%: lak/meson.build

LAK_SOURCES=debug.cpp strconv.cpp string_view.cpp unicode.cpp memmanip.cpp

HEADERS=$(wildcard $(INCLUDE_DIR)/*.hpp) $(wildcard $(INCLUDE_DIR)/*.inl) $(wildcard lak/inc/lak/*) Makefile
SOURCES=$(wildcard $(SOURCE_DIR)/*.cpp) $(LAK_SOURCES:%=lak/src/%)
OBJECTS=$(patsubst lak/src/%,$(OBJ_DIR)/lak_%,$(patsubst $(SOURCE_DIR)/%,$(OBJ_DIR)/%,$(SOURCES:%.cpp=%.obj)))

$(warning $(OBJECTS))

$(OBJ_DIR) $(BUILD_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.obj: $(SOURCE_DIR)/%.cpp $(HEADERS) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/lak_%.obj: lak/src/%.cpp $(HEADERS) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/fasm2bit: $(OBJECTS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(OBJ_DIR) $(BUILD_DIR)
