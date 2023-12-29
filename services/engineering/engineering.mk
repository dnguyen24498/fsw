FSW_ENGINEERING_LIB_SRCS := $(wildcard services/engineering/*.cpp)
FSW_ENGINEERING_LIB_OBJS := $(FSW_ENGINEERING_LIB_SRCS:.cpp=.o)

EXPORT_HEADERS += services/engineering/Engineering.h

libfswengineering.a: $(FSW_ENGINEERING_LIB_OBJS)
	ar rcs $(BUILD_ROOT)/$@ $^ $(FSW_CORE_LIB_OBJS)
	
services/engineering/%.o: services/engineering/%.cpp
	$(CXX) $(CXXFLAGS) -DLOG_FILE -c $< -o $@ -I$(BUILD_INCLUDE)
	