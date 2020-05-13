CXX=g++ -m64 -std=c++11 #-pg
CXXFLAGS=-g -Wall -Wunreachable-code
DEFINES=-DDEBUG 
#DEFINES=-DNDEBUG
INCLUDES=
LD=g++ -m64 -std=c++11 #-pg
LDFLAGS=-g -ggdb
#DASM=objdump
#DASMFLAGS=-dS

OBJS:= script_wrapper.o  luaenv.o task.o
LIBS= -llua

EXE = test

all: ${EXE}

${EXE}: main.o ${OBJS} 
	${LD} ${LDFLAGS} -o $@ $< ${OBJS} ${LIBS}
	#${DASM} ${DASMFLAGS} $@ >${EXE}.s

.cpp.o:
	$(CXX) ${CXXFLAGS} ${DEFINES} ${INCLUDES} -c $< -o $@

.cc.o:
	$(CXX) ${CXXFLAGS} ${DEFINES} ${INCLUDES} -c $< -o $@

clean:
	rm -f *.o *.s ${EXE} ${OBJS}  




