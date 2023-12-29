FSW_CORE_LIB_SRCS := $(wildcard core/*.cpp)
FSW_CORE_LIB_OBJS := $(FSW_CORE_LIB_SRCS:.cpp=.o)

EXPORT_HEADERS += $(wildcard core/*.h)
IMPORT_LIBS += libfswcore.a

libfswcore.a: $(FSW_CORE_LIB_OBJS)
	ar rcs $(BUILD_ROOT)/$@ $^
	
core/%.o: core/%.cpp
	$(CXX) $(CXXFLAGS) -DLOG_FILE -c $< -o $@
	