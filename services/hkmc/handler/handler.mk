HKMC_HANDLER_LIB_SRCS := $(wildcard services/hkmc/handler/*.cpp)
HKMC_HANDLER_LIB_OBJS := $(HKMC_HANDLER_LIB_SRCS:.cpp=.o)

hkmchandler.fo: $(HKMC_HANDLER_LIB_OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $(BUILD_ROOT)/$@ $^ $(FSW_CORE_LIB_OBJS)

services/hkmc/handler/%.o: services/hkmc/handler/%.cpp
	$(CXX) $(CXXFLAGS) -c -fPIC -o $@ $< -I$(BUILD_INCLUDE)