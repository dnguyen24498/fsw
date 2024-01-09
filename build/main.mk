TARGET := fsw

PROJECT_VERSION_MAJOR := 1
PROJECT_VERSION_MINOR := 0
PROJECT_VERSION_PATCH := 2

BUILD_ROOT := build/out
BUILD_INCLUDE := build/include
DEFAULT_CONFIG := /home/runner/fsw/.config

FSW_SRCS := $(wildcard init/*.cpp)
FSW_OBJS := $(FSW_SRCS:.cpp=.o)

CXX := g++
CXXFLAGS := \
	-std=c++14 -Wall -Wextra -pthread -g -ldl \
	-DDEFAULT_CONFIG=$(DEFAULT_CONFIG) \
	-DPROJECT_VERSION_MAJOR=$(PROJECT_VERSION_MAJOR) \
	-DPROJECT_VERSION_MINOR=$(PROJECT_VERSION_MINOR) \
	-DPROJECT_VERSION_PATCH=$(PROJECT_VERSION_PATCH)

EXPORT_HEADERS :=
IMPORT_SERVICES += \
  hkmchandler.fo

all: $(TARGET)

include core/core.mk
include services/services.mk
include test/test.mk

$(TARGET): core headers $(IMPORT_SERVICES) $(FSW_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(FSW_OBJS) $(FSW_CORE_LIB_OBJS)

services: $(IMPORT_SERVICES)

init/%.o: init/%.cpp
	$(CXX) $(CXXFLAGS) -c -fPIC $< -o $@ -I$(BUILD_INCLUDE)

headers:
	mkdir -p $(BUILD_INCLUDE)
	cp $(EXPORT_HEADERS) $(BUILD_INCLUDE)

clean:
	find . -name "*.o" -type f -delete
	find . -name "*.a" -type f -delete
	find . -name "*.fo" -type f -delete
	rm -rf $(BUILD_INCLUDE)
	rm -f $(TARGET)

.PHONY: all
