
setup_edlib:
	cd edlib/build && cmake -D CMAKE_BUILD_TPE=Release .. && make

build_test:
	g++ -O3 -I`pwd`/edlib/edlib/include/ -L`pwd`/edlib/build/lib/ -std=c++17 main.cpp fuzzy_ner.cpp utils/timer.cpp -fopenmp -ledlib
