
setup_edlib:
	git submodule update --init
	cd edlib/build && cmake -D CMAKE_BUILD_TPE=Release .. && make

build_test:
	g++ -O3 -I`pwd`/edlib/edlib/include/ -L`pwd`/edlib/build/lib/ -std=c++17 main.cpp fuzzy_ner.cpp utils/timer.cpp -fopenmp -ledlib

build_fuzzy_ner:
	g++ -O3 -c -I`pwd`/edlib/edlib/include/ -L`pwd`/edlib/build/lib/ -std=c++17 fuzzy_ner.cpp -fopenmp -ledlib -fPIC

build_shared:
	g++ -shared -o libfuzzyner.so fuzzy_ner.o

build_wrapper:
	g++ -O3 -Wall -Werror -shared -std=c++17 -fPIC \
	`python3 -m pybind11 --includes` -I /home/jsteinbauer/.conda/envs/pybind_11/include/python3.9 \
	-I . -I`pwd`/edlib/edlib/include -L`pwd`/edlib/build/lib/ fuzzy_ner_wrapper.cpp -o fuzzy_ner`python3.9-config \
	--extension-suffix` -L. -lfuzzyner -Wl,-rpath,. -fopenmp -ledlib