PREFIX=/usr/local
C++ = g++
C_FLAGS = -g -std=c++14 -O2
INCLUDES = -I$(PREFIX)/include -I./ -I/home/giles/anaconda3/include
LIBRARIES = -llapack -lblas
FLAGS =  $(INCLUDES) $(LIBRARIES) $(C_FLAGS)

testreader.exe: testreader.o
	$(C++) testreader.o -o testreader.exe $(FLAGS) 
	
testreader.o: testreader.cc testreader.hh
	$(C++) -c testreader.cc $(FLAGS)

clean:                                              
	rm ./*.o ./*~ ./\#*