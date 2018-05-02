PREFIX=/usr/local
C++ = g++
C_FLAGS = -g
INCLUDES = -I$(PREFIX)/include -I./
LIBRARIES = -O2
FLAGS =  $(INCLUDES) $(LIBRARIES) $(C_FLAGS)

testreader.exe: testreader.o
	$(C++) testreader.o -o testreader.exe $(FLAGS) 
	
testreader.o: testreader.cc testreader.hh
	$(C++) -c testreader.cc $(FLAGS)

clean:                                              
	rm ./*.o ./*~ ./\#*