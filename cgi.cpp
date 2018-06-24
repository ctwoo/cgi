
#include "cgi.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <utility>

using std::string;
using std::size_t;

size_t cgi::read() {
    auto rm = std::getenv("REQUEST_METHOD");
    if (rm == nullptr) {
        std::cout << jsonify("CGI Error: No method specified.");
        return 0;
    }
    string method(rm);
    if (method.size() < 3 || method == "(null)") {
        std::cout << "CGI Error: Invalid method specified [" << method << "].\n";
        return 0;
    }
    if (method == "GET") {
        return parse_query_string(string(std::getenv("QUERY_STRING")));
    } else if (method == "POST") {
        std::string line;
        std::string doc;
        constexpr auto limit = 1024u * 1024; 
        while (getline(std::cin, line) && doc.size() < limit) {
            doc += line;
            doc.append(1, '\n');
            line.clear();
        }
        kvp.emplace("POST content", doc);
        return doc.size();

    }
    std::cout << "CGI Error: Unsupported method [" << method << "].\n";
    return 0;
}

size_t cgi::parse_query_string(const string& qs) {
    if (qs.empty()) return 0;

    std::istringstream iss(qs);
    string pair;
    while (std::getline(iss, pair, '&')) { 
        auto pos = pair.find('=');
        if (pos != std::string::npos) {
            kvp[pair.substr(0,pos)] = pair.substr(pos+1);
        }
    }
    return kvp.size();
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

/*
int main(int argc, char** argv) {
  cgi input;
  size_t count = 0;

  for (int i=1; i < argc; ++i) {
    // for testing, allow setting query string on the command line
    if (std::strncmp(argv[i], "QUERY_STRING", 12) == 0) {
        setenv("REQUEST_METHOD", "GET", true);
        string qs(argv[i]);
        qs.erase(qs.begin(), qs.begin()+13);
        setenv("QUERY_STRING", qs.c_str(), true);
    }
  }
  count = input.read();
  std::cout << jsonify(input.kvp);
  return count;
}
*/


