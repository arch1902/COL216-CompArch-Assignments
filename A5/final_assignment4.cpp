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

ofstream out;
map<string,int> reg;
vector<map<string,int>> all_regs;
map<int,vector<string>> params;
vector<map<int,vector<string>>> all_params;
map<string,int> label;
vector<map<string,int>> all_labels;
map<int,int> data_memory;
map<string,int> statistics;
vector<string> operations = {"add","sub","mul","beq","bne","slt","lw","sw","addi"};
vector<string> registers = {"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$k0","$k1","$ra","$t0","$t1",
"$t2","$t3","$t4","$t5","$t6","$t7","$t8","$t9","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$gp",
"$fp","$sp"};
vector<string> instructions; //Vector containing all instructions 
vector<vector<string>> all_instructions;
regex n("[-]?[0-9]+");  //for checking if a string is convertible to an Integer
regex l("([A-Z|a-z])[A-Z|a-z|0-9|_]*"); // for Label
int INSTRUCTION_MEMORY = pow(2,17); // Memory is word Addressable hence it has 2^19 Bytes
int DATA_MEMORY = pow(2,19)-4;
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
string curr_cpu_blocking_register = "";
vector<vector<int>> Dram_Memory;
vector<map<int,int>> all_lines_numbers;
vector<int> Row_buffer;
vector<vector<string>> Dram_queue;
map<string,int>  Blocking_registers;
vector<map<string,int>> all_blocking_registers;
vector<int> num_array;

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
void print(){
    out<<"zero = "<<reg["$zero"]<<endl;
    out<<"s0 = "<<reg["$s0"]<<endl;
    out<<"s1 = "<<reg["$s1"]<<endl;
    out<<"s2 = "<<reg["$s2"]<<endl;
    out<<"s3 = "<<reg["$s3"]<<endl;
    out<<"s4 = "<<reg["$s4"]<<endl;
    out<<"s5 = "<<reg["$s5"]<<endl;
    out<<"s6 = "<<reg["$s6"]<<endl;
    out<<"s7 = "<<reg["$s7"]<<endl;
    out<<"t0 = "<<reg["$t0"]<<endl;
    out<<"t1 = "<<reg["$t1"]<<endl;
    out<<"t2 = "<<reg["$t2"]<<endl;
    out<<"t3 = "<<reg["$t3"]<<endl;
    out<<"t4 = "<<reg["$t4"]<<endl;
    out<<"t5 = "<<reg["$t5"]<<endl;
    out<<"t6 = "<<reg["$t6"]<<endl;
    out<<"t7 = "<<reg["$t7"]<<endl;
    out<<"t8 = "<<reg["$t8"]<<endl;
    out<<"t9 = "<<reg["$t9"]<<endl;
    out<<"v0 = "<<reg["$v0"]<<endl;
    out<<"v1 = "<<reg["$v1"]<<endl;
    out<<"a0 = "<<reg["$a0"]<<endl;
    out<<"a1 = "<<reg["$a1"]<<endl;
    out<<"a2 = "<<reg["$a2"]<<endl;
    out<<"a3 = "<<reg["$a3"]<<endl;
    out<<"k0 = "<<reg["$k1"]<<endl;
    out<<"gp = "<<reg["$gp"]<<endl;
    out<<"sp = "<<reg["$sp"]<<endl;
    out<<"fp = "<<reg["$fp"]<<endl;
    out<<"ra = "<<reg["$ra"]<<endl;
    out<<"at = "<<reg["$at"]<<endl;
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
    out<<"File Number "<<f+1<<" "<<all_instructions[f][i]<<endl;
    out<<"File Number "<<f+1<<" "<<all_params[f][i][1]<<" = "<<all_regs[f][all_params[f][i][1]]<<endl;
}
void sub(int i, int f){
    int y = (all_params[f][i][3][0]=='$') ? all_regs[f][all_params[f][i][3]] : stoi(all_params[f][i][3]);
    all_regs[f][all_params[f][i][1]]=all_regs[f][all_params[f][i][2]]-y;
    out<<"File Number "<<f+1<<" "<<all_instructions[f][i]<<endl;
    out<<"File Number "<<f+1<<" "<<all_params[f][i][1]<<" = "<<all_regs[f][all_params[f][i][1]]<<endl;
}
void mul(int i,int f){
    int y = (all_params[f][i][3][0]=='$') ? all_regs[f][all_params[f][i][3]] : stoi(all_params[f][i][3]);
    all_regs[f][all_params[f][i][1]]=all_regs[f][all_params[f][i][2]]*y;
    out<<"File Number "<<f+1<<" "<<all_instructions[f][i]<<endl;
    out<<"File Number "<<f+1<<" "<<all_params[f][i][1]<<" = "<<all_regs[f][all_params[f][i][1]]<<endl;
}
void addi(int i,int f){
    //cout<<all_params[f][i][1]<<" "<<all_params[f][i][2]<<" "<<all_params[f][i][3]<<endl;
    all_regs[f][all_params[f][i][1]]=all_regs[f][all_params[f][i][2]]+stoi(all_params[f][i][3]);
    out<<"File Number "<<f+1<<" "<<all_instructions[f][i]<<endl;
    out<<"File Number "<<f+1<<" "<<all_params[f][i][1]<<" = "<<all_regs[f][all_params[f][i][1]]<<endl;
}
void slt(int i,int f){
    int y = (all_params[f][i][3][0]=='$') ? all_regs[f][all_params[f][i][3]] : stoi(all_params[f][i][3]);
    if(all_regs[f][all_params[f][i][2]]< y ){
        all_regs[f][all_params[f][i][1]] = 1;
    }else {
        all_regs[f][all_params[f][i][1]] = 0;
    }
    out<<"File Number "<<f+1<<" "<<all_instructions[f][i]<<endl;
    out<<"File Number "<<f+1<<" "<<all_params[f][i][1]<<" = "<<all_regs[f][all_params[f][i][1]]<<endl;
}
void beq(int i,int f){
    if (all_labels[f].find(all_params[f][i][3]) == all_labels[f].end()){
        cout<<"File Number "<<f+1<<" "<< "Invalid Label on line "<<i+1<<endl;
        exit(-1);
    }
    out<<"File Number "<<f+1<<" "<<all_instructions[f][i]<<endl;
    int y = (all_params[f][i][2][0]=='$') ? all_regs[f][all_params[f][i][2]] : stoi(all_params[f][i][2]);
    if (all_regs[f][all_params[f][i][1]] == y){
        all_pc[f] = all_labels[f][all_params[f][i][3]];
    }else{
        all_pc[f]++;
    }
}
void bne(int i,int f){
    if (label.find(params[i][3]) == label.end()){
        cout << "Invalid Label on line "<<i+1<<endl;
        exit(-1);
    }
    out<<"File Number "<<f+1<<" "<<all_instructions[f][i]<<endl;
    int y = (all_params[f][i][2][0]=='$') ? all_regs[f][all_params[f][i][2]] : stoi(all_params[f][i][2]);
    if (reg[params[i][1]] != y){
        all_pc[f] = all_labels[f][all_params[f][i][3]];
    }else{
        all_pc[f]++;
    }
}
void jump(int i, int f){
    if (all_labels[f].find(all_params[f][i][1]) == all_labels[f].end()){
        cout<<"File Number "<<f+1<<" "<< "Invalid Label on line "<<i+1;
        exit(-1);
    }
    out<<"File Number "<<f+1<<" "<<all_instructions[f][i]<<endl;
    all_pc[f] = all_labels[f][all_params[f][i][1]];
}

void sort_queue(){
    if (Dram_queue.empty()){return;}
    vector<string> first = Dram_queue[0];
    int row = stoi(first[1])/1024;
    int blocking_row = -1;
    int blocking_column = -1;
    bool is_first = false;
    if (first[2]==curr_cpu_blocking_register){
        is_first = true;
        blocking_row = row;
        blocking_column = stoi(first[1])%1024;
    }
    int off = 1;
    for(int i = 1;i<Dram_queue.size();i++){
        vector<string> curr = Dram_queue[i];
        int r = stoi(curr[1])/1024;
        if(r==row){
            Dram_queue.erase(Dram_queue.begin()+i);
            Dram_queue.insert(off+Dram_queue.begin(),curr);
            off++;
        }
        if (curr[2]==curr_cpu_blocking_register){
            blocking_row = r;
            blocking_column = stoi(curr[1])%1024;
        }       
    }
    if(blocking_row!= -1 && !is_first){
        if(blocking_row == row){
            int off2 = 1;
            for(int i = 1;i<Dram_queue.size();i++){
                vector<string> curr = Dram_queue[i];
                int r = stoi(curr[1])/1024;
                int c = stoi(curr[1])%1024;
                if(c == blocking_column&&r == blocking_row){
                    Dram_queue.erase(Dram_queue.begin()+i);
                    Dram_queue.insert(off2+Dram_queue.begin(),curr);
                    off2++;
                }
            }
        }else{
         for(int i = 1;i<Dram_queue.size();i++){
                vector<string> curr = Dram_queue[i];
                int r = stoi(curr[1])/1024;   
                if(r==blocking_row){
                    Dram_queue.erase(Dram_queue.begin()+i);
                    Dram_queue.insert(off+Dram_queue.begin(),curr);
                    off++;
                    break;
                }                
            }
        }
    }
}    

bool checker(int i,int file_number){
    if(all_pc[file_number]>=num_array[file_number]){return false;}
    for (auto j :all_params[file_number][i]){
        if (all_blocking_registers[file_number][j]>0){
            return false;
        }
    }
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
    for(int i = 0;i<512;i++){Dram_Memory.push_back(Row_buffer);}
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
            if (num>INSTRUCTION_MEMORY){
                cout<<"File Number "<<j+1<<"Error! Instruction memory limit exceeded"<<endl;
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
        out<<"\n"<<"Clock Cycle "<<clock_cycle<<":"<<endl;
        //out<<" Current CPU Blocking "<<curr_cpu_blocking_register<<endl;
        // for (auto const & string_vec : Dram_queue) {
        //     out<<"(";
        //     for (auto const & str : string_vec) {
        //         out << str<<", ";
        //     }
        //     out<<")";
        // }
        // out<<endl;
        // out<<endl;
        // for(auto x:Blocking_registers){
        //     out<<"( ";
        //     out <<x.first<<","<<x.second;
        //     out<<" ) ";
        // }
        
        // bool flag = false;
        // string removed_register;
        // //DRAM 
        // if (!Dram_queue.empty()){
        //     vector<string> Curr_executed = Dram_queue[0];
        //     int address = stoi(Curr_executed[1]);
        //     int row = address/1024;
        //     int  col = address%1024;
        //     if(address>INSTRUCTION_MEMORY || col%4 !=0){
        //         cout<<"Program tried to access invalid memory address "<<address<<endl;
        //         exit(-1);
        //     }
        //     // for(auto k: Dram_queue[0]){
        //     //      out<<k<<" ";
        //     // }
        //     // out<<endl;
        //     while(true){
        //         //out<<Dram_queue[0][3]<<endl;0
        //         if (Dram_queue[0][3]=="0"){
        //             out<<"Started "<<Dram_queue[0][0]<<" "<<Dram_queue[0][1]<<" "<<Dram_queue[0][2]<<" on Line "<<Dram_queue[0][5]<<endl;
        //             int curr = 0;
        //             if(Dram_queue[0][0]=="sw") row_buffer_updates++;
        //             if(last_buffer_row!=row){
        //                 curr += ROW_ACCESS_DELAY;
        //                 row_buffer_updates++;
        //                 if (last_buffer_row!=-1){
        //                     if(dirty_bit){
        //                     curr += ROW_ACCESS_DELAY;
        //                     out<<"Row "<<last_buffer_row<<" will be copied back to DRAM and ";
        //                     }else{
        //                        out<<"Row "<<last_buffer_row<<" need NOT be copied back to DRAM and "; 
        //                     }
        //                 }
        //                 out<<"Row "<<row<<" will be activated\n";
        //                 dirty_bit = 0;
        //             }
        //             curr += COL_ACCESS_DELAY;
        //             Dram_queue[0][4] = to_string(curr);
        //             Dram_queue[0][3] = "1";
        //             out<<"Completed "<<"1"<<"/"<<Dram_queue[0][4]<<endl;
        //             break;
        //         }
        //         out<<"Completed "<<to_string(stoi(Dram_queue[0][3])+1)<<"/"<<Dram_queue[0][4]<<endl;
        //         if (stoi(Dram_queue[0][3])==ROW_ACCESS_DELAY-1){
        //             if(Dram_queue[0][4]==to_string(2*ROW_ACCESS_DELAY+COL_ACCESS_DELAY)){
        //                 for (int j = 0;j<256;j++){
        //                     if (Dram_Memory[last_buffer_row][j] != Row_buffer[j]){
        //                         out<<"Memory at "<<last_buffer_row*1024+j*4<<" = "<<to_string(Row_buffer[j])<<endl;
        //                     }
        //                 }
        //                 Dram_Memory[last_buffer_row] = Row_buffer;
        //             }
        //             if(stoi(Dram_queue[0][4])==ROW_ACCESS_DELAY+COL_ACCESS_DELAY){
        //                 Row_buffer = Dram_Memory[row];
        //                 last_buffer_row = row;
        //             }
        //             Dram_queue[0][3] = to_string(stoi(Curr_executed[3])+1);
        //             break;
        //         }
        //         if (stoi(Dram_queue[0][3])==2*ROW_ACCESS_DELAY-1 && Dram_queue[0][4]==to_string(2*ROW_ACCESS_DELAY+COL_ACCESS_DELAY)){
        //             Row_buffer = Dram_Memory[row];
        //             last_buffer_row = row;
        //             Dram_queue[0][3] = to_string(stoi(Curr_executed[3])+1);
        //             break;
        //         }

        //         if(stoi(Dram_queue[0][3])==stoi(Dram_queue[0][4])-1){
        //             if(Dram_queue[0][0]=="lw"){
        //                 reg[Dram_queue[0][2]] = Row_buffer[col/4];
        //                 flag  = true;
        //                 removed_register = Dram_queue[0][2];
        //                 out<<removed_register<<" = "<<Row_buffer[col/4]<<endl;
        //                 //Blocking_registers[Dram_queue[0][2]] -=1;
        //             }
        //             if(Dram_queue[0][0] =="sw"){
        //                 Row_buffer[col/4] = stoi(Dram_queue[0][2]);
        //                 dirty_bit = 1;
        //             }
        //             out<<"Finished Instruction "<<Dram_queue[0][0]<<" "<<Dram_queue[0][1]<<" "<<Dram_queue[0][2]<<" on Line "<<Dram_queue[0][5]<<endl;
        //             Dram_queue.erase(Dram_queue.begin());
        //             sort_queue();
        //             //Dram_queue[0][3] = to_string(stoi(Dram_queue[0][3])+1);
        //             break;
        //         }
        //         Dram_queue[0][3] = to_string(stoi(Dram_queue[0][3])+1);
        //         break;
        //     }
        // }

        //Normal Commands 
        for(int i=0;i<N;i++){
            if (checker(all_pc[i],i)){
                //out<<"Here"<<endl;
                string Instruction = all_params[i][all_pc[i]][0];
                // if (flag){
                //     all_blocking_registers[i][removed_register] -=1;
                //     flag = false;
                // }
                //cout<<Instruction<<endl;
                if (Instruction == "lw"){
                    string relevant_registor = all_params[i][all_pc[i]][1];
                    int offset = stoi(all_params[i][all_pc[i]][2]);
                    int address = offset+all_regs[i][all_params[i][all_pc[i]][3]];
                    Dram_queue.push_back({"lw",to_string(address),relevant_registor,"0","0",to_string(all_lines_numbers[i][all_pc[i]])}); 
                    sort_queue();  
                    all_blocking_registers[i][relevant_registor] ++;
                    out<< "DRAM Request(Read) Issued for "<<"lw "<<address<<" "<<relevant_registor<<" on Line "<<all_lines_numbers[i][all_pc[i]]<<endl;        
                    all_pc[i]++;
                    continue;
                }else if(Instruction == "sw"){
                    string relevant_registor = all_params[i][all_pc[i]][1];
                    int offset = stoi(all_params[i][all_pc[i]][2]);
                    int address = offset+all_regs[i][all_params[i][all_pc[i]][3]];
                    Dram_queue.push_back({"sw",to_string(address),to_string(all_regs[i][relevant_registor]),"0","0",to_string(all_lines_numbers[i][all_pc[i]])});  
                    sort_queue();
                    out<< "DRAM Request(Write) Issued for "<<"sw "<<address<<" "<<all_regs[i][relevant_registor]<<" on Line "<<all_lines_numbers[i][all_pc[i]]<<endl;         
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
                    all_pc[i]++;
                    continue;
                }
            }
            // if (flag){
            //     Blocking_registers[removed_register] -=1;
            //     flag = false;
            // }
        }
        // if(all_regs[i]["$zero"] != 0){
        //     cout<<"The value in Zero Registor is not mutable."<<endl;
        //     exit(-1);
        // }
    }
    cout<<"Total Number of cycles taken = "<<clock_cycle<<endl;
    cout<<"Total Number of Row Buffer Updates = "<<row_buffer_updates<<endl; 

    out<<"\n"<<"RELEVANT STATISTICS :->"<<endl;
    out<<"Total Number of cycles taken = "<<clock_cycle<<endl;
    out<<"Total Number of Row Buffer Updates = "<<row_buffer_updates<<endl;  


    out<<"\n"<<"DRAM memory structure :"<<endl;
    for(int j = 0;j<512;j++){
        for(int i =0;i<256;i++){
            if (Dram_Memory[j][i] != 0){
                out<<"Memory at row "<<j<<" column "<<i<<" address "<<j*1024+i*4<<" = "<<Dram_Memory[j][i]<<endl;
            }
        }
    }
    out<<"\n"<<"Integer Register Values :"<<endl;
    print();
	return 0;
}
//------------------------------------Execution Ends---------------------------------------------------------------------