CXX = g++

######################################################
# if $(COMPILE_FLAGS) is given by upper makefile 
# else if no $(COMPILE_FLAGS) is given 
ifneq ($(COMPILE_FLAGS),)
CXXFLAGS += $(COMPILE_FLAGS) 
else
CXXFLAGS = -Wall -O3 -DSF1_DEBUG
endif


TEST_OBJS = t_NGramExtractor.o t_LikeExtractor.o t_StemExtractor.o t_KoreanLanguageExtractor.o t_LAManager.o t_master_suite.o
TEST_OBJS1 = t_NGramExtractor.o t_master_suite.o
TEST_OBJS2 = t_LikeExtractor.o t_master_suite.o
TEST_OBJS3 = t_StemExtractor.o t_master_suite.o
TEST_OBJS4 = t_KoreanLanguageExtractor.o t_master_suite.o
TEST_OBJS5 = t_Regulation.o t_master_suite.o
TEST_OBJS6 = t_LAManager.o
TEST_EXE1 = t_NGramExtractor
TEST_EXE2 = t_LikeExtractor
TEST_EXE3 = t_StemExtractor
TEST_EXE4 = t_KoreanLanguageExtractor
TEST_EXE5 = t_Regulation
TEST_EXE6 = t_LAManager

SOURCE_ROOT = ../../source

INCLUDE_PATH = \
			   -I$(SOURCE_ROOT)/core/include \
			   -I$(SOURCE_ROOT)external/stemmer \
			   -I$(SOURCE_ROOT)external/kmaorange/src/interface \
			   -I$(SOURCE_ROOT)external/kmaorange/src/pos_tags \
			   -I$(SOURCE_ROOT)/external/kmaorange/interface \
			   -I$(SOURCE_ROOT)/external/stemmer \
			   -I$(WISELIB)/include \
			   -I$(IZENELIB)/include \
			   -I$(IZENELIB)/include/3rdparty \
			   -I/usr/local/include/boost-1_36

LIB_PATH = -L $(SOURCE_ROOT)/../lib\
#-L$(SOURCE_ROOT)/external/kmaorange \
		   -L$(SOURCE_ROOT)/external/stemmer \


LIB = -lsf1v5_la_manager \
	  -lsf1v5_configuration_manager \
	  -lsf1v5_kmaorange \
	  -lboost_unit_test_framework-gcc41-mt \
	  -lboost_thread-gcc41-mt \
	  -lboost_serialization-gcc41-mt \
	  -lstemmer \


OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))


all: temp $(TEST_LIB_OUT) $(TEST_EXE1) $(TEST_EXE2) $(TEST_EXE3) $(TEST_EXE4) $(TEST_EXE5) $(TEST_EXE6)

temp :
	rm -rf $(TEST_EXE5)

$(TEST_EXE1): $(TEST_OBJS1)
	g++ $(CXXFLAGS) -o $(TEST_EXE1) $(TEST_OBJS1) $(LIB_PATH) $(LIB)
$(TEST_EXE2): $(TEST_OBJS2)
	g++ $(CXXFLAGS) -o $(TEST_EXE2) $(TEST_OBJS2) $(LIB_PATH) $(LIB)
$(TEST_EXE3): $(TEST_OBJS3)
	g++ $(CXXFLAGS) -o $(TEST_EXE3) $(TEST_OBJS3) $(LIB_PATH) $(LIB)
$(TEST_EXE4): $(TEST_OBJS4)
	g++ $(CXXFLAGS) -o $(TEST_EXE4) $(TEST_OBJS4) $(LIB_PATH) $(LIB)
$(TEST_EXE5): $(TEST_OBJS5)
	g++ $(CXXFLAGS) -o $(TEST_EXE5) $(TEST_OBJS5) $(LIB_PATH) $(LIB)
$(TEST_EXE6): $(TEST_OBJS6)
	g++ $(CXXFLAGS) -o $(TEST_EXE6) $(TEST_OBJS6) $(LIB_PATH) $(LIB)

clean:
	rm -rf *.o $(TARGET) $(TEST_EXE1) $(TEST_EXE2) $(TEST_EXE3) $(TEST_EXE4) $(TEST_EXE5) $(TEST_EXE6)


%.o : %.cpp
	$(CXX) $(INCLUDE_PATH) $(CXXFLAGS) -c -o $@ $?
