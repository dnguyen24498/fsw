FSW_UPDATE_ENGINE_LIB_SRCS := $(wildcard services/updateengine/*.cpp)
FSW_UPDATE_ENGINE_LIB_OJBS := $(FSW_UPDATE_ENGINE_LIB_SRCS:.cpp=.o)

fswupdateengine.fo: $(FSW_UPDATE_ENGINE_LIB_OJBS)
	$(CXX) -shared -o $(BUILD_ROOT)/$@ $^ $(FSW_CORE_LIB_OBJS)
	
services/updateengine/%.o: services/updateengine/%.cpp
	$(CXX) -c -fPIC -o $@ $< -I$(BUILD_INCLUDE)
