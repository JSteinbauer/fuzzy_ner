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
#include <omp.h>
#include "utils/timer.hpp"

#include "edlib.h"

/*
#define THREAD_NUM 10
omp_set_thread_num(THREAD_NUM); // set number of threads in
*/

using namespace std;


struct EntityValue {
    string value;
    list<string> synonyms;
};

struct EntityType {
    string name;
    list<EntityValue> entity_values;
};


class FuzzyNer {
	public:
		FuzzyNer(list<string> &synonyms) {
		    copy(synonyms.begin(), synonyms.end(), back_inserter(this->synonyms));
		    this->prepare_word_vectors();
		}
		~FuzzyNer() {
		}

		void build_char_map() {
            wstring synonym_wstring;
            char_count = 0;
            for (vector<string>::iterator it = synonyms.begin(); it != synonyms.end(); ++it) {
                synonym_wstring = wstring_converter.from_bytes(*it);
                for (size_t n = 0; n < synonym_wstring.size(); ++n) {
                    if (char_map.find(synonym_wstring[n]) == char_map.end()) {
                        char_map[synonym_wstring[n]] = char_count;
                        char_count++;
                    }
                }
            }
		}

        // Prepares vectors of character occurrences and length for each synonym
		void prepare_word_vectors() {
		    // Build the character map
		    this->build_char_map();

		    synonym_vectors.resize(synonyms.size());
		    synonym_sizes.resize(synonyms.size());
		    wstring synonym_wstring;
            for (vector<string>::iterator it = synonyms.begin(); it != synonyms.end(); ++it) {
                int index = it - synonyms.begin();
                synonym_vectors[index].resize(128, 0);
                synonym_sizes[index] = it->size();
                synonym_wstring = wstring_converter.from_bytes(*it);
		        for (size_t n = 0; n < synonym_wstring.size(); ++n) {
                    synonym_vectors[index][char_map[synonym_wstring[n]]]++;
                }
		    }
		}

		void find_matches(string& text, float min_score) {
		    short unsigned int text_length = text.size();
		    short unsigned int min_length = text_length*min_score;
		    short unsigned int max_length = text_length/min_score;

		    EdlibAlignResult edit_distance_result;

            wstring text_wstring = wstring_converter.from_bytes(text);
		    map<unsigned short, unsigned short> char_frequency;
		    for (size_t n = 0; n < text_wstring.size(); ++n) {
                char_frequency[char_map[text_wstring[n]]] =
                (char_frequency.find(char_map[text_wstring[n]]) == char_frequency.end())
                ? 1 : char_frequency[char_map[text_wstring[n]]]+1;
		    }

		    vector<string> match_candidates;
		    for (vector<unsigned short>::iterator it = synonym_sizes.begin(); it != synonym_sizes.end(); ++it) {
                int index = it - synonym_sizes.begin();
                unsigned int max_length = max(text_length, *it);
                // Continue loop if the synonym is too long or too short
                if (*it > max_length || *it < min_length) {
                    continue;
                }

                if (consider_synonym_for_similarity_match(char_frequency, index, max_length, min_score)) {
                    edit_distance_result = edlibAlign(text.c_str(), text_length, synonyms[index].c_str(), *it, edlibDefaultAlignConfig());
                    if (edit_distance_result.status == EDLIB_STATUS_OK) {
                        float synonym_score = 1. - (float) edit_distance_result.editDistance/max_length;
                        if (synonym_score >= min_score) {
                            // cout << synonyms[index] << " , score : " << synonym_score << endl;
                        }
                    }
                }

		    }
		    edlibFreeAlignResult(edit_distance_result);
		}

        bool consider_synonym_for_similarity_match(map<unsigned short, unsigned short>& char_frequency, int index, unsigned int max_length, float min_score) {
            int max_char_diff = (1.-min_score)*max_length;
            int char_diff = 0;
            for (auto const& [char_index, char_occurrence] : char_frequency) {
                char_diff += abs(synonym_vectors[index][char_index]-char_occurrence);
                if (char_diff > max_char_diff) {
                    return false;
                }
            }
            return true;
        }

        // Printing methods
		void print_synonyms() {
		    for (vector<string>::iterator i = synonyms.begin(); i != synonyms.end(); ++i) {
		        cout << *i << endl;
		    }
		}
		void print_ascii() {
		    for (vector<string>::iterator it = synonyms.begin(); it != synonyms.end(); ++it) {
		        for (char const &c: *it) {
                    cout << int(c) << ' ';
                }
                cout << endl;
		    }

		}
		void print_word_vectors() {
            for (vector<vector<unsigned short>>::iterator it = synonym_vectors.begin(); it != synonym_vectors.end(); ++it) {
		        for (unsigned short const &c: *it) {
                    cout << c << ' ';
                }
                cout << endl;
		    }
		}
		void print_word_sizes() {
            for (vector<unsigned short>::iterator it = synonym_sizes.begin(); it != synonym_sizes.end(); ++it) {
                cout << *it << endl;
		    }
		}


	private:
	    vector<string> synonyms;
	    vector<unsigned short> synonym_sizes;
	    vector<vector<unsigned short>> synonym_vectors;
	    int char_count;
	    map<wchar_t, int> char_map;

	    wstring_convert<codecvt_utf8_utf16<wchar_t>> wstring_converter;

};

list<string> read_synonyms(string directory) {
    list<string> synonym_list;

    string line;
    ifstream myfile(directory.c_str());
    if (myfile.is_open())
    {
        while ( getline (myfile, line) )
        {
            synonym_list.push_back(line);
        }
        myfile.close();
    }

    return synonym_list;
}

int main() {
//    list<string> synonyms {"Hello", "you", "dog"};
    list<string> synonyms = read_synonyms("/home/jsteinbauer/ondewo/ondewo-cai/data/gazetteers/street_name_vienna/all.txt");

    /*
    EntityValue test = {"Hello", synonyms};

    list<EntityValue> values {test};
    EntityType test2 = {"Animal", values};

    cout << test.value << endl;
    cout << test2.name << endl;
    */
    Timer timer;
    timer.start();
    FuzzyNer *test = new FuzzyNer(synonyms);
    /*
    test->print_word_vectors();
    test->print_word_sizes();
    */
    timer.stop();
    cout << "time (ms) " << timer.elapsedMilliseconds() << endl;

    //string test_string = "Sait";
    string test_string = "Saitensteterstraße";
    vector<string> test_strings;

    for (int i = 0; i < 100; ++i) {
        if (i < 50) {
            test_strings.push_back("Saitensteterstraße");
        }
        else {
            test_strings.push_back("Sait");
        }
    }
    /*
    for (int i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            test_strings.push_back("Saitensteterstraße");
        }
        else {
            test_strings.push_back("Sait");
        }
    }
    */
    timer.start();

    //#pragma omp parallel for
    for (int i = 0; i < 100; ++i) {
        test->find_matches(test_strings[i], 0.8);
    }
    timer.stop();
    cout << "time (ms) " << timer.elapsedMilliseconds() << endl;
    return 0;
}
