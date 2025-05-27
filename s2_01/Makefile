test : ingredient.o recette.o test.o
	g++ -Wall -o test ingredient.o recette.o test.o

ingredient.o : ingredient.cpp ingredient.h
	g++ -Wall -c ingredient.cpp

recette.o : recette.cpp recette.h	
	g++ -Wall -c recette.cpp

test.o : test.cpp
	g++ -Wall -c test.cpp

clean :
	rm *.o test