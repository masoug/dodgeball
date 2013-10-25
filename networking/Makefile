# Makefile for building dodgeball

INC_DIR	:= ./
OBJ_DIR	:= ./
SRC_DIR	:= ./

TARGET 	:= 	networking
SRCS	:= 	main.cpp
OBJS	:= 	$(SRCS:.cpp=.o)
OBJS	:=	$(patsubst %, $(OBJ_DIR)/%, $(OBJS))

INC		:= 	-I$(INC_DIR) -I/usr/include/google 
CCFLAGS	:= 	-Wall $(INC)
LDFLAGS	:= 	
LIBS	:= 	-lprotobuf -lenet

CC		:= 	g++


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp 
	$(CC) $(CCFLAGS) -o $@ -c $<

clean::
	rm -rf $(OBJ_DIR)/*.o $(TARGET)

distclean:: clean
