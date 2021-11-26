#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "fuzzy_ner.hpp"

namespace py = pybind11;

PYBIND11_MODULE(fuzzy_ner, m) {
    py::class_<FuzzyNer>(m, "FuzzyNer")
        .def(py::init<list<string> &>())
        .def("find_matches", &FuzzyNer::find_matches)
        .def("find_matches_single_word", &FuzzyNer::find_matches_single_word)
        .def("consider_synonym_for_similarity_match", &FuzzyNer::consider_synonym_for_similarity_match)
        .def("get_substrings", &FuzzyNer::get_substrings)
        .def("print_synonyms", &FuzzyNer::print_synonyms)
        .def("print_word_vectors", &FuzzyNer::print_word_vectors)
        .def("print_word_sizes", &FuzzyNer::print_word_sizes);

    py::class_<EntityMatch>(m, "EntityMatch")
        .def_readwrite("synonym", &EntityMatch::synonym)
        .def_readwrite("annotation", &EntityMatch::annotation)
        .def_readwrite("score", &EntityMatch::score);
}
