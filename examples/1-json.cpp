#include "../hfiles/json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <string>
using namespace std;

int main(){
    json temp;
    temp["name"] = {"zhangsan", "lisi", "wangwu"};
    temp["zhangsan"]["age"] = 20;
    temp["lisi"]["age"] = 19;
    string str = temp.dump();   // dump方法将json转化为string 
    cout << str << endl;
}