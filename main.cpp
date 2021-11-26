#include <iostream>
#include <list>
#include <vector>
#include <fstream>
#include "fuzzy_ner.hpp"
#include "utils/timer.hpp"

using namespace std;


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
    /*
    string string_to_split = "This is  a string to split";
    vector<string> split_string = get_substrings(string_to_split);
    for (vector<string>::iterator it = split_string.begin(); it != split_string.end(); ++it) {
        cout << *it << endl;
    }
    */


//    list<string> synonyms {"Hello", "you", "dog"};
    list<string> synonyms = read_synonyms("/home/jsteinbauer/ondewo/ondewo-cai/data/gazetteers/street_name_vienna/all.txt");

    list<string> substrings = read_synonyms("/home/jsteinbauer/Codes/cpp_snippets/fuzzy_ner/test_strings.txt");
    vector<string> substrings_vec;
    copy(substrings.begin(), substrings.end(), back_inserter(substrings_vec));
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

    timer.start();
    string test_text ("My favorite stree in vienna is Seitensteterstrasse");
    vector<EntityMatch> results = test->find_matches(test_text, 0.78);
    /*
    test->print_word_vectors();
    test->print_word_sizes();
    */
    timer.stop();
    cout << "time for initialization (ms) " << timer.elapsedMilliseconds() << endl;

    for (vector<EntityMatch>::iterator it = results.begin(); it != results.end(); ++it) {
        cout << it->synonym << " " << it->annotation << " " << it->score << endl;
    }
    //string test_string = "Sait";
    string test_string = "Saitensteterstraße";
    vector<string> test_strings;

    return 0;
}