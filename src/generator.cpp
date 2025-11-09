#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>

using namespace std;

enum ActionIdx { R0=0, W0, R1, W1, R2, W2, STR, ACTN };

vector<int> profile_weights(char profile) {
    switch (profile) {
        case 'a': 
            return { 5, 5, 30, 5, 24, 1, 30 };
        case 'b':
            return { 12, 12, 12, 12, 11, 11, 30 };
        case 'c':
            return { 2, 20, 3, 20, 3, 20, 30 };
        default:
            cout << "Unknown profile '" << profile << "'. Use a|b|c." << endl;
            exit(1);
    }
}

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "Usage: " << argv[0] <<" <profile:a|b|c> <threads> <ops_per_file> [seed] [filename_prefix]" << endl;
        cout << "Example: " << argv[0] << " a 3 100000 123 test1" << endl;
        return 1;
    }

    const char profile = argv[1][0];
    const int threads  = max(1, atoi(argv[2]));
    const long long ops = atoll(argv[3]);
    const unsigned seed = (argc >= 5) ? static_cast<unsigned>(stoul(argv[4])) : random_device{}();
    const string prefix = (argc >= 6) ? argv[5] : "actions";

    auto w = profile_weights(profile);
    mt19937 rng(seed);
    discrete_distribution<int> dist(w.begin(), w.end());
    uniform_int_distribution<int> valgen(100, 1000);

    for (int t = 0; t < threads; ++t) {
        string fname = prefix + "_p" + profile + "_t" + to_string(t) + ".txt";
        ofstream out(fname);
        if (!out) {
            cout << "Cannot open file: " << fname << endl;
            return 1;
        }

        for (long long i = 0; i < ops; ++i) {
            switch (dist(rng)) {
                case R0:  out << "read 0\n"; break;
                case W0:  out << "write 0 " << valgen(rng) << "\n"; break;
                case R1:  out << "read 1\n"; break;
                case W1:  out << "write 1 " << valgen(rng) << "\n"; break;
                case R2:  out << "read 2\n"; break;
                case W2:  out << "write 2 " << valgen(rng) << "\n"; break;
                case STR: out << "string\n"; break;
            }
        }
        cout << "Wrote " << fname << " (" << ops << " ops)\n";
    }
    return 0;
}
