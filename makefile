#OBJS specifies which files to compile as part of the project
OBJS = Board.o IO.o Minimax.o

#CC specifies which compiler we're using
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -w

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_ttf

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = Latrunculi

%.cpp: %.h %.hpp

%.o: %.cpp
	g++ -c $^ -o $@

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) main.cpp $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

web:$(OBJS)
	emcc -s USE_SDL=2 -s USE_SDL_TTF=2 main.cpp Minimax.cpp IO.cpp Board.cpp -o $(OBJ_NAME).html
