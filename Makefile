# Makefile for building dodgeball

INC_DIR	:=	inc
OBJ_DIR	:=	obj
SRC_DIR	:=	src

TARGET 	:= 	dodgeball
SRCS	:= 	main.cpp dodgeball_engine.cpp dynamic_object.cpp state_machine.cpp networking.cpp
OBJS	:= 	$(SRCS:.cpp=.o)
OBJS	:=	$(patsubst %, $(OBJ_DIR)/%, $(OBJS))

INC		:= 	-I$(INC_DIR) -I/usr/include/bullet -I/usr/include/irrlicht -I/usr/include/google -I/usr/include/OpenThreads
CCFLAGS	:= 	-Wall $(INC)
LDFLAGS	:= 	
LIBS	:= 	-lIrrlicht -lBulletDynamics -lBulletCollision -lLinearMath -lenet -lprotobuf -lOpenThreads

CC		:= 	g++


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp 
	$(CC) $(CCFLAGS) -o $@ -c $<

clean::
	rm -rf $(OBJ_DIR)/*.o $(TARGET)

distclean:: clean
