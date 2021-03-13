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
#include<cmath>
using namespace std;

fstream file1; 
ofstream out;
map<string,int> reg;
map<int,vector<string>> params;
map<string,int> label;
map<int,int> data_memory;
map<string,int> statistics;
vector<string> operations = {"add","sub","mul","beq","bne","slt","lw","sw","addi"};
vector<string> registers = {"$zero","$r1","$r2","$r3","$r4","$r5","$r6","$r7","$r8","$r9",
"$r10","$r11","$r12","$r13","$r14","$r15","$r16","$r17","$r18","$r19","$r20","$r21","$r22","$r23","$r24",
"$r25","$r26","$r27","$r28","$r29","$r30","$r31"};
regex n("[-]?[0-9]+");  //for checking if a string is convertible to an Integer
regex l("([A-Z|a-z])[A-Z|a-z|0-9|_]*"); // for Label
int n_total;

int INSTRUCTION_MEMORY = pow(2,17); // Memory is word Addressable hence it has 2^19 Bytes
int DATA_MEMORY = pow(2,17);


const std::string WHITESPACE = " \n\r\t\f\v";
 
std::string ltrim(const std::string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string trim(const std::string& s)
{
    return rtrim(ltrim(s));
}

void validator(vector<string> V, string s,int l){
    for( auto j : V){
        if (j == s){
            return;
        }
    }
    cout<<"Invalid input "<<s<<" found on line "<<l<<endl;
    exit(-1);
}

void print(){
    out<<"R0 [zero] = "<<reg["$zero"]<<endl;
    out<<"R1  [r1]  = "<<reg["$r1"]<<endl;
    out<<"R2  [r2]  = "<<reg["$r2"]<<endl;
    out<<"R3  [r3]  = "<<reg["$r3"]<<endl;
    out<<"R4  [r4]  = "<<reg["$r4"]<<endl;
    out<<"R5  [r5]  = "<<reg["$r5"]<<endl;
    out<<"R6  [r6]  = "<<reg["$r6"]<<endl;
    out<<"R7  [r7]  = "<<reg["$r7"]<<endl;
    out<<"R8  [r8]  = "<<reg["$r8"]<<endl;
    out<<"R9  [r9]  = "<<reg["$r9"]<<endl;
    out<<"R10 [r10] = "<<reg["$r10"]<<endl;
    out<<"R11 [r11] = "<<reg["$r11"]<<endl;
    out<<"R12 [r12] = "<<reg["$r12"]<<endl;
    out<<"R13 [r13] = "<<reg["$r13"]<<endl;
    out<<"R14 [r14] = "<<reg["$r14"]<<endl;
    out<<"R15 [r15] = "<<reg["$r15"]<<endl;
    out<<"R16 [r16] = "<<reg["$r16"]<<endl;
    out<<"R17 [r17] = "<<reg["$r17"]<<endl;
    out<<"R18 [r18] = "<<reg["$r18"]<<endl;
    out<<"R19 [r19] = "<<reg["$r19"]<<endl;
    out<<"R20 [r20] = "<<reg["$r20"]<<endl;
    out<<"R21 [r21] = "<<reg["$r21"]<<endl;
    out<<"R22 [r22] = "<<reg["$r22"]<<endl;
    out<<"R23 [r23] = "<<reg["$r23"]<<endl;
    out<<"R24 [r24] = "<<reg["$r24"]<<endl;
    out<<"R25 [r25] = "<<reg["$r25"]<<endl;
    out<<"R26 [r26] = "<<reg["$r26"]<<endl;
    out<<"R27 [r27] = "<<reg["$r27"]<<endl;
    out<<"R28 [r28] = "<<reg["$r28"]<<endl;
    out<<"R29 [r29] = "<<reg["$r29"]<<endl;
    out<<"R30 [r30] = "<<reg["$r30"]<<endl;
    out<<"R31 [r31] = "<<reg["$r31"]<<endl<<endl;
}

string dectohex(int n){
    stringstream ss; 
    ss << hex << n; 
    string res = ss.str(); 
    return res;    
}

void print_hex(){
    out<<"R0 [zero] = "<<dectohex(reg["$zero"])<<endl;
    out<<"R1  [r1]  = "<<dectohex(reg["$r1"])<<endl;
    out<<"R2  [r2]  = "<<dectohex(reg["$r2"])<<endl;
    out<<"R3  [r3]  = "<<dectohex(reg["$r3"])<<endl;
    out<<"R4  [r4]  = "<<dectohex(reg["$r4"])<<endl;
    out<<"R5  [r5]  = "<<dectohex(reg["$r5"])<<endl;
    out<<"R6  [r6]  = "<<dectohex(reg["$r6"])<<endl;
    out<<"R7  [r7]  = "<<dectohex(reg["$r7"])<<endl;
    out<<"R8  [r8]  = "<<dectohex(reg["$r8"])<<endl;
    out<<"R9  [r9]  = "<<dectohex(reg["$r9"])<<endl;
    out<<"R10 [r10] = "<<dectohex(reg["$r10"])<<endl;
    out<<"R11 [r11] = "<<dectohex(reg["$r11"])<<endl;
    out<<"R12 [r12] = "<<dectohex(reg["$r12"])<<endl;
    out<<"R13 [r13] = "<<dectohex(reg["$r13"])<<endl;
    out<<"R14 [r14] = "<<dectohex(reg["$r14"])<<endl;
    out<<"R15 [r15] = "<<dectohex(reg["$r15"])<<endl;
    out<<"R16 [r16] = "<<dectohex(reg["$r16"])<<endl;
    out<<"R17 [r17] = "<<dectohex(reg["$r17"])<<endl;
    out<<"R18 [r18] = "<<dectohex(reg["$r18"])<<endl;
    out<<"R19 [r19] = "<<dectohex(reg["$r19"])<<endl;
    out<<"R20 [r20] = "<<dectohex(reg["$r20"])<<endl;
    out<<"R21 [r21] = "<<dectohex(reg["$r21"])<<endl;
    out<<"R22 [r22] = "<<dectohex(reg["$r22"])<<endl;
    out<<"R23 [r23] = "<<dectohex(reg["$r23"])<<endl;
    out<<"R24 [r24] = "<<dectohex(reg["$r24"])<<endl;
    out<<"R25 [r25] = "<<dectohex(reg["$r25"])<<endl;
    out<<"R26 [r26] = "<<dectohex(reg["$r26"])<<endl;
    out<<"R27 [r27] = "<<dectohex(reg["$r27"])<<endl;
    out<<"R28 [r28] = "<<dectohex(reg["$r28"])<<endl;
    out<<"R29 [r29] = "<<dectohex(reg["$r29"])<<endl;
    out<<"R30 [r30] = "<<dectohex(reg["$r30"])<<endl;
    out<<"R31 [r31] = "<<dectohex(reg["$r31"])<<endl<<endl;
}

//Operator $reg1,reg2,reg3/Int
void validator_add(string s, int l, string instruction){
        stringstream s_stream(s);
        int count = 0;
        string substr;
        while(s_stream.good()) {
            getline(s_stream, substr, ',');
            substr = trim(substr);
            if (instruction=="addi" && count == 2){
                    if(!regex_match(substr,n)){
                        cout<<"Invalid second operator "<<substr<<"for operator "<<instruction<<endl;
                        exit(-1);
                    }
            }else if (count == 2){
                if(substr[0] =='$'){validator(registers,substr,l+1);}
                else{
                    if(!regex_match(substr,n)){
                        cout<<"Invalid second operator "<<substr<<"for operator "<<instruction<<endl;
                        exit(-1);
                    }
                }
            }
            else validator(registers,substr,l+1);
            params[l].push_back(substr);
            count +=1;
        }
        if (count != 3){
            cout<<"Invalid number of operands for "<< instruction <<" operator on line "<<l+1<<endl;
            exit(-1);
        }
}
// beq $reg1,$reg2/Int,label
void validator_beq(string s, int l, string instruction){
        stringstream s_stream(s);   
        int count = 0;
        string substr;
        while(s_stream.good()) {
            getline(s_stream, substr, ',');
            substr = trim(substr);
            if(count==0){validator(registers,substr,l+1);}
            if(count ==1){
                if(substr[0] =='$'){validator(registers,substr,l+1);}
                else{
                    if(!regex_match(substr,n)){
                        cout<<"Invalid second operator "<<substr<<"for operator "<<instruction<<endl;
                        exit(-1);
                    }
                }
            }
            params[l].push_back(substr);
            count +=1;
        }
        if (count != 3){
            cout<<"Invalid operands for "<<instruction <<" operator on line "<<l+1<<endl;
            exit(-1);
        }
}
//lw $reg1,off($reg2)
void validator_lw(string s, int l, string instruction){

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
                if(q==string::npos){cout<<"Invalid Operand for "<<instruction<<" on line "<<l+1<<endl;exit(-1);}
                string lh = substr.substr(0,q);
                // cout<<lh<<"\n";
                if(lh==""){
                    params[l].push_back("0");
                }
                else if (regex_match(lh,n)){
                    params[l].push_back(lh);
                }else{cout<<"Invalid Operand for "<<instruction<<" on line "<<l+1<<endl;exit(-1);}
                //(register) 
                string rh = substr.substr(q);
                if (rh[0] != '(' || rh.back() != ')'){cout<<"Invalid Operand for "<<instruction<<" on line "<<l+1<<endl;exit(-1);}
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
    string input_file = argv[1];
    out.open(input_file+"_output.txt");
    vector<string> instructions;
    string line;
    int num=0;
    size_t x;
 
    string Instruction;
    while(getline(file1,line)){
        line = trim(line);
        instructions.push_back(line);
        if(line == ""){
            num++;
            continue;
        }
        x = line.find_first_of('$');
        Instruction = line;
        //cout<<(string::npos);
        //cout<<(x==string::npos);
        if(x==string::npos){
            
            if (Instruction.substr(0,2) == "j "){
                params[num].push_back("j");
                string oprd = trim(Instruction.substr(2));
                params[num].push_back(oprd);
            }else if(Instruction.back() == ':'){
                string b = trim(Instruction.substr(0,Instruction.size()-1));
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
            
            Instruction = trim(line.substr(0,x));  
            //cout<<Instruction<<endl;          
            validator(operations,Instruction,num+1);           
            params[num].push_back(Instruction);
            string operands = trim(line.substr(x));            
            if (Instruction == "add"||Instruction =="sub"||Instruction =="mul"||Instruction =="slt"||Instruction =="addi"){  //$t1, $t2, $t3                
                validator_add(operands,num,Instruction);
            }else if (Instruction=="beq"||Instruction =="bne"){
                validator_beq(operands,num,Instruction);
            }else if (Instruction == "lw"||Instruction =="sw"){
                
                validator_lw(operands,num,Instruction);
            }else{
                continue;
            }
        }
        //cout<<num<<" "<<INSTRUCTION_MEMORY<<endl;
        num++;
        if (num>INSTRUCTION_MEMORY){
            cout<<"Error! Instruction memory limit exceeded"<<endl;
            exit(-1);
        }
        // instruction memory nums X 4 bytes
    }
    file1.close();
// PARSING ENDS ------------------------------------------------------------------------------------
    // for (auto const& j : params){
    //      int c = j.first;
    //      vector<string> v = j.second;
    //      cout<<c<<"\n";
    //      for(auto j: v){
    //          cout<<string(j)<<" ";
    //      }
    //      cout<<"\n";
    // }
    // for(auto const& j : label){
    //     cout<<j.first<<"->"<<j.second<<"   ";
    // }
    // cout<<endl;
    int pc = 0;
    while(pc<num){
        if(instructions[pc]==""){pc++;continue;}
        n_total++;
        //cout<<pc<<endl;
        out<<"Integer Register : "<<pc+1<<"   "<<instructions[pc]<<" \n"<<endl;
        Instruction=params[pc][0];
        statistics[Instruction] ++;
        if(Instruction=="add") {
            int y = (params[pc][3][0]=='$') ? reg[params[pc][3]] : stoi(params[pc][3]);
            reg[params[pc][1]]=reg[params[pc][2]]+y;
            pc++;
        } else if (Instruction=="sub"){
            int y = (params[pc][3][0]=='$') ? reg[params[pc][3]] : stoi(params[pc][3]);
            reg[params[pc][1]]=reg[params[pc][2]]-y;
            pc++;
        } else if (Instruction=="mul"){
            int y = (params[pc][3][0]=='$') ? reg[params[pc][3]] : stoi(params[pc][3]);
            reg[params[pc][1]]=reg[params[pc][2]]*y;
            pc++;
        } else if (Instruction=="beq"){
            if (label.find(params[pc][3]) == label.end()){
                cout << "Invalid Label on line "<<pc+1<<endl;
                exit(-1);
            }
            int y = (params[pc][2][0]=='$') ? reg[params[pc][2]] : stoi(params[pc][2]);
            //cout<<"\n"<<y<<" "<<reg[params[pc][1]];
            if (reg[params[pc][1]] == y){
                pc = label[params[pc][3]];
                print_hex();
                continue;
            }
            pc++;
        } else if (Instruction=="bne"){
            if (label.find(params[pc][3]) == label.end()){
                cout << "Invalid Label on line "<<pc+1<<endl;
                exit(-1);
            }
            int y = (params[pc][2][0]=='$') ? reg[params[pc][2]] : stoi(params[pc][2]);
            if (reg[params[pc][1]] != y){
                pc = label[params[pc][3]];
                print_hex();
                continue;
            }
            pc++;    
        } else if (Instruction=="slt"){
            int y = (params[pc][3][0]=='$') ? reg[params[pc][3]] : stoi(params[pc][3]);
            if(reg[params[pc][2]]< y ){
                reg[params[pc][1]] = 1;
            }else {
                reg[params[pc][1]] = 0;
            }
            pc++; 
        } else if (Instruction=="j"){
            if (label.find(params[pc][1]) == label.end()){
                cout << "Invalid Label on line "<<pc+1;
                exit(-1);
            }
            pc = label[params[pc][1]];
        } else if (Instruction=="lw"){
            int offset = stoi(params[pc][2]);
            if((offset+reg[params[pc][3]])%4 != 0 || (offset+reg[params[pc][3]])<0 ||(offset+reg[params[pc][3]])>DATA_MEMORY){
                cout<<"Program tried to access invalid memory location. "<<endl;
                exit(-1);
            }
            reg[params[pc][1]] = data_memory[(offset+reg[params[pc][3]])];
            pc++;           
        } else if (Instruction=="sw"){
        // 0 -1 , 4 - 189, 8 - 2537
            int offset = stoi(params[pc][2]);
            if((offset+reg[params[pc][3]])%4 != 0 || (offset+reg[params[pc][3]])<0  || (offset+reg[params[pc][3]])>DATA_MEMORY){
                cout<<"Program tried to access invalid memory location."<<endl;
                exit(-1);
            }
            data_memory[(offset+reg[params[pc][3]])] = reg[params[pc][1]];
            pc++;
        } else if (Instruction=="addi"){
            reg[params[pc][1]]=reg[params[pc][2]]+stoi(params[pc][3]);
            pc++;     
        } else if( label.find(params[pc][0]) != label.end() ){
             pc ++;
             n_total --;
        }else{
            continue;
        }     
        print_hex();
    }
    cout<<"Number of clock cycles "<<n_total<<endl;
    out<<"Number of clock cycles "<<n_total<<endl;
    cout<<"Instruction Memory Used "<<num*4 <<" Bytes"<<endl;
    out<<"Instruction Memory Used "<<num*4 <<" Bytes"<<endl;
    cout<<"Data Memory Used "<<data_memory.size()*4 <<" Bytes"<<endl;
    out<<"Data Memory Used "<<data_memory.size()*4 <<" Bytes"<<endl;
    cout<<"Number of times each instruction was executed :"<<endl;
    out<<"Number of times each instruction was executed :"<<endl;      
    for (auto j: operations){
        cout<< j<< " ->"<<statistics[j]<<endl;
        out<< j << " ->"<<statistics[j]<<endl;
    }
    cout<< "j"<< " ->"<<statistics["j"]<<endl;
    out<< "j" << " ->"<<statistics["j"]<<endl;
    out.close();
	return 0;
}
