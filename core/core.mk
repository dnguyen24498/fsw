FSW_CORE_LIB_SRCS := $(wildcard core/*.cpp)
FSW_CORE_LIB_OBJS := $(FSW_CORE_LIB_SRCS:.cpp=.o)

EXPORT_HEADERS += $(wildcard core/*.h)

core: $(FSW_CORE_LIB_OBJS)
	
core/%.o: core/%.cpp
	$(CXX) $(CXXFLAGS) -DLOG_FILE -c -fPIC -o $@ $<
	