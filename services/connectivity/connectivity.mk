FSW_CONNECTIVITY_LIB_SRCS := $(wildcard services/connectivity/*.cpp)
FSW_CONNECTIVITY_LIB_OBJS := $(FSW_CONNECTIVITY_LIB_SRCS:.cpp=.o)

fswconnectivity.fo: $(FSW_CONNECTIVITY_LIB_OBJS)
	$(CXX) -shared -o $(BUILD_ROOT)/$@ $^ $(FSW_CORE_LIB_OBJS)
	
services/connectivity/%.o: services/connectivity/%.cpp
	$(CXX) -c -fPIC -o $@ $< -I$(BUILD_INCLUDE)
