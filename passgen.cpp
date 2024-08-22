#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <cerrno>
using namespace std;

string generate(int length, string charset);

// Function to copy text to clipboard using xclip (Linux specific)
int copyToClipboardWindows(const string& text);
int copyToClipboardXClip(const string& text);

void helpMenu();

bool checkMissingArguments(int argc, int i, char **argv, bool &error);
bool checkNext(int i, char **argv, bool &error);

const string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;:,.<>?";


int main(int argc, char** argv)
{
    if (argc <= 1) {
        helpMenu();
        return 1;
    }

    int length = 10;
    string pass;
    bool copy = false;
    bool output = false;
    bool generateBool = false;
    bool error = false;
    string pathStr;
    string charset = CHARSET;

    for (int i = 1; i < argc; ++i) {

        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {

            helpMenu();
            return 0; // no operations past this point

        } else if (strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--generate") == 0) {

            if (!error) generateBool = true;

        } else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--length") == 0) {

            if (checkMissingArguments(argc, i, argv, error)) {
                length = stoi(argv[i + 1]);
                ++i;
            }

        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--copy") == 0) {

            copy = true;

        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {

            if (checkMissingArguments(argc, i, argv, error)) {
                output = true;
                pathStr = argv[i + 1];
                ++i;
            }

        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--set")  == 0) {

            if (checkMissingArguments(argc, i, argv, error)) {
                charset = argv[i + 1];
                ++i;
            }

        } else {

            cerr << "Error: Unknown option: " << argv[i] << "\n";
            helpMenu();
            return 1;
        }
    }

    if (generateBool && !error) {
        pass = generate(length, charset);
        cout << pass << "\n";

        if (copy) {
            copyToClipboardXClip(pass);
        }

        if (output) {
            FILE *fp;
            fp = fopen(pathStr.c_str(), "w");
            if (fp == NULL) {
                cerr << "Error opening file: " << strerror(errno) << "\n";
                return 1;
            }
            fprintf(fp, "%s", pass.c_str());
            fclose(fp);
        }
    }

    return 0;
}

bool checkNext(int i, char **argv, bool &error) {

    string next = argv[i + 1];
    if (next[0] == '-') {
        if (!error) helpMenu();
        cerr << "Error: Missing argument for option " << argv[i] << "\n";
        error = true;
        return false;
    }
    return true;

}


bool checkMissingArguments(int argc, int i, char **argv, bool &error) {
    if (i + 1 >= argc) {
        helpMenu();
        cerr << "Error: Missing argument for option " << argv[i] << "\n";
        error = true;
        return false;
    }

    return checkNext(i, argv, error);

}

void helpMenu() {
    cout << "Usage: password [options]\n";
    cout << "Options:\n";
    cout << "\t-h, --help\t\tDisplay this help menu\n";
    cout << "\t-g, --generate\t\tGenerate a random password\n";
    cout << "\t-l, --length\t\tSet the length of the generated password\n";
    cout << "\t-c, --copy\t\tCopy the password to clipboard\n";
    cout << "\t-o, --output\t\tOutput to file path\n";
    cout << "\t-s, --set\t\tSet the charset of the generated password\n";
    cout << "\t./password -g -c -l 15 \tGenerates a password of length 15 and copies it\n";
}

string generate(int length, string charset) {

    string result;

    srand(time(nullptr)); // Seed the random number generator

    for (int i = 0; i < length; ++i) {
        result += charset[rand() % charset.length()]; // random index
    }

    return result;

}

int copyToClipboardXClip(const string& text) {

    FILE* pipe = popen("xclip -selection clipboard", "w");
    if (!pipe) {
        return 1;
    }
    if (fprintf(pipe, "%s", text.c_str()) < 0) {
        cerr << "Error: Unable to write to pipe to xclip: " << strerror(errno) << "\n";
        pclose(pipe);
        return 1;
    }
    if (pclose(pipe) == -1) {
        cerr << "Error: Unable to close pipe to xclip: " << strerror(errno) << "\n";
        return 1;
    }
    return 0;
}