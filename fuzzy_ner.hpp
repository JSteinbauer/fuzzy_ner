#ifndef FUZZY_NER_HPP
#define FUZZY_NER_HPP

#include <iostream>
#include <cstdio>
#include <cmath>
#include <list>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>
#include <string>
#include <limits.h>
#include <omp.h>
#include <regex>
#include <algorithm>

#include "edlib.h"


using namespace std;

struct EntityMatch {
    string synonym;
    string annotation;
    float score;
};

class FuzzyNer {
	public:
		FuzzyNer(list<string> &synonyms);
		~FuzzyNer();

		vector<EntityMatch> find_matches(string& text, float min_score);
		vector<EntityMatch> find_matches_single_word(string& text, float min_score);

        bool consider_synonym_for_similarity_match(map<unsigned short, unsigned short>& char_frequency, int index, unsigned int max_length, float min_score);
        vector<string> get_substrings(string text);

        // Printing methods
		void print_synonyms();
		void print_word_vectors();
		void print_word_sizes();

        regex* delimiter_rgx;
	private:
        // Prepares vectors of character occurrences and length for each synonym
		void prepare_word_vectors();

		void build_char_map();

	    vector<string> synonyms;
	    vector<unsigned short> synonym_sizes;
	    vector<vector<unsigned short>> synonym_vectors;
	    int char_count;
	    map<wchar_t, int> char_map;

	    wstring_convert<codecvt_utf8_utf16<wchar_t>> wstring_converter;
};

#endif