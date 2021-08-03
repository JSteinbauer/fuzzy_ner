
build_test:
	g++ -O3 -I/home/jsteinbauer/Codes/cpp_snippets/edlib/edlib/include/ -L/home/jsteinbauer/Codes/cpp_snippets/edlib/build/lib/ -std=c++17 fuzzy_ner.cpp utils/timer.cpp -fopenmp -ledlib
