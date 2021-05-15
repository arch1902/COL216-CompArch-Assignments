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
#include<math.h>
#include<algorithm>
using namespace std;

ofstream out;
map<string,int> reg;
vector<map<string,int>> all_regs;
// map<int,vector<string>> params;
vector<map<int,vector<string>>> all_params;
// map<string,int> label;
vector<map<string,int>> all_labels;
map<int,int> data_memory;
map<string,int> statistics;
vector<string> operations = {"add","sub","mul","beq","bne","slt","lw","sw","addi"};
vector<string> registers = {"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$k0","$k1","$ra","$t0","$t1",
"$t2","$t3","$t4","$t5","$t6","$t7","$t8","$t9","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$gp",
"$fp","$sp"};
// vector<string> instructions; 
vector<vector<string>> all_instructions; //2D Vector containing all instructions 
regex n("[-]?[0-9]+");  //for checking if a string is convertible to an Integer
regex l("([A-Z|a-z])[A-Z|a-z|0-9|_]*"); // for Label
int TOTAL_DATA_MEMORY = pow(2,19); // Memory is word Addressable hence it has 2^19 Bytes
int ROW_ACCESS_DELAY  = 10;
int COL_ACCESS_DELAY = 2;
int clock_cycle = 0;
int last_buffer_row = -1;
int pc = 0;
vector<int> all_pc;
int row_activate = 0;
int row_writeback = 0;
int row_buffer_updates = 0;
int dirty_bit = 0;
int CORE_MEMORY;
vector<vector<int>> Dram_Memory;
vector<map<int,int>> all_lines_numbers;
vector<int> Row_buffer;
map<int,vector<vector<string>>> Dram_queue;
// map<string,int>  Blocking_registers;
vector<map<string,int>> all_blocking_registers;
vector<int> num_array;
bool request_issued;
int total_int_executed=0;
int rows_for_a_core;
int TOTAL_QUEUE_SIZE = 64;
int individual_queue_size;
bool Is_dram_free = true;
bool flag = false;
int stopcore = -1;
bool issued_request_computed = false;
int issue_counter;
vector<string> executing_instruction;
vector<string> issued_instruction;
vector<vector<string>> Initial_cache = {{"0"},{"0"},{"0"},{"0"},{"0"},{"0"},{"0"},{"0"},{"0"}};
// cache -> total clock cycle, counter, to be pushed back, mode_class 1, 
vector<vector<string>> cache = Initial_cache;
bool mrm_writing;
int mrm_writing_to_core;
bool assign_computed = false;
int assign_counter = 0;
int total_assign_time = 0;
map<int,string> curr_blocking_register;

// This code removes trailing and starting whitespace characters
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
//Checks if the input string is present in the register or not
void validator(vector<string> V, string s,int l, int file_number){
    for( auto j : V){
        if (j == s){
            return;
        }
    }
    cout<<"File Number "<<file_number+1<<": Invalid input "<<s<<" found on line "<<l<<endl;
    exit(-1);
}

// Prints the registers in decimal format
void print(int i){
    out<<"zero = "<<all_regs[i]["$zero"]<<endl;
    out<<"s0 = "<<all_regs[i]["$s0"]<<endl;
    out<<"s1 = "<<all_regs[i]["$s1"]<<endl;
    out<<"s2 = "<<all_regs[i]["$s2"]<<endl;
    out<<"s3 = "<<all_regs[i]["$s3"]<<endl;
    out<<"s4 = "<<all_regs[i]["$s4"]<<endl;
    out<<"s5 = "<<all_regs[i]["$s5"]<<endl;
    out<<"s6 = "<<all_regs[i]["$s6"]<<endl;
    out<<"s7 = "<<all_regs[i]["$s7"]<<endl;
    out<<"t0 = "<<all_regs[i]["$t0"]<<endl;
    out<<"t1 = "<<all_regs[i]["$t1"]<<endl;
    out<<"t2 = "<<all_regs[i]["$t2"]<<endl;
    out<<"t3 = "<<all_regs[i]["$t3"]<<endl;
    out<<"t4 = "<<all_regs[i]["$t4"]<<endl;
    out<<"t5 = "<<all_regs[i]["$t5"]<<endl;
    out<<"t6 = "<<all_regs[i]["$t6"]<<endl;
    out<<"t7 = "<<all_regs[i]["$t7"]<<endl;
    out<<"t8 = "<<all_regs[i]["$t8"]<<endl;
    out<<"t9 = "<<all_regs[i]["$t9"]<<endl;
    out<<"v0 = "<<all_regs[i]["$v0"]<<endl;
    out<<"v1 = "<<all_regs[i]["$v1"]<<endl;
    out<<"a0 = "<<all_regs[i]["$a0"]<<endl;
    out<<"a1 = "<<all_regs[i]["$a1"]<<endl;
    out<<"a2 = "<<all_regs[i]["$a2"]<<endl;
    out<<"a3 = "<<all_regs[i]["$a3"]<<endl;
    out<<"k0 = "<<all_regs[i]["$k1"]<<endl;
    out<<"gp = "<<all_regs[i]["$gp"]<<endl;
    out<<"sp = "<<all_regs[i]["$sp"]<<endl;
    out<<"fp = "<<all_regs[i]["$fp"]<<endl;
    out<<"ra = "<<all_regs[i]["$ra"]<<endl;
    out<<"at = "<<all_regs[i]["$at"]<<endl;
}

// Validator for the operands of instructions "add","sub","mul","addi","slt"
//Operator $reg1,reg2,reg3/Int
void validator_add(string s, int l, int m, string instruction, int file_number){
        stringstream s_stream(s);
        int count = 0;
        string substr;
        while(s_stream.good()) {
            getline(s_stream, substr, ',');
            substr = trim(substr);
            if (instruction=="addi" && count == 2){
                    if(!regex_match(substr,n)){
                        cout<<"File Number "<<file_number+1<<": Invalid second operator "<<substr<<"for operator "<<instruction<<endl;
                        exit(-1);
                    }else{
                        if (abs(stoi(substr))>pow(2,16)){
                            cout<<"File Number "<<file_number+1<<": Error! I-type instruction cannot exceed 2^16 in MIPS on line "<<l+1<<endl;
                            exit(-1); 
                        }
                    }
            }else if (count == 2){
                if(substr[0] =='$'){validator(registers,substr,l+1,file_number);}
                else{
                    if(!regex_match(substr,n)){
                        cout<<"File Number "<<file_number+1<<": Invalid second operator "<<substr<<"for operator "<<instruction<<endl;
                        exit(-1);
                    }else{
                        if (abs(stoi(substr))>pow(2,16)){
                            cout<<"File Number "<<file_number+1<<": Error! I-type instruction cannot exceed 2^16 in MIPS on line "<<l+1<<endl;
                            exit(-1); 
                        }
                    }
                }
            }
            else validator(registers,substr,l+1,file_number);
            all_params[file_number][m].push_back(substr);
            count +=1;
        }
        if (count != 3){
            cout<<"File Number "<<file_number+1<<": Invalid number of operands for "<< instruction <<" operator on line "<<l+1<<endl;
            exit(-1);
        }
}
// Validator for instructions "bne" and "beq"
// beq $reg1,$reg2/Int,label
void validator_beq(string s, int l,int m, string instruction, int file_number){
        stringstream s_stream(s);   
        int count = 0;
        string substr;
        while(s_stream.good()) {
            getline(s_stream, substr, ',');
            substr = trim(substr);
            if(count==0){validator(registers,substr,l+1,file_number);}
            if(count ==1){
                if(substr[0] =='$'){validator(registers,substr,l+1,file_number);}
                else{
                    if(!regex_match(substr,n)){
                        cout<<"File Number "<<file_number+1<<": Invalid second operator "<<substr<<"for operator "<<instruction<<endl;
                        exit(-1);
                    }else{
                        if (abs(stol(substr))>pow(2,16)){
                            cout<<"File Number "<<file_number+1<<": Error! I-type instruction cannot exceed 2^16 in MIPS on line "<<l+1<<endl;
                            exit(-1); 
                        }
                    }
                }
            }
            all_params[file_number][m].push_back(substr);
            count +=1;
        }
        if (count != 3){
            cout<<"File Number "<<file_number+1<<": Invalid operands for "<<instruction <<" operator on line "<<l+1<<endl;
            exit(-1);
        }
}
//Validator for Instructions "lw" and "sw"
//lw $reg1,off($reg2)
void validator_lw(string s, int l,int m, string instruction, int file_number){

        stringstream s_stream(s);
        int count = 0;
        string substr;
        while(s_stream.good()) {    
            getline(s_stream, substr, ',');
            substr = trim(substr);
            if(count==0){validator(registers,substr,l+1,file_number);all_params[file_number][m].push_back(substr);}
            if(count ==1){
                //off(reg)
                size_t q;
                q = substr.find_first_of('(');
                if(q==string::npos){cout<<"File Number "<<file_number+1<<": Invalid Operand for "<<instruction<<" on line "<<l+1<<endl;exit(-1);}
                string lh = substr.substr(0,q);
                // cout<<lh<<"\n";
                if(lh==""){
                    all_params[file_number][m].push_back("0");
                }
                else if (regex_match(lh,n)){
                    if (abs(stoll(lh))>pow(2,16)){
                        cout<<"File Number "<<file_number+1<<": Error! I-type instruction cannot exceed 2^16 in MIPS on line "<<l+1<<endl;
                        exit(-1); 
                    }
                    all_params[file_number][m].push_back(lh);
                }else{cout<<"File Number "<<file_number+1<<": Invalid Operand for "<<instruction<<" on line "<<l+1<<endl;exit(-1);}
                //(register) 
                string rh = substr.substr(q);
                if (rh[0] != '(' || rh.back() != ')'){cout<<"File Number "<<file_number+1<<": Invalid Operand for "<<instruction<<" on line "<<l+1<<endl;exit(-1);}
                rh = trim(rh.substr(1,rh.size()-2));
                validator(registers,rh,l,file_number);
                all_params[file_number][m].push_back(rh);
            }
            count +=1;
        }
        if (count != 2){
            cout<<"File Number "<<file_number+1<<": Invalid operands for "<<instruction <<" operator on line "<<l+1;
            exit(-1);
        }
}

void add(int i, int f){
    int y = (all_params[f][i][3][0]=='$') ? all_regs[f][all_params[f][i][3]] : stoi(all_params[f][i][3]);
    all_regs[f][all_params[f][i][1]]=all_regs[f][all_params[f][i][2]]+y;
    out<<"File Number "<<f+1<<" : "<<all_instructions[f][i]<<endl;
    out<<"File Number "<<f+1<<" : "<<all_params[f][i][1]<<" = "<<all_regs[f][all_params[f][i][1]]<<endl;
}
void sub(int i, int f){
    int y = (all_params[f][i][3][0]=='$') ? all_regs[f][all_params[f][i][3]] : stoi(all_params[f][i][3]);
    all_regs[f][all_params[f][i][1]]=all_regs[f][all_params[f][i][2]]-y;
    out<<"File Number "<<f+1<<" : "<<all_instructions[f][i]<<endl;
    out<<"File Number "<<f+1<<" : "<<all_params[f][i][1]<<" = "<<all_regs[f][all_params[f][i][1]]<<endl;
}
void mul(int i,int f){
    int y = (all_params[f][i][3][0]=='$') ? all_regs[f][all_params[f][i][3]] : stoi(all_params[f][i][3]);
    all_regs[f][all_params[f][i][1]]=all_regs[f][all_params[f][i][2]]*y;
    out<<"File Number "<<f+1<<" : "<<all_instructions[f][i]<<endl;
    out<<"File Number "<<f+1<<" : "<<all_params[f][i][1]<<" = "<<all_regs[f][all_params[f][i][1]]<<endl;
}
void addi(int i,int f){
    //cout<<all_params[f][i][1]<<" "<<all_params[f][i][2]<<" "<<all_params[f][i][3]<<endl;
    all_regs[f][all_params[f][i][1]]=all_regs[f][all_params[f][i][2]]+stoi(all_params[f][i][3]);
    out<<"File Number "<<f+1<<" : "<<all_instructions[f][i]<<endl;
    out<<"File Number "<<f+1<<" : "<<all_params[f][i][1]<<" = "<<all_regs[f][all_params[f][i][1]]<<endl;
}
void slt(int i,int f){
    int y = (all_params[f][i][3][0]=='$') ? all_regs[f][all_params[f][i][3]] : stoi(all_params[f][i][3]);
    if(all_regs[f][all_params[f][i][2]]< y ){
        all_regs[f][all_params[f][i][1]] = 1;
    }else {
        all_regs[f][all_params[f][i][1]] = 0;
    }
    out<<"File Number "<<f+1<<" : "<<all_instructions[f][i]<<endl;
    out<<"File Number "<<f+1<<" : "<<all_params[f][i][1]<<" = "<<all_regs[f][all_params[f][i][1]]<<endl;
}
void beq(int i,int f){
    if (all_labels[f].find(all_params[f][i][3]) == all_labels[f].end()){
        cout<<"File Number "<<f+1<<" : "<< "Invalid Label on line "<<i+1<<endl;
        exit(-1);
    }
    out<<"File Number "<<f+1<<" : "<<all_instructions[f][i]<<endl;
    int y = (all_params[f][i][2][0]=='$') ? all_regs[f][all_params[f][i][2]] : stoi(all_params[f][i][2]);
    if (all_regs[f][all_params[f][i][1]] == y){
        all_pc[f] = all_labels[f][all_params[f][i][3]];
    }else{
        all_pc[f]++;
    }
}
void bne(int i,int f){
    if (all_labels[f].find(all_params[f][i][3]) == all_labels[f].end()){
        cout << "Invalid Label on line "<<i+1<<endl;
        exit(-1);
    }
    out<<"File Number "<<f+1<<" : "<<all_instructions[f][i]<<endl;
    int y = (all_params[f][i][2][0]=='$') ? all_regs[f][all_params[f][i][2]] : stoi(all_params[f][i][2]);
    if (all_regs[f][all_params[f][i][1]] != y){
        all_pc[f] = all_labels[f][all_params[f][i][3]];
    }else{
        all_pc[f]++;
    }
}
void jump(int i, int f){
    if (all_labels[f].find(all_params[f][i][1]) == all_labels[f].end()){
        cout<<"File Number "<<f+1<<" "<< " : Invalid Label on line "<<i+1;
        exit(-1);
    }
    out<<"File Number "<<f+1<<" : "<<all_instructions[f][i]<<endl;
    all_pc[f] = all_labels[f][all_params[f][i][1]];
}

bool Is_Mrm_empty(){
    if (Dram_queue.size() == 0){return true;}
    else {
        for (auto j:Dram_queue){
            if (j.second.size()>0){
                return false;
            }
        }
        return true;
    }
}
bool checker(int i,int file_number){
    if(all_pc[file_number]>=num_array[file_number]){return false;}
    string ins = all_params[file_number][i][0];
    if((ins =="lw"||ins =="sw")){
        if (request_issued) {return false;}
        int offset = stoi(all_params[file_number][i][2]);
        int address = offset+all_regs[file_number][all_params[file_number][i][3]];
        int r = (address + CORE_MEMORY*file_number)/1024;
        if (Dram_queue.find(r) == Dram_queue.end()){

            if(Dram_queue.size()>=16){
                return false;
            }
        }else{
            if(Dram_queue[r].size()>=8){
                return false;
            }
        }
    }
    for (int j = 0; j<all_params[file_number][i].size();j++){
        if (all_blocking_registers[file_number][all_params[file_number][i][j]]>0){
            if (ins =="lw" && j == 1){continue;}
            curr_blocking_register[file_number] = all_params[file_number][i][j];
            return false;
        }
    }
    curr_blocking_register[file_number] = "";
    return true;
}


int main(int argc, char *argv[]) {
    int N = stoi(argv[1]);
    int M  = stoi(argv[2]);
    all_params.resize(N);
    all_instructions.resize(N);
    all_labels.resize(N);
    all_pc.resize(N);
    all_regs.resize(N);
    all_blocking_registers.resize(N);
    num_array.resize(N);
    individual_queue_size = TOTAL_QUEUE_SIZE/N;
    rows_for_a_core = (1024/N);
    CORE_MEMORY = 1024*rows_for_a_core;
    assign_computed=false;
    

    vector<string> input_files;
    fstream files[N];
    int j = 0;
    while (j<N){
        input_files.push_back(argv[j+3]);
        files[j].open(argv[j+3]);
        if(!files[j].is_open()){cout<<"File"<<j+1<<"not found"<<endl;exit(-1);}       
        j++;
    }
    ROW_ACCESS_DELAY = atoi(argv[j+3]);
    COL_ACCESS_DELAY = atoi(argv[j+4]);
    //reg["$zero"] = 0;
    for(int i = 0;i<256;i++){Row_buffer.push_back(0);}
    for(int i = 0;i<1024;i++){Dram_Memory.push_back(Row_buffer);}
    out.open("output.txt");
    for (int i = 0;i<N;i++){
        int num=0;
        int num2 = 0;
        map<int,int> line_number = {};
        string line;
        size_t x;
        string Instruction;
        while(getline(files[i],line)){
            line = trim(line);
            num2++;
            if(line == ""){
                continue;
            }
            all_instructions[i].push_back(line);
            x = line.find_first_of('$');
            Instruction = line;
            if(x==string::npos){
                // The line does not contain $ symbol hence it can either be a label or a jump instruction
                if (Instruction.substr(0,2) == "j "){
                    all_params[i][num].push_back("j");
                    string oprd = trim(Instruction.substr(2));
                    all_params[i][num].push_back(oprd);
                }else if(Instruction.back() == ':'){
                    string b = trim(Instruction.substr(0,Instruction.size()-1));
                    if (regex_match(b,l)){
                        if(all_labels[i].find(b) != all_labels[i].end()){
                            cout<<"Same Label can't be defined twice"<<endl;
                            cout<<"Label "<<b<<" is defined on lines "<<all_labels[i][b]+1<<" and "<<num2<<" of file"<<i+1<<endl;
                            exit(-1);
                        }
                        all_labels[i][b] = num;
                        all_params[i][num].push_back(b);
                    }else{
                        cout<< "Invaid Label format on line"<<num2<<" of file"<<i+1;
                        exit(-1);
                    }
                }else{
                    cout<< "Invalid Input "<<Instruction<<" on line "<<num2<<" of file"<<i+1;;
                    exit(-1);
                }
            }else{
                // The line contains $ symbol
                Instruction = trim(line.substr(0,x));           
                validator(operations,Instruction,num2,i);           
                all_params[i][num].push_back(Instruction);
                string operands = trim(line.substr(x));            
                if (Instruction == "add"||Instruction =="sub"||Instruction =="mul"||Instruction =="slt"||Instruction =="addi"){  //$t1, $t2, $t3                
                    validator_add(operands,num2-1,num,Instruction,i);
                }else if (Instruction=="beq"||Instruction =="bne"){
                    validator_beq(operands,num2-1,num,Instruction,i);
                }else if (Instruction == "lw"||Instruction =="sw"){
                    validator_lw(operands,num2-1,num,Instruction,i);
                }else{
                    continue;
                }
            }
            line_number[num] = num2;
            num++;
            // Throws an error if number of commands is more than Instruction storage limit.
            if (num>pow(2,17)){
                cout<<"File Number "<<j+1<<" : Error! Instruction memory limit exceeded"<<endl;
                exit(-1);
            }
            // instruction memory nums X 4 bytes
        }
        files[i].close();
        num_array[i] = num;
        all_lines_numbers.push_back(line_number);
    }

// -----------------------------------------------PARSING ENDS ---------------------------------------------------------
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
    //       cout<<j.first<<"->"<<j.second<<"   ";
    // }
    // cout<<endl;
    // Program Counter that iterates over the instruction set and points to the instruction being executed
    while (true){
        if (clock_cycle>=M){break;}
        //if (pc<num && label.find(params[pc][0])!=label.end()){pc++;continue;}
        clock_cycle++;
        out<<"\n"<<"Clock Cycle "<<clock_cycle<<" -> "<<endl;

        //MRM
        while(true){
            if (Is_dram_free && !Is_Mrm_empty()){
                if (!assign_computed){
                    int clock = 0;
                    if (executing_instruction.empty()){
                        for (auto i :Dram_queue){
                            if(i.second.size()>0){
                                executing_instruction = i.second[0];
                                clock = 1;
                                break;
                                }
                            }
                    }else{
                        int a = stoi(executing_instruction[1]);
                        int r = (a+CORE_MEMORY*stoi(executing_instruction[6]))/1024;
                        if (Dram_queue[r].size()>0){
                            executing_instruction = Dram_queue[r][0];
                            clock = 2;
                        }else{
                            Dram_queue.erase(r);
                            bool flag2 = false;
                            for (auto j : curr_blocking_register){
                                int core = j.first;
                                string curr_reg = j.second;
                                for (auto z : Dram_queue){
                                    vector<vector<string>> temp = z.second;
                                    if (temp.size() == 0){continue;}
                                    if (stoi(temp[0][6]) != core){continue;}
                                    int row3 = (stoi(temp[0][1])+CORE_MEMORY*stoi(temp[0][6]))/1024;
                                    clock ++;
                                    for (auto k : temp){
                                        if (k[2] == curr_reg){
                                            executing_instruction = Dram_queue[row3][0];
                                            flag2 = true;
                                            break;
                                        }
                                    }
                                    if (flag2){break;}
                                }
                                if (flag2){break;}
                            }
                            if (!flag2){
                                for (auto i :Dram_queue){
                                    if(i.second.size()>0){
                                        executing_instruction = i.second[0];
                                        clock = 3;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    assign_computed = true;
                    total_assign_time = clock;
                    assign_counter = 1;
                }else{
                    if (assign_counter!=total_assign_time){
                        assign_counter ++;
                        break;
                    }else{
                        //cout<<"reached here"<<endl;
                        // for(auto p : executing_instruction){
                        //     cout<<p<<" ";
                        // }
                        // cout<<endl;
                        //cout<<executing_instruction[1]<<" "<<executing_instruction[6]<<" "<<endl;
                        int row_of_executing_ins = (stoi(executing_instruction[1]) +CORE_MEMORY*stoi(executing_instruction[6]))/1024;
                        if (Dram_queue[row_of_executing_ins][0] != executing_instruction){cout<<"Error 10"<<endl;}
                        Dram_queue[row_of_executing_ins].erase(Dram_queue[row_of_executing_ins].begin());
                        if(Dram_queue[row_of_executing_ins].size() == 0){Dram_queue.erase(row_of_executing_ins);}
                        if (stoi(cache[7][0]) == row_of_executing_ins && cache[7].size()>1){
                            if (cache[7].size()>1 && cache[7][1] == "0"){
                                cache[7] = {"0"};
                                cache[8] = {"0"};
                            }else{
                                cache[7][1] = to_string(stoi(cache[7][1])-1);
                            }
                        }
                        if (stoi(cache[5][0]) == row_of_executing_ins && cache[5].size()>1){
                            if (cache[5].size()>1 && cache[5][1] == "0"){
                                cache[5] = {"0"};
                                cache[6] = {"0"};
                            }else{
                                cache[5][1] = to_string(stoi(cache[5][1])-1);
                            }
                        }
                        Is_dram_free = false;
                        assign_computed = false;
                        assign_counter = 0;
                    }
                }
                break;
            }else if(request_issued){
                //sw 2 1000, lw r1 1000 
                //sw 3 1000, sw 1 1000
                //lw r1 1000, lw r1 2000

                if (cache[0][0]=="0"){
                    int clock_count = 0;
                    int r = (stoi(issued_instruction[1])+CORE_MEMORY*stoi(issued_instruction[6]))/1024;
                    int a = stoi(issued_instruction[1]); 
                    int s = Dram_queue[r].size();
                    int c = stoi(issued_instruction[6]);
                    cache[3] =issued_instruction;

                    if (issued_instruction[0] == "sw"){
                        int j = Dram_queue[r].size()-1;

                        while(j>=0){
                            if (stoi(Dram_queue[r][j][1]) == a){
                                if (Dram_queue[r][j][0] == "sw"){
                                    //sw-sw redundancy 
                                    clock_count = min(j,s-j);
                                    cache[5] = {to_string(r),to_string(j)};
                                    cache[6] = Dram_queue[r][j];
                                    break;
                                }else{
                                    break;
                                }
                            }
                            j-= 1;
                        }
                    }else{
                        string redundant_register = issued_instruction[2];
                        int j = Dram_queue[r].size()-1; 
                        while(j>=0){
                            if (stoi(Dram_queue[r][j][1]) == a){
                                if (Dram_queue[r][j][0] == "sw"){
                                    // sw-lw forwarding - issue array
                                    clock_count = min(j,s-j)+1;
                                    cache[2] = {"1"};
                                    cache[4] = {Dram_queue[r][j][2]};
                                }else{
                                    break;
                                }
                            }
                            j-= 1;
                        }
                        for (auto i : Dram_queue){
                            int curr_row = i.first;
                            vector<vector<string>> row_array = i.second;
                            if (row_array.size() == 0){continue;}
                            if(stoi(row_array[0][6])!= c){continue;}
                            int j = row_array.size()-1;
                            int s2 = row_array.size()-1;
                            while(j>=0){
                                if (row_array[j][2] == redundant_register){
                                   //lw-lw redundancy 
                                    clock_count+=min(j,s2-j);
                                    cache[7] = {to_string(curr_row),to_string(j)};
                                    cache[8] = row_array[j];
                                }
                                j-=1;
                            }
                        }                        
                    }
                    if (clock_count == 0 ){clock_count = 1;}
                    cache[0][0] = {to_string(clock_count)};
                    if(cache[2][0]=="0" && cache[5][0]=="0" && cache[7][0]=="0"){
                        cache[0][0]="1";
                    }  
                    

                    cache[1] = {"1"};
                }else if(stoi(cache[1][0]) == stoi(cache[0][0])){

                    int r = (stoi(cache[3][1])+CORE_MEMORY*stoi(cache[3][6]))/1024;
                    if (cache[2][0] == "0"){
                        // if (Dram_queue[r].size() == 0){
                        //     Dram_queue[r] = {cache[3]};
                        // }else{
                        vector<string> temporary = cache[3];
                        Dram_queue[r].push_back(temporary);
                        // }
                    }else{
                        all_regs[stoi(cache[3][6])][cache[3][2]] = stoi(cache[4][0]);
                        out<<"CORE "<< cache[3][6] << ": "<< cache[3][2] <<" = "<< cache[4][0]<<endl;
                        all_blocking_registers[stoi(cache[3][6])][cache[3][2]] -=1;
                        mrm_writing = true;
                        mrm_writing_to_core = stoi(cache[3][6]);
                    }
                    // removing sw-sw or lw-lw redundancy if present
                    vector<string> s = {"0"};
                    if (cache[5] != s){
                        Dram_queue[stoi(cache[5][0])].erase(Dram_queue[stoi(cache[5][0])].begin()+stoi(cache[5][1]));
                    }

                    if (cache[7] != s){                       
                        Dram_queue[stoi(cache[7][0])].erase(Dram_queue[stoi(cache[7][0])].begin()+stoi(cache[7][1]));
                        all_blocking_registers[stoi(cache[8][6])][cache[8][2]] -=1;
                    }

                    cache = Initial_cache;
                    request_issued = false;
                }else{
                    cache[1] = {to_string((stoi(cache[1][0])+1))};
                }
                break;
            }else{
                break;
            }
        }

        //DRAM
        if(!Is_dram_free && !(executing_instruction.size()>0)){

            vector<string> Curr_executed = executing_instruction;
            int address = stoi(Curr_executed[1]);
            int row = (address+CORE_MEMORY*stoi(Curr_executed[6]))/1024;
            int  col = (address+CORE_MEMORY*stoi(Curr_executed[6]))%1024;
            if(address >= CORE_MEMORY|| col%4 !=0){
                cerr<<"Program tried to access invalid memory address "<<address<<endl;
                exit(-1);
            }
            while(true){
                if (((stoi(executing_instruction[6]) == mrm_writing_to_core) && mrm_writing) && stoi(executing_instruction[3])==stoi(executing_instruction[4])-1){
                    break;
                }
                if (executing_instruction[3]=="0"){
                    out<<"File Number "<<stoi(executing_instruction[6])+1<<" : Started "<<executing_instruction[0]<<" "<<executing_instruction[1]<<" "<<executing_instruction[2]<<" on Line "<<executing_instruction[5]<<endl;
                    int curr = 0;
                    if(executing_instruction[0]=="sw") row_buffer_updates++;
                    if(last_buffer_row!=row){
                        curr += ROW_ACCESS_DELAY;
                        row_buffer_updates++;
                        if (last_buffer_row!=-1){
                            if(dirty_bit){
                            curr += ROW_ACCESS_DELAY;
                                out<<"Row "<<last_buffer_row<<" will be copied back to DRAM and ";
                            }else{
                                out<<"Row "<<last_buffer_row<<" need NOT be copied back to DRAM and "; 
                            }
                        }
                        out<<"Row "<<row<<" will be activated\n";
                        dirty_bit = 0;
                    }
                    curr += COL_ACCESS_DELAY;
                    executing_instruction[4] = to_string(curr);
                    executing_instruction[3] = "1";
                    out<<"File Number "<<stoi(executing_instruction[6])+1<<" : Completed "<<"1"<<"/"<<executing_instruction[4]<<endl;
                    break;
                }
                out<<"File Number "<<stoi(executing_instruction[6])+1<<" : Completed "<<to_string(stoi(executing_instruction[3])+1)<<"/"<<executing_instruction[4]<<endl;
                if (stoi(executing_instruction[3])==ROW_ACCESS_DELAY-1){
                    if(executing_instruction[4]==to_string(2*ROW_ACCESS_DELAY+COL_ACCESS_DELAY)){
                        for (int j = 0;j<256;j++){
                            if (Dram_Memory[last_buffer_row][j] != Row_buffer[j]){
                                out<<"Memory at "<<last_buffer_row*1024+j*4<<" = "<<to_string(Row_buffer[j])<<endl;
                            }
                        }
                        Dram_Memory[last_buffer_row] = Row_buffer;
                    }
                    if(stoi(executing_instruction[4])==ROW_ACCESS_DELAY+COL_ACCESS_DELAY){
                        Row_buffer = Dram_Memory[row];
                        last_buffer_row = row;
                    }
                    executing_instruction[3] = to_string(stoi(Curr_executed[3])+1);
                    break;
                }
                if (stoi(executing_instruction[3])==2*ROW_ACCESS_DELAY-1 && executing_instruction[4]==to_string(2*ROW_ACCESS_DELAY+COL_ACCESS_DELAY)){
                    Row_buffer = Dram_Memory[row];
                    last_buffer_row = row;
                    executing_instruction[3] = to_string(stoi(Curr_executed[3])+1);
                    break;
                }

                if(stoi(executing_instruction[3])==stoi(executing_instruction[4])-1){
                    total_int_executed++;
                    if(executing_instruction[0]=="lw"){
                        all_regs[stoi(executing_instruction[6])][executing_instruction[2]] = Row_buffer[col/4];
                        flag  = true;
                        stopcore = stoi(executing_instruction[6]);
                        all_blocking_registers[stoi(executing_instruction[6])][executing_instruction[2]] -=1;
                        out<<"File Number "<<stoi(executing_instruction[6])+1<<" : "<<executing_instruction[2]<<" = "<<Row_buffer[col/4]<<endl;
                    }
                    if(executing_instruction[0] =="sw"){
                        Row_buffer[col/4] = stoi(executing_instruction[2]);
                        dirty_bit = 1;
                    }
                    out<<"File Number "<<stoi(executing_instruction[6])+1<<" : Finished Instruction "<<executing_instruction[0]<<" "<<executing_instruction[1]<<" "<<executing_instruction[2]<<" on Line "<<executing_instruction[5]<<endl;
                    Is_dram_free = true;
                    break;
                }
                executing_instruction[3] = to_string(stoi(executing_instruction[3])+1);
                break;
            }
        }           
        //Normal Commands 
        for(int i=0;i<N;i++){
            //cout<<i<<" "<<stopcore<<" "<<flag<<" "<<mrm_writing<<" "<<mrm_writing_to_core<<endl;
            if (i != stopcore && !flag && !((i == mrm_writing_to_core) && mrm_writing)){
                if (checker(all_pc[i],i)){
                    //cout<<all_instructions[i][all_pc[i]]<<endl;
                    total_int_executed++;
                    string Instruction = all_params[i][all_pc[i]][0];
                    if (Instruction == "lw"){
                        total_int_executed--;
                        string relevant_registor = all_params[i][all_pc[i]][1];
                        int offset = stoi(all_params[i][all_pc[i]][2]);
                        int address = offset+all_regs[i][all_params[i][all_pc[i]][3]];
                        issued_instruction = {"lw",to_string(address),relevant_registor,"0","0",to_string(all_lines_numbers[i][all_pc[i]]),to_string(i),"0","0"}; 
                        request_issued = true;
                        // ins,address,register/value,counter,total time,line number, file number
                        all_blocking_registers[i][relevant_registor] ++;
                        out<<"File Number "<<i+1<< " : DRAM Request(Read) Issued for "<<"lw "<<address<<" "<<relevant_registor<<" on Line "<<all_lines_numbers[i][all_pc[i]]<<endl;        
                        all_pc[i]++;
                        continue;
                    }else if(Instruction == "sw"){
                        total_int_executed--;
                        string relevant_registor = all_params[i][all_pc[i]][1];
                        int offset = stoi(all_params[i][all_pc[i]][2]);
                        int address = offset+all_regs[i][all_params[i][all_pc[i]][3]];
                        issued_instruction = {"sw",to_string(address),to_string(all_regs[i][relevant_registor]),"0","0",to_string(all_lines_numbers[i][all_pc[i]]),to_string(i)};
                        request_issued = true;
                        out<<"File Number "<<i+1<< " : DRAM Request(Write) Issued for "<<"sw "<<address<<" "<<all_regs[i][relevant_registor]<<" on Line "<<all_lines_numbers[i][all_pc[i]]<<endl;         
                        all_pc[i]++;
                        continue;
                    }else if (Instruction == "add"){ 
                        add(all_pc[i],i);
                        all_pc[i]++;
                        continue;
                    }else if (Instruction == "sub"){
                        sub(all_pc[i],i);
                        all_pc[i]++;
                        continue;
                    }else if (Instruction == "mul"){
                        mul(all_pc[i],i);
                        all_pc[i]++;
                        continue;
                    }else if (Instruction == "addi"){
                        addi(all_pc[i],i);
                        all_pc[i]++;
                        continue;
                    }else if (Instruction == "bne"){ 
                        bne(all_pc[i],i);
                        continue;
                    }else if (Instruction == "beq"){ 
                        beq(all_pc[i],i);
                        continue;
                    }else if (Instruction == "slt"){
                        slt(all_pc[i],i);
                        all_pc[i]++;
                        continue;
                    }else if(Instruction == "j"){
                        jump(all_pc[i],i);
                        continue;
                    }else{
                        total_int_executed--;
                        all_pc[i]++;
                        continue;
                    }
                };
            }else{
                stopcore = -1;
                flag = false;
                mrm_writing = false;
                mrm_writing_to_core = -1;
            }
            if(all_regs[i]["$zero"] != 0){
                cout<<"The value in Zero Registor is not mutable."<<endl;
                exit(-1);
            }
        } 
        // bool completed_everything = false;
        // if (Is_Mrm_empty()){
        //     if (Is_dram_free){
        //         completed_everything = true;
        //         for (int i = 0;i<N;i++){
        //             if (all_pc[i]<num_array[i]){
        //                 completed_everything = false;
        //                 break;
        //             }

        //         }
        //     }
        // }
        // if (completed_everything){
        //     cout<<"Completed in "<<clock_cycle<<endl;
        //     break; 
        // }
        out<<"---------------------------------\n";
        out<<"MRM Queue :-"<<endl;
        for (auto const& j : Dram_queue){
            int c = j.first;
            vector<vector<string>> v = j.second;
            out<<c<<"\n";
            for(auto j: v){
                for(auto k:j){
                    out<<k<<" ";
                }
                out<<"\n";
            }
            out<<"\n";
        }
        out<<"MRM to DRAM"<<endl;
        out<<assign_computed<<" "<<total_assign_time<<" "<<assign_counter<<endl;
        out<<"CPU to MRM"<<endl;
        out<<cache[0][0]<<" "<<cache[1][0]<<" "<<endl; 
        out<<"Issued Instruction"<<endl;
        for(auto j:issued_instruction){
            out<<j<<" ";
        }
        out<<endl;
        out<<"Executing Instruction"<<endl;
        for(auto j:executing_instruction){
            out<<j<<" ";
        }
        out<<endl;       
        out <<"All Blocking Registers"<<endl;
        for (int i = 0;i<N;i++){
            for (auto j : all_blocking_registers[i]){
                if(j.second!= 0){out<<j.first<<"->"<<j.second<<", ";}
            }
            out<<endl;
        }
        out <<"Cache"<<endl;
        for (int i = 0;i<9;i++){
            for (auto j : cache[i]){
                out<< j <<" ";
            }
            out<<endl;
        }
        out<<"---------------------------------\n";

    }
    cout<<"Total Number of instructions executed in "<<M<<" Clock Cycles : "<<total_int_executed<<endl;
    cout<<"Total Number of Row Buffer Updates = "<<row_buffer_updates<<endl; 

    out<<"\n"<<"RELEVANT STATISTICS :->"<<endl;
    out<<"Total Number of instructions executed in "<<M<<" Clock Cycles : "<<total_int_executed<<endl;
    out<<"Total Number of Row Buffer Updates = "<<row_buffer_updates<<endl; 




    // out<<"\n"<<"DRAM memory structure :"<<endl;
    // for(int j = 0;j<512;j++){
    //     for(int i =0;i<256;i++){
    //         if (Dram_Memory[j][i] != 0){
    //             out<<"Memory at row "<<j<<" column "<<i<<" address "<<j*1024+i*4<<" = "<<Dram_Memory[j][i]<<endl;
    //         }
    //     }
    // }
    //out<<"\n"<<"Integer Register Values :"<<endl;
    for(int i=0;i<N;i++){
        out<<"\n"<<"Integer Register Values for File "<<i+1<<" -> "<<endl;
        print(i);
        out<<endl;
    }
	return 0;
}
//------------------------------------Execution Ends---------------------------------------------------------------------s