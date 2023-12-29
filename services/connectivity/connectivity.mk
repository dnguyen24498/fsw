FSW_CONNECTIVITY_LIB_SRCS := $(wildcard services/connectivity/*.cpp)
FSW_CONNECTIVITY_LIB_OBJS := $(FSW_CONNECTIVITY_LIB_SRCS:.cpp=.o)

EXPORT_HEADERS += services/connectivity/SerialConnectivity.h

libfswconnectivity.a: $(FSW_CONNECTIVITY_LIB_OBJS)
	ar rcs $(BUILD_ROOT)/$@ $^ $(FSW_CORE_LIB_OBJS)
	
services/connectivity/%.o: services/connectivity/%.cpp
	$(CXX) $(CXXFLAGS) -DLOG_FILE -c $< -o $@ -I$(BUILD_INCLUDE)

