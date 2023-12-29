CXX := g++
CXXFLAGS := -std=c++14 -Wall -Wextra -pthread

TARGET := fsw

BUILD_ROOT := build/out
BUILD_INCLUDE := build/include

FSW_SRCS := $(wildcard init/*.cpp)
FSW_OBJS := $(FSW_SRCS:.cpp=.o)

EXPORT_HEADERS :=
IMPORT_LIBS :=

all: $(TARGET)

include core/core.mk
include services/services.mk

$(TARGET): $(IMPORT_LIBS) copy_headers $(FSW_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ init/init.o -L$(BUILD_ROOT) $(addprefix -l,$(patsubst lib%.a,%,$(IMPORT_LIBS)))

init/%.o: init/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I$(BUILD_INCLUDE)

copy_headers:
	mkdir -p $(BUILD_INCLUDE)
	cp $(EXPORT_HEADERS) $(BUILD_INCLUDE)

clean:
	find . -name "*.o" -type f -delete
	find . -name "*.a" -type f -delete
	rm -f $(TARGET)

.PHONY: all