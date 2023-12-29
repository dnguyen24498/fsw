FSW_UPDATE_ENGINE_LIB_SRCS := $(wildcard services/updateengine/*.cpp)
FSW_UPDATE_ENGINE_LIB_OJBS := $(FSW_UPDATE_ENGINE_LIB_SRCS:.cpp=.o)

EXPORT_HEADERS += services/updateengine/UpdateEngine.h

libfswupdateengine.a: $(FSW_UPDATE_ENGINE_LIB_OJBS)
	ar rcs $(BUILD_ROOT)/$@ $^ $(FSW_CORE_LIB_OBJS)
	
services/updateengine/%.o: services/updateengine/%.cpp
	$(CXX) $(CXXFLAGS) -DLOG_FILE -c $< -o $@ -I$(BUILD_INCLUDE)
