#include "util.h"

#include <algorithm>
#include <map>
#include <string>
#include <sstream>
#include <utility>

using std::string;
using std::size_t;

char to_hex(const char code) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
}

char from_hex(const char ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

string decode(const string& source) {
    // TODO: this would much more efficient if 
    // instead of iterating over every character,
    // jump to each % and +
    std::ostringstream dest;
    for (auto it = source.begin(); it != source.end(); ) {
        if (*it == '%') {
            // if 2 or more  chars away from end of string
            if (std::distance(source.begin(), it) < int(source.length() - 2)) {
                char c = from_hex(*(it+1)) << 4 | from_hex (*(it+2));
                dest << c;
                it += 2;
            }
        } else if (*it == '+') {
            dest << ' ';
        } else {
            dest << *it;
        }
        ++it;
    }
    return dest.str();
}

string jsonify(const string& message) {
    string msg = "Content-type: application/json\n\n";
    msg += "{\"msg\":\"" + message + "\"}\n"; 
    return msg;
}

string jsonify(const std::map<string,string>& content) {
    string msg = "Content-type: application/json\n\n";
    msg += "{"; 
    for (const auto& pair: content) {
        auto ss = std::stringstream(pair.second);
        double tmp;
        ss >> tmp;
        if (!ss || ss.rdbuf()->in_avail() > 0) {
            msg += '"' + pair.first + "\":\"" + pair.second + "\","; 
        } else {
            msg += '"' + pair.first + "\":" + pair.second + ","; 
        }
    }
    msg.pop_back();
    msg += "}\n"; 
    return msg;
}

void find_replace(string* source, 
        const string& old_string, 
        const string& new_string) {
    for(std::string::size_type i = 0; (i = source->find(old_string, i)) != string::npos;)
    {
        source->replace(i, old_string.length(), new_string);
        i += new_string.length();
    }
}

