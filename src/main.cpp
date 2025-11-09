#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <future>
#include <sstream>

#include "mystruct.h"

using namespace std;
using namespace chrono;

vector<ifstream> load_files(char** argv) {
    vector<ifstream> files;
    files.reserve(3);
    for(int i = 1; i < 4; i++) {
        files.emplace_back(argv[i]);
    }
    return files;
}

void start_task(MyStruct& mystruct, istream& in) {
    string task;
    while (in >> task) {
    if (task == "read") {
        int field;
            in >> field;
            mystruct.read(field);
        } else if (task == "write") {
            int field, value;
            in >> field >> value;
            mystruct.write(field, value);
        } else string result = mystruct;
    }
}

string read_all(ifstream& file) {
    string result, line;
    while (getline(file, line)) {
        result += line + '\n';
    }
    return result;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <file1> <file2> <file3>" << endl;
        return 1;
    }

    MyStruct mystruct;
    auto files = load_files(argv);

    vector<string> tasks;
    tasks.reserve(files.size());
    for (auto& f : files) tasks.push_back(read_all(f));

    for (int threads = 0; threads < 3; threads++) {
        cout << "Experiment for " << threads + 1 << " threads" << endl;
        vector<future<high_resolution_clock::duration>> results;
        results.reserve(threads + 1);
        for (int i = 0; i <= threads; i++) {
            results.emplace_back(async(launch::async, [&, i]() -> high_resolution_clock::duration {
                istringstream in(tasks[i]);
                auto start = high_resolution_clock::now();
                start_task(mystruct, in);
                auto end = high_resolution_clock::now();
                return end - start;
            }));
        }

        for (int i = 0; i <= threads; ++i) {
            auto d  = results[i].get();
            auto ms = duration_cast<milliseconds>(d).count();
            cout << "Thread " << i + 1 << ": " << ms << " ms\n";
        }
    }

    return 0;
}