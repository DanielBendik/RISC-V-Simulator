# PROGRAM: RISC-V Simulator
# AUTHOR:  Daniel Bendik

.SUFFIXES: .cpp .o

CXXFLAGS = -g -ansi -pedantic -Wall -Werror -Wextra -std=c++14

OBJECTS = hex.o memory.o main.o rv32i_decode.o registerfile.o rv32i_hart.o cpu_single_hart.o

TARGET = rv32i

all: $(TARGET)

$(TARGET): $(OBJECTS)
	g++ $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

.cpp.o:
	g++ $(CXXFLAGS) -c $<

main.o: main.cpp hex.h memory.h rv32i_decode.h
hex.o: hex.cpp hex.h
memory.o: memory.cpp memory.h hex.h
rv32i_decode.o: rv32i_decode.cpp rv32i_decode.h hex.h
registerfile.o: registerfile.cpp registerfile.h
rv32i_hart.o: rv32i_hart.cpp rv32i_hart.h
cpu_single_hart.o: cpu_single_hart.cpp cpu_single_hart.h

clean:
	rm -f $(TARGET) $(OBJECTS)