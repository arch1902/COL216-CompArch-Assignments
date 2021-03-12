// Use "make run" to compile and run on "text.txt"
#include <iostream>
#include<string>
#include<algorithm>
#include<fstream>
#include<vector>
#include<map>
#include<regex>
#include<sstream>
#include<fstream>
#include <cstddef>
using namespace std;

fstream file1 ;
map<string,int> reg;
map<int,vector<string>> params;
map<string,int> label;
vector<string> operations = {"add","sub","mul","beq","bne","slt","lw","sw","addi"};
vector<string> registers = {"$r0","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1",
"$t2","$t3","$t4","$t5","$t6","$t7","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8",
"$t9","$k0","$k1","$gp","$sp","$s8","$ra"};
regex n("[0-9]+");
regex l("([A-Z|a-z])[A-Z|a-z|0-9|_]+");

int INSTRUCTION_MEMORY = 2^19;
int DATA_MEMORY = 2^19;

fstream out("out.txt");

int findfirst(string s, char c){
    for (int i = 0;i<s.size();i++){
        if (s[i]==c){
            return i;
        }
    }
    return -1;
}
string trim(string str){
    return regex_replace(str, regex("(^[ ]+)|([ ]+$)"),"");
}

void validator(vector<string> V, string s,int l){
    for( auto j : V){
        if (j == s){
            return;
        }
    }
    cout<<"Invalid input "<<s<<" found on line"<<l;
    exit(-1);
}

void print(){
    out<<"R0  [r0] = "<<reg["$r0"]<<endl;
    out<<"R1  [at] = "<<reg["$at"]<<endl;
    out<<"R2  [v0] = "<<reg["$v0"]<<endl;
    out<<"R3  [v1] = "<<reg["$v1"]<<endl;
    out<<"R4  [a0] = "<<reg["$a0"]<<endl;
    out<<"R5  [a1] = "<<reg["$a1"]<<endl;
    out<<"R6  [a2] = "<<reg["$a2"]<<endl;
    out<<"R7  [a3] = "<<reg["$a3"]<<endl;
    out<<"R8  [t0] = "<<reg["$t0"]<<endl;
    out<<"R9  [t1] = "<<reg["$t1"]<<endl;
    out<<"R10 [t2] = "<<reg["$t2"]<<endl;
    out<<"R11 [t3] = "<<reg["$t3"]<<endl;
    out<<"R12 [t4] = "<<reg["$t4"]<<endl;
    out<<"R13 [t5] = "<<reg["$t5"]<<endl;
    out<<"R14 [t6] = "<<reg["$t6"]<<endl;
    out<<"R15 [t7] = "<<reg["$t7"]<<endl;
    out<<"R16 [s0] = "<<reg["$s0"]<<endl;
    out<<"R17 [s1] = "<<reg["$s1"]<<endl;
    out<<"R18 [s2] = "<<reg["$s2"]<<endl;
    out<<"R19 [s3] = "<<reg["$s3"]<<endl;
    out<<"R20 [s4] = "<<reg["$s4"]<<endl;
    out<<"R21 [s5] = "<<reg["$s5"]<<endl;
    out<<"R22 [s6] = "<<reg["$s6"]<<endl;
    out<<"R23 [s7] = "<<reg["$s7"]<<endl;
    out<<"R24 [t8] = "<<reg["$t8"]<<endl;
    out<<"R25 [t9] = "<<reg["$t9"]<<endl;
    out<<"R26 [k0] = "<<reg["$k0"]<<endl;
    out<<"R27 [k1] = "<<reg["$k1"]<<endl;
    out<<"R28 [gp] = "<<reg["$gp"]<<endl;
    out<<"R29 [sp] = "<<reg["$sp"]<<endl;
    out<<"R30 [s8] = "<<reg["$s8"]<<endl;
    out<<"R31 [ra] = "<<reg["$ra"]<<endl<<endl;
}

void dectohex(int n){ 
    char hexaDeciNum[100]; 
    int i = 0; 
    while(n!=0) 
    {    
        int temp  = 0; 
        temp = n % 16; 
        if(temp < 10) 
        { 
            hexaDeciNum[i] = temp + 48; 
            i++; 
        } 
        else
        { 
            hexaDeciNum[i] = temp + 55; 
            i++; 
        } 
        n = n/16; 
    } 
    for(int j=i-1; j>=0; j--) 
        out << hexaDeciNum[j];
}

void validator_add(string s, int l, string instruction){
        stringstream s_stream(s);
        int count = 0;
        string substr;
        while(s_stream.good()) {
            getline(s_stream, substr, ',');
            substr = trim(substr);
            if (instruction=="addi" && count == 2){
                    if(!regex_match(substr,n)){
                        cout<<"Invalid second operator "<<substr<<"for operator "<<instruction;
                    }
            }
            validator(registers,substr,l+1);
            params[l].push_back(substr);
            count +=1;
        }
        if (count != 3){
            cout<<"Invalid number of operands for "<< instruction <<" operator on line "<<l+1;
            exit(-1);
        }
}

void validator_beq(string s, int l, string instruction){
        stringstream s_stream(s);   
        int count = 0;
        string substr;
        while(s_stream.good()) {
            getline(s_stream, substr, ',');
            substr = trim(substr);
            cout<<substr<<"\n";
            if(count==0){validator(registers,substr,l+1);}
            if(count ==1){
                if(substr[0] =='$'){validator(registers,substr,l+1);}
                else{
                    if(!regex_match(substr,n)){
                        cout<<"Invalid second operator "<<substr<<"for operator "<<instruction;
                    }
                }
            }
            params[l].push_back(substr);
            count +=1;
        }
        if (count != 3){
            cout<<"Invalid operands for "<<instruction <<" operator on line "<<l+1;
            exit(-1);
        }
}

void validator_lw(string s, int l, string instruction){
        cout<<"In lw \n";
        stringstream s_stream(s);
        int count = 0;
        string substr;
        while(s_stream.good()) {    
            getline(s_stream, substr, ',');
            substr = trim(substr);
            if(count==0){validator(registers,substr,l+1);params[l].push_back(substr);}
            if(count ==1){
                //off(reg)
                size_t q;
                q = substr.find_first_of('(');
                if(q==string::npos){cout<<"Invalid Operand for "<<instruction<<" on line "<<l+1<<"\n";exit(-1);}
                string lh = substr.substr(0,q);
                //cout<<lh<<"\n";
                if (regex_match(lh,n)){
                    params[l].push_back(lh);
                }else{cout<<"Invalid Operand for "<<instruction<<" on line "<<l+1;exit(-1);}
                //(register) 
                string rh = substr.substr(q);
                if (rh[0] != '(' || rh.back() != ')'){cout<<"Invalid Operand for "<<instruction<<" on line "<<l+1;exit(-1);}
                rh = trim(rh.substr(1,rh.size()-2));
                validator(registers,rh,l);
                params[l].push_back(rh);
            }
            count +=1;
        }
        if (count != 2){
            cout<<"Invalid operands for "<<instruction <<" operator on line "<<l+1;
            exit(-1);
        }
}


int main(int argc, char *argv[]) {
	// your code goes here

    file1.open(argv[1]);
    vector<string> instructions;
    string line;
    int num=0;
    int x;
 
    string Instruction;
    while(getline(file1,line)){
        line = trim(line);
        instructions.push_back(line);
        x = findfirst(line,'$');
        Instruction = line;
        cout<<Instruction<<"FFFFFFFFFFFF\n";
        cout<<x<<"\n";
        //cout<<(string::npos);
        //cout<<(x==string::npos);
        if(true){
            cout<<"Did not find FFFFFFFFFFFFFFFFF $";
            if (Instruction.substr(0,2) == "j "){
                Instruction = "j";
                params[num].push_back(Instruction);
                string oprd = trim(Instruction.substr(2));
                params[num].push_back(oprd);
            }else if(Instruction.back() == ':'){
                string b = Instruction.substr(0,Instruction.size()-1);
                if (regex_match(b,l)){
                     label[b] = num;
                     params[num].push_back(b);
                 }else{
                     cout<< "Invaid Label format on line"<<num+1;
                     exit(-1);
                 }
             }else{
                cout<< "Invalid Input "<<Instruction<<" on line "<<num+1;
                exit(-1);
            }
        }else{
            cout<<"found $\n";
            Instruction = trim(line.substr(0,x));
            cout<<Instruction<<"\n";
            validator(operations,Instruction,num+1);           
            params[num].push_back(Instruction);
            string operands = trim(line.substr(x));
            cout<<operands<<"\n";
            if (Instruction == "add"||Instruction =="sub"||Instruction =="mul"||Instruction =="slt"||Instruction =="addi"){  //$t1, $t2, $t3 
                cout<<"Am in wrong place"<<"\n";
                validator_add(operands,num,Instruction);
            }else if (Instruction=="beq"||Instruction =="bne"){
                validator_beq(operands,num,Instruction);
            }else if (Instruction == "lw"||Instruction =="sw"){
                cout<<"going lw \n";
                validator_lw(operands,num,Instruction);
            }else{
                continue;
            }
        }
        num++;
    }
     for (auto const& j : params){
         int c = j.first;
         vector<string> v = j.second;
         cout<<c<<"\n";
         for(auto j: v){
             cout<<string(j)<<" ";
         }
         cout<<"\n";
    }
    file1.close();

    // string curr;

    // int pc = 0;
    // while(pc<num){
    //     curr=instructions[pc];
    //     Instruction=params[pc][0];

    //     if(Instruction=="add") {
    //         reg[params[pc][1]]=reg[params[pc][2]]+reg[params[pc][3]];
    //         pc++;

    //     } else if (Instruction=="sub"){
    //         reg[params[pc][1]]=reg[params[pc][2]]-reg[params[pc][3]]; 
    //         pc++;

    //     } else if (Instruction=="mul"){
    //         reg[params[pc][1]]=reg[params[pc][2]]*reg[params[pc][3]]; 
    //         pc++;
            
    //     } else if (Instruction=="beq"){
    //         if(reg[params[pc][1]]==reg[params[pc][2]]){
    //             pc = stoi(reg[params[pc][3]]);
    //         }else {
    //             pc++;
    //         }
    //     } else if (Instruction=="bne"){
    //         if(reg[params[pc][1]]!=reg[params[pc][2]]){
    //             pc = stoi(reg[params[pc][3]]);
    //         }else {
    //             pc++;
    //         }            
    //     } else if (Instruction=="slt"){
    //         if(reg[params[pc][1]]<reg[params[pc][2]]){
    //             reg[params[pc][3]] = 1;
    //         }else {
    //             reg[params[pc][3]] = 0;
    //         }
    //         pc++; 
    //     } else if (Instruction=="j"){
    //         pc = params[pc][1];
    //     } else if (Instruction=="lw"){
    //         reg[params[pc][1]]=stoi(params[pc][2]);
    //         pc++;
            
    //     } else if (Instruction=="sw"){
          
            
    //     } else if (Instruction=="addi"){
           
            
    //     } else {
    //         out << "Wrong Instruction !";
    //     }
        
    //     //Process
        
    //     out<<"Integer Register : \n"<<endl;
    //     print(); 
    // }
    // out.close();
	return 0;
}

/*

R0  [r0] = 0
R1  [at] = 0
R2  [v0] = 0
R3  [v1] = 0
R4  [a0] = 0
R5  [a1] = 0
R6  [a2] = 2147483204
R7  [a3] = 0
R8  [t0] = 0
R9  [t1] = 0
R10 [t2] = 0
R11 [t3] = 0
R12 [t4] = 0
R13 [t5] = 0
R14 [t6] = 0
R15 [t7] = 0
R16 [s0] = 0
R17 [s1] = 0
R18 [s2] = 0
R19 [s3] = 0
R20 [s4] = 0
R21 [s5] = 0
R22 [s6] = 0
R23 [s7] = 0
R24 [t8] = 0
R25 [t9] = 0
R26 [k0] = 0
R27 [k1] = 0
R28 [gp] = 268468224
R29 [sp] = 2147483196
R30 [s8] = 0
R31 [ra] = 0

add, sub, mul, beq, bne, slt, j, lw, sw,addi

*/