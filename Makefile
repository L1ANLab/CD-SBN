CC = g++ -fdiagnostics-color=always
FLAGS = -std=c++17 -g -Wall -I.
LIBS = -pthread

BUILD = build
OBJ = build/obj

GRAPH = graph
ALGORITHM = detection
UTILS = utils
BUILD_TOOLS = build/tools

all : dir $(BUILD)/cdsbn

dir: $(OBJ)

$(OBJ) :
	mkdir -p $(OBJ)

#################### start ####################

$(BUILD)/cdsbn: $(OBJ)/main.o \
		$(OBJ)/snapshot.o \
		$(OBJ)/continuous.o \
		$(OBJ)/synopsis.o \
		$(OBJ)/graph.o $(OBJ)/induced_graph.o \
		$(OBJ)/statistic.o \
		$(OBJ)/globals.o
	$(CC) $(FLAGS) $(OBJ)/main.o \
		$(OBJ)/snapshot.o \
		$(OBJ)/continuous.o \
		$(OBJ)/synopsis.o \
		$(OBJ)/graph.o $(OBJ)/induced_graph.o \
		$(OBJ)/statistic.o \
		$(OBJ)/globals.o \
		-o $(BUILD)/cdsbn $(LIBS)

$(OBJ)/main.o: $(ALGORITHM)/main.cpp \
		$(UTILS)/CLI11.hpp \
		$(UTILS)/globals.h $(UTILS)/types.h \
		$(UTILS)/statistic.h \
		$(GRAPH)/graph.h \
		$(ALGORITHM)/snapshot.h \
		$(ALGORITHM)/continuous.h \
		$(ALGORITHM)/synopsis.h
	$(CC) -c $(FLAGS) $(ALGORITHM)/main.cpp -o $(OBJ)/main.o

#################### detection ####################


$(OBJ)/snapshot.o: $(ALGORITHM)/snapshot.cpp \
		$(UTILS)/types.h $(UTILS)/globals.h \
		$(GRAPH)/graph.h $(OBJ)/induced_graph.o \
		$(ALGORITHM)/snapshot.h
	$(CC) -c $(FLAGS) $(ALGORITHM)/snapshot.cpp \
		-o $(OBJ)/snapshot.o

$(OBJ)/continuous.o: $(ALGORITHM)/continuous.cpp \
		$(UTILS)/types.h $(UTILS)/globals.h \
		$(GRAPH)/graph.h $(OBJ)/induced_graph.o \
		$(ALGORITHM)/continuous.h
	$(CC) -c $(FLAGS) $(ALGORITHM)/continuous.cpp \
		-o $(OBJ)/continuous.o

$(OBJ)/synopsis.o: $(ALGORITHM)/synopsis.cpp \
		$(UTILS)/types.h $(UTILS)/globals.h \
		$(GRAPH)/graph.h $(OBJ)/induced_graph.o \
		$(ALGORITHM)/synopsis.h
	$(CC) -c $(FLAGS) $(ALGORITHM)/synopsis.cpp \
		-o $(OBJ)/synopsis.o

#################### graph ####################

$(OBJ)/graph.o: $(GRAPH)/graph.cpp \
		$(UTILS)/types.h $(UTILS)/utils.h \
		$(GRAPH)/graph.h
	$(CC) -c $(FLAGS) $(GRAPH)/graph.cpp \
		-o $(OBJ)/graph.o

$(OBJ)/induced_graph.o: $(GRAPH)/induced_graph.cpp \
		$(UTILS)/types.h \
		$(GRAPH)/induced_graph.h $(GRAPH)/graph.h
	$(CC) -c $(FLAGS) $(GRAPH)/induced_graph.cpp \
		-o $(OBJ)/induced_graph.o

#################### utils ####################

$(OBJ)/globals.o: $(UTILS)/globals.cpp $(UTILS)/globals.h
	$(CC) -c $(FLAGS) $(UTILS)/globals.cpp \
		-o $(OBJ)/globals.o

$(OBJ)/statistic.o: $(UTILS)/statistic.cpp \
		$(UTILS)/types.h $(UTILS)/statistic.h 
	$(CC) -c $(FLAGS) $(UTILS)/statistic.cpp \
		-o $(OBJ)/statistic.o

#################### end ####################

.PHONY: clean

clean: 
	rm -r ${BUILD}
