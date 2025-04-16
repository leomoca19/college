#pragma once
#include <fstream>
#include <map>
#include <sstream>
#include <string>

using namespace std;

class Inventory {
 public:
  const char* fname;
  map<string, unsigned short> items;

  Inventory(const char* fname) : fname(fname) {
    ifstream file(fname);
    if (file.is_open()) {
      string str;
      while (getline(file, str)) {
        int delimPos = str.find(' ');
        string upc = str.substr(0, delimPos);
        int qty = stoi(str.substr(delimPos, str.size()));
        items.insert(pair<string, int>(upc, qty));
      }
      file.close();
    }
  }

  string print() const {
    ostringstream oss;
    oss << fname
        << (!items.size() ? " is empty"
                          : " has " + to_string(items.size()) + " items")
        << endl;

    for (const auto& item : items) {
      oss << item.first << ' ' << item.second << endl;
    }
    return oss.str();
  }

  void save() const {
    for (const auto& item : items) {
      ofstream file(fname);
      file << item.first << ' ' << item.second << endl;
    }
  }

  void add(const string& upc, const unsigned short& qty) {
    // find upc, then create new pair or update pair
    if (items.contains(upc)) {
      items[upc] = +qty;
    } else {
      items.insert(pair<string, int>(upc, qty));
    }
  }
};