#include "fuzzy_ner.hpp"


using namespace std;

FuzzyNer::FuzzyNer(list<string> &synonyms) {
    copy(synonyms.begin(), synonyms.end(), back_inserter(this->synonyms));
    this->prepare_word_vectors();
    delimiter_rgx = new regex("\\s+");
}
FuzzyNer::~FuzzyNer() {}

void FuzzyNer::build_char_map() {
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
void FuzzyNer::prepare_word_vectors() {
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

vector<EntityMatch> FuzzyNer::find_matches(string& text, float min_score) {
    vector<string> split_text = this->get_substrings(text);

    // randomly shuffle split text to increase efficiency of parallel for loop
    std::random_shuffle ( split_text.begin(), split_text.end() );

    vector<EntityMatch> entity_matches;
    #pragma omp parallel
    {
        vector<EntityMatch> entity_matches_slave;
        #pragma omp for nowait
        for (int i = 0; i < split_text.size(); ++i) {
            vector<EntityMatch> new_entity_matches = this->find_matches_single_word(split_text[i], min_score);
            entity_matches_slave.insert(
                entity_matches_slave.end(),
                make_move_iterator(new_entity_matches.begin()),
                make_move_iterator(new_entity_matches.end())
            );
        }
        #pragma omp critical
        {
            entity_matches.insert(
                entity_matches.end(),
                make_move_iterator(entity_matches_slave.begin()),
                make_move_iterator(entity_matches_slave.end())
            );
        }
    }
    return entity_matches;
}

vector<EntityMatch> FuzzyNer::find_matches_single_word(string& text, float min_score) {

    short unsigned int text_length = text.size();
    short unsigned int min_length = text_length*min_score;
    short unsigned int max_length = min_score > 0 ? text_length/min_score : USHRT_MAX;

    bool edlib_memory_allocated = false;
    EdlibAlignResult edit_distance_result;

    wstring text_wstring = wstring_converter.from_bytes(text);
    map<unsigned short, unsigned short> char_frequency;
    for (size_t n = 0; n < text_wstring.size(); ++n) {
        char_frequency[char_map[text_wstring[n]]] =
        (char_frequency.find(char_map[text_wstring[n]]) == char_frequency.end())
        ? 1 : char_frequency[char_map[text_wstring[n]]]+1;
    }

    EntityMatch new_entity_match;
    vector<EntityMatch> match_candidates;
    for (vector<unsigned short>::iterator it = synonym_sizes.begin(); it != synonym_sizes.end(); ++it) {
        int index = it - synonym_sizes.begin();
        unsigned int max_length = max(text_length, *it);
        // Continue loop if the synonym is too long or too short
        if (*it > max_length || *it < min_length) {
            continue;
        }

        if (consider_synonym_for_similarity_match(char_frequency, index, max_length, min_score)) {
            edlib_memory_allocated = true;
            edit_distance_result = edlibAlign(text.c_str(), text_length, synonyms[index].c_str(), *it, edlibDefaultAlignConfig());
            if (edit_distance_result.status == EDLIB_STATUS_OK) {
                float synonym_score = 1. - (float) edit_distance_result.editDistance/max_length;
                if (synonym_score >= min_score) {
                    new_entity_match = {.synonym = synonyms[index], .annotation = text, .score = synonym_score};
                    //lock_guard<mutex> guard(entity_matches_mutex);
                    match_candidates.push_back(new_entity_match);
                }
            }
        }
    }
    if (edlib_memory_allocated) {
        edlibFreeAlignResult(edit_distance_result);
    }
    return match_candidates;
}

bool FuzzyNer::consider_synonym_for_similarity_match(map<unsigned short, unsigned short>& char_frequency, int index, unsigned int max_length, float min_score) {
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

vector<string> FuzzyNer::get_substrings(string text) {
    sregex_token_iterator iter(text.begin(), text.end(), *delimiter_rgx, {-1,0});
    sregex_token_iterator end;

    vector<string> split_string;
    vector<int> indices;
    int i = 0;
    for ( ; iter != end; ++iter) {
        split_string.push_back(*iter);
        if (!regex_match((string) *iter, *delimiter_rgx)) {
            indices.push_back(i);
        }
        i++;
    }

    vector<string> substrings;
    for (int i = 0; i < indices.size(); ++i) {
        for (int k = 0; k < indices.size() - i; ++k) {
            string substring("");
            for (int n = indices[k]; n < indices[k+i]+1; ++n) {
                substring += split_string[n];
            }
            substrings.push_back(substring);
        }
    }
    return substrings;
}

// Printing methods
void FuzzyNer::print_synonyms() {
    for (vector<string>::iterator it = synonyms.begin(); it != synonyms.end(); ++it) {
        cout << *it << endl;
    }
}

void FuzzyNer::print_word_vectors() {
    for (vector<vector<unsigned short>>::iterator it = synonym_vectors.begin(); it != synonym_vectors.end(); ++it) {
        for (unsigned short const &c: *it) {
            cout << c << ' ';
        }
        cout << endl;
    }
}
void FuzzyNer::print_word_sizes() {
    for (vector<unsigned short>::iterator it = synonym_sizes.begin(); it != synonym_sizes.end(); ++it) {
        cout << *it << endl;
    }
}

