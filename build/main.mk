TARGET := fsw

BUILD_ROOT := build/out
BUILD_INCLUDE := build/include

FSW_SRCS := $(wildcard init/*.cpp)
FSW_OBJS := $(FSW_SRCS:.cpp=.o)

CXX := g++
CXXFLAGS := -std=c++14 -Wall -Wextra -pthread -g -ldl -L$(BUILD_ROOT)

EXPORT_HEADERS :=
IMPORT_SERVICES :=

all: $(TARGET)

include core/core.mk
include services/services.mk

$(TARGET): core headers $(IMPORT_SERVICES) $(FSW_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(FSW_OBJS) $(FSW_CORE_LIB_OBJS)

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
