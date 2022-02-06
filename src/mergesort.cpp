#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <chrono>
#include <tbb/parallel_invoke.h>

using namespace std;

/* TODO: Use proper error_t mechanism to return back  errors */

/* Check: Standard List of punctuations as per cctype.h,
   modify as needed
*/
const string PUNCTUATIONS = {"!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"};


/* 
   Remove leading and trailing punctuation marks, with the help of 
   PUNCTUATIONS string 
*/
void clean_puncts(string& word) {
    if(word.empty()){
        return;
    }
    word.erase(0, min(word.find_first_not_of(PUNCTUATIONS), word.size()-1));
    word.erase(word.find_last_not_of(PUNCTUATIONS) + 1 , string::npos);
}


/*
    1) Read an incoming string, 
    2) break it down into words, 
    3) remove leading and trailing punctuations
    4) Store shared_ptr's to these words in the vector split_words
    5) TODO: Check for error returns .
*/
void split_into_words(const string incoming,
                     vector<shared_ptr<string>>& split_words) {
    shared_ptr<string> temp = make_shared<string>();
    for (int i = 0; i < incoming.length()-1; i++)
    {   
        /*  TODO: Check if delimeter list is non-standard(i.e RFC type standard) 
            if not ask for a vector<string> delimeter from user, 
            and check for each delimeter, instead of the ones below(ugly!)
        */
        char c = incoming[i];
        if (c == ' ' || c == '\n' || c == '\t' ||
           (c == '\r' && incoming[i+1] == '\n'))
        {
            clean_puncts(*temp);
            if(!(*temp).empty()){
                /* Move to a new shared_ptr, which is to be 
                    stored in split_words
                */
                shared_ptr<string> temp2 = move(temp);
                temp = make_shared<string>();
                split_words.push_back(temp2); 
            }

            /* Special case where delimeter is 2 bytes*/
            if(c == '\r') {
                i++;
            }
        } else {
            (*temp).append(1,c);
        }
    }
    return;
}


void print_words(vector<shared_ptr<string>> & split_words){
    cout<<"================================================================"<<endl;
    for (auto s: split_words){
        cout<< (*s)<<endl;
    }
    cout<<"================================================================"<<endl;
}


/*
    Regular merge with the caveat that it iterates over vector of shared_ptr's
    but uses the values at the ptrs'(string) for comparison to decide 
    sorting order
*/
void merge(vector<shared_ptr<string>>& split_words, 
            int begin, int mid, int end) {
    if(split_words.empty() || end <= begin){
        return; // Nothing to do;
    }           
    vector<shared_ptr<string>> temp_ptr_copy;
    int i = begin;
    int j = mid+1;
    while (i <= mid && j <= end) {
        if ((*split_words[i])  < (*split_words[j])) {
            temp_ptr_copy.push_back(split_words[i]);
            i++;
        } else {
            temp_ptr_copy.push_back(split_words[j]);
            j++;
        }
    }
    if (i > mid) {
        temp_ptr_copy.insert(temp_ptr_copy.end(), split_words.begin() + j, 
                             split_words.begin() + end);
    } else {
        temp_ptr_copy.insert(temp_ptr_copy.end(), split_words.begin() + i, 
                             split_words.begin() + mid+1);
    } 

    copy(temp_ptr_copy.begin(), temp_ptr_copy.end(), 
         split_words.begin() + begin);     
}


/* 
    Parallelized version of regular merge_sort functionality using Intel TBB 
*/
void mergesort_internal(vector<shared_ptr<string>>& split_words,
                        int begin, int end){
    if(end <= begin){
        return;
    }
    int mid = begin + (end-begin)/2;

#ifdef _PARALLEL_SORT_    

    /*Parallelizing the divide step of mergesort*/
    tbb::parallel_invoke([&] {mergesort_internal(split_words, begin, mid);},
                         [&] {mergesort_internal(split_words, mid +1, end);});  

#else  /* Regular SERIAL Sort */
    mergesort_internal(split_words, begin, mid);
    mergesort_internal(split_words, mid +1, end);

#endif

    merge(split_words, begin, mid, end);
} 


void mergesort(vector<shared_ptr<string>> & split_words) {          
    mergesort_internal(split_words, 0, split_words.size()-1);
}


void mergesort(vector<shared_ptr<string>> & split_words,
                int begin, int end) {
    mergesort_internal(split_words, begin, end);
}
    

/* 
    Remove duplicate words: my_unique is the modified utility
    for std::unique, since we have iterators to shared_ptr,
    but what we need to compare are the strings pointed to by them
*/
/* TODO: Specific utility function, may use templates for reuse */
vector<shared_ptr<string>>::iterator 
my_unique(vector<shared_ptr<string>>::iterator begin,
          vector<shared_ptr<string>>::iterator end) {
                                        
    if (begin == end) {
        return end;
    }              
    vector<shared_ptr<string>>::iterator result = begin;
    while (++begin != end) {
        if (!(**result == **begin)) {
            *(++result) = *begin;
        }
    }                          
    return ++result;
}

/* 
    Read input file and store its contents in the passed string    
*/
void read_text (string& incoming, string filename) {

    ifstream source (filename);
    string line;

    if(!source.is_open()){
        cout<<"Error reading file"<<endl;
        return;
    }

    while(getline(source, line)){
        incoming.append(line);
    }

    source.close();

    /* Unlike basic_string/c_string  STL string do not contain delimeter 
    at the end, add one.*/
    incoming.append("\r\n"); 
}

/* 
    Read text input from stdin and store in the passed string    
*/
void read_text (string& incoming) {

    cout<<"Input text: "<<endl;

    /* TODO: Use generic stream defaulting to stdin*/
    while(!cin.eof()){
        string line;
        getline(cin, line);
        
        if(cin.fail()){
            break;
        }

        incoming.append(line);
    }
    /* Unlike basic_string/ c_string  STL string do not contain delimeter 
    at the end, add one.*/
    incoming.append("\r\n"); 

}


void print_time_elapsed(chrono::time_point<chrono::system_clock> start,
                        chrono::time_point<chrono::system_clock> end) {

    chrono::duration<double> elapsed_seconds = end - start;
    time_t end_time = chrono::system_clock::to_time_t(end);
    cout <<"elapsed time: " << elapsed_seconds.count() << "s\n";
}

void run(string filename) {

    string incoming;
    vector<shared_ptr<string>> split_words;
    chrono::time_point<chrono::system_clock> start, end;

    if(filename.empty()) {
        read_text(incoming);
    } else {
        read_text(incoming, filename);
    }
    
    if (incoming.empty()) {
        cout<<"Failed to read input"<<endl;
        return;
    }

    split_into_words(incoming, split_words);

    start = chrono::system_clock::now();
    mergesort(split_words);
    end = chrono::system_clock::now();

    auto it = my_unique(split_words.begin(), split_words.end());
    split_words.erase(it, split_words.end());

    print_words(split_words);
    print_time_elapsed(start, end);

}

int main(int argc, char *argv[]) {

    if(argc == 1){
        run("");
    } else {
        string filename(argv[1]);
        
        run("../test/" + filename);
    }

    return 0;
}
