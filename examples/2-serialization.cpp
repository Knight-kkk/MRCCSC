#include "../hfiles/json.hpp"
using json = nlohmann::json;

#include <iostream>
using namespace std;

#include <map>
#include <string>

// 可以将容器直接序列化为json
int main(){
    vector<int> ages = {1,2,3,4};
    json js;
    js["ages"] = ages;
    unordered_map<int,string> umap;
    umap.insert(make_pair<int,string>(3,"hello"));
    umap.insert({4,"test"});
    js["sentence"] = umap;
    cout << js.dump() << endl;
    return 0;
}