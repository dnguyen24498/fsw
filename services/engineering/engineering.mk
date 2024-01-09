FSW_ENGINEERING_LIB_SRCS := $(wildcard services/engineering/*.cpp)
FSW_ENGINEERING_LIB_OBJS := $(FSW_ENGINEERING_LIB_SRCS:.cpp=.o)

fswengineering.fo: $(FSW_ENGINEERING_LIB_OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $(BUILD_ROOT)/$@ $^ $(FSW_CORE_LIB_OBJS)
	
services/engineering/%.o: services/engineering/%.cpp
	$(CXX) $(CXXFLAGS) -c -fPIC -o $@ $< -I$(BUILD_INCLUDE)
	