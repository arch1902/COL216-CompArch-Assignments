#include <iostream>
#include<string>
#include<algorithm>
#include<fstream>
#include<vector>
#include<map>
#include<regex>
#include<sstream>
#include<fstream>
using namespace std;

vector<string> registers = {"$r0","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1",
"$t2","$t3","$t4","$t5","$t6","$t7","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8",
"$t9","$k0","$k1","$gp","$sp","$s8","$ra"};

string trim(string str){
    return regex_replace(str, regex("(^[ ]+)|([ ]+$)"),"");
};

string ltrim(const string& s)
{
    size_t start = s.find_first_not_of(" ");
    return (start == string::npos) ? "" : s.substr(start);
}
 
string rtrim(const string& s)
{
    size_t end = s.find_last_not_of(" ");
    return (end == string::npos) ? "" : s.substr(0, end + 1);
}
void validator(vector<string> V, string s,int l){
    for( auto j : V){
        if (j == s){
            return;
        }
    }
    cout<<"Invalid character Error";
    exit(-1);
}

int main(){
    string s = "ab";
    cout<<s.back()<<"\n";
    vector<string> v = {"ab","bc","c"};
    if (v[0] == "bc"||"ab"||""){cout<<"fffffffff";}
    validator(v,s,0);
    s = "$t 2,$t3,$t1 \n";
    stringstream s_stream(trim(s));
    while(s_stream.good()) {
        string substr;
        getline(s_stream, substr, ',');
        cout<<substr<<"\n";
    }
    string o = regex_replace(s, regex("(^[ ]+)|([ ]+$)"),"");
    cout<<o<<"\n";
    size_t x = o.find_first_of(' ');
    cout<<o.substr(0,x)<<"\n";
    cout<<o.substr(x)<<"\n";
    return 0;
}