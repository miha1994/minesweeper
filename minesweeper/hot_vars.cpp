#include "hot_vars.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

___hot_vars::___hot_vars () {
    ifstream in;
    in.open ("assets/inf/hot_vars.txt");
    string s;
    int state = -1;
    while (in >> s) {
        if (s.find_first_of ("0123456789") == string::npos) {
            if (s == "double") {
                state = 0;
            } else if (s == "int") {
                state = 1;
            } else if (s == "string") {
                state = 2;
            } else {
                state = -1;
            }
        } else {
            switch (state) {
            case 0:
                double_.push_back (stod (s));
                break;
            case 1:
                int_.push_back (stoi (s));
                break;
            case 2:
                getline (in, s);
                string_.push_back (s);
                break;
            }
        }
    }
}

___hot_vars hot_vars;