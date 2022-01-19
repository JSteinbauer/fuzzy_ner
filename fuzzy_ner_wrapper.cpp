#include <pybind11/pybind11.h>
#include "fuzzy_ner.hpp"

namespace py = pybind11;

PYBIND11_MODULE(fuzzy_ner, m) {
    py::class_<FuzzyNer>(m, "FuzzyNer")
        .def(py::init<list<string> &>())
        .def("find_matches", &FuzzyNer::find_matches)
        .def("find_matches_single_word", &FuzzyNer::find_matches_single_word);

}

