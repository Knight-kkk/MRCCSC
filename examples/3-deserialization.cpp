#include "../hfiles/json.hpp"
using json = nlohmann::json;

#include <iostream>
using namespace std;
#include <string>

int main(){
    string temp = "{\"ages\":[1,2,3,4],\"sentence\":[[4,\"tes\"],[3,\"hello\"]]}";
    json js = json::parse(temp);
    std::cout << js["ages"] << endl;
    std::cout << js["sentence"] << endl;
    return 0;
}