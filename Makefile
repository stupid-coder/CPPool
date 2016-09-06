test/test.app: test/test.cpp
	g++ -ggdb -o test/test.app test/test.cpp -Iinclude -lpthread
