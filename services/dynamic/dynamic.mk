FSW_DYNAMIC_LIB_SRCS := $(wildcard services/dynamic/*.cpp)
FSW_DYNAMIC_LIB_OBJS := $(FSW_DYNAMIC_LIB_SRCS:.cpp=.o)

fswdynamic.fo: $(FSW_DYNAMIC_LIB_OBJS)
	$(CXX) -shared -o $(BUILD_ROOT)/$@ $^ $(FSW_CORE_LIB_OBJS)
	
services/dynamic/%.o: services/dynamic/%.cpp
	$(CXX) -c -fPIC -o $@ $< -I$(BUILD_INCLUDE)
	