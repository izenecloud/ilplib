CXX = g++

######################################################
# if $(COMPILE_FLAGS) is given by upper makefile 
# else if no $(COMPILE_FLAGS) is given 
ifneq ($(COMPILE_FLAGS),)
CXXFLAGS += $(COMPILE_FLAGS) 
else
CXXFLAGS = -Wall -O3
endif

DIRS = ./regulator 
TEST_DIR = ./test

TARGET_PATH = ../lib
TARGET = $(TARGET_PATH)/libsf1v5_la_manager.a

INCLUDE_PATH = \
			   -I../include \
			   -I../../external/stemmer \
			   -I../../external/kmaorange/src/interface \
			   -I../../external/kmaorange/src/pos_tags \
			   -I/usr/local/include/boost-1_36 \
			   -I$(WISELIB)/include \
			   -I$(IZENELIB)/include \
			   -I$(IZENELIB)/include/3rdparty



OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))



all: $(TARGET) subdirs 

$(TARGET) : $(OBJECTS)
	ar rsuv $@ $^

subdirs :
	@for dir in $(DIRS); do \
		make -C $$dir || exit $?; \
	done


clean: test_dir
	rm -rf *.o $(TARGET) 
	@for dir in $(DIRS); do \
		make -C $$dir clean || exit $?; \
	done

test_dir :
	@for dir in $(TEST_DIR); do \
		make -C $$dir clean || exit $?; \
	done


%.o : %.cpp
	$(CXX) $(INCLUDE_PATH) $(CXXFLAGS) -c -o $@ $?
