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
vector<string> registers = {"$zero","$r0","$r1","$r2","$r3","$r4","$r5","$r6","$r7","$r8","$r9","$t0","$t1",
"$t2","$t3","$t4","$t5","$t6","$t7","$t8","$t9","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8",
"$s9","$sp"};
vector<string> instructions; //Vector containing all instructions 
regex n("[-]?[0-9]+");  //for checking if a string is convertible to an Integer
regex l("([A-Z|a-z])[A-Z|a-z|0-9|_]*"); // for Label
int INSTRUCTION_MEMORY = pow(2,17); // Memory is word Addressable hence it has 2^19 Bytes
int DATA_MEMORY = pow(2,19)-4;
int ROW_ACCESS_DELAY  = 10;
int COL_ACCESS_DELAY = 2;
int clock_cycle = 0;
int last_buffer_row = -1;
int pc = 0;
int num=0;
int row_activate = 0;
int row_writeback = 0;
vector<vector<int>> Dram_Memory;
vector<int> Row_buffer;
vector<vector<string>> Dram_queue;
map<string,int>  Blocking_registers;


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
void validator(vector<string> V, string s,int l){
    for( auto j : V){
        if (j == s){
            return;
        }
    }
    cout<<"Invalid input "<<s<<" found on line "<<l<<endl;
    exit(-1);
}

// Prints the registers in decimal format
void print(){
    out<<reg["$r1"];
    out<<" "<<reg["$r2"];
    out<<" "<<reg["$r3"];
    out<<" "<<reg["$r4"];
    out<<" "<<reg["$r5"];
    out<<" "<<reg["$r6"];
    out<<" "<<reg["$r7"];
    out<<" "<<reg["$r8"];
    out<<" "<<reg["$r9"];
    out<<" "<<reg["$r10"];
    out<<" "<<reg["$r11"];
    out<<" "<<reg["$r12"];
    out<<" "<<reg["$r13"];
    out<<" "<<reg["$r14"];
    out<<" "<<reg["$r15"];
    out<<" "<<reg["$r16"];
    out<<" "<<reg["$r17"];
    out<<" "<<reg["$r18"];
    out<<" "<<reg["$r19"];
    out<<" "<<reg["$r20"];
    out<<" "<<reg["$r21"];
    out<<" "<<reg["$r22"];
    out<<" "<<reg["$r23"];
    out<<" "<<reg["$r24"];
    out<<" "<<reg["$r25"];
    out<<" "<<reg["$r26"];
    out<<" "<<reg["$r27"];
    out<<" "<<reg["$r28"];
    out<<" "<<reg["$r29"];
    out<<" "<<reg["$r30"];
    out<<" "<<reg["$r31"]<<endl;
}


// Validator for the operands of instructions "add","sub","mul","addi","slt"
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
                    }else{
                        if (abs(stoi(substr))>pow(2,16)){
                            cout<<"Error! I-type instruction cannot exceed 2^16 in MIPS on line "<<l+1<<endl;
                            exit(-1); 
                        }
                    }
            }else if (count == 2){
                if(substr[0] =='$'){validator(registers,substr,l+1);}
                else{
                    if(!regex_match(substr,n)){
                        cout<<"Invalid second operator "<<substr<<"for operator "<<instruction<<endl;
                        exit(-1);
                    }else{
                        if (abs(stoi(substr))>pow(2,16)){
                            cout<<"Error! I-type instruction cannot exceed 2^16 in MIPS on line "<<l+1<<endl;
                            exit(-1); 
                        }
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
// Validator for instructions "bne" and "beq"
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
                    }else{
                        if (abs(stoi(substr))>pow(2,16)){
                            cout<<"Error! I-type instruction cannot exceed 2^16 in MIPS on line "<<l+1<<endl;
                            exit(-1); 
                        }
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
//Validator for Instructions "lw" and "sw"
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

bool checker(int i){
    if(pc>=num){return false;}
    for (auto j :params[i]){
        if (Blocking_registers[j]>0){
            return false;
        }
    }
    return true;
}
void add(int i){
    int y = (params[i][3][0]=='$') ? reg[params[i][3]] : stoi(params[i][3]);
    reg[params[i][1]]=reg[params[i][2]]+y;
    out <<instructions[i]<<endl;
    out<<params[i][1]<<" = "<<reg[params[i][1]]<<endl;
}
void sub(int i){
    int y = (params[i][3][0]=='$') ? reg[params[i][3]] : stoi(params[i][3]);
    reg[params[i][1]]=reg[params[i][2]]-y;
    out <<instructions[i]<<endl;
    out<<params[i][1]<<" = "<<reg[params[i][1]]<<endl;
}
void mul(int i){
    int y = (params[i][3][0]=='$') ? reg[params[i][3]] : stoi(params[i][3]);
    reg[params[i][1]]=reg[params[i][2]]*y;
    out <<instructions[i]<<endl;
    out<<params[i][1]<<" = "<<reg[params[i][1]]<<endl;
}
void addi(int i){
    reg[params[i][1]]=reg[params[i][2]]+stoi(params[i][3]);
    out <<instructions[i]<<endl;
    out<<params[i][1]<<" = "<<reg[params[i][1]]<<endl;
}
void slt(int i){
    int y = (params[i][3][0]=='$') ? reg[params[i][3]] : stoi(params[i][3]);
    if(reg[params[i][2]]< y ){
        reg[params[i][1]] = 1;
    }else {
        reg[params[i][1]] = 0;
    }
    out <<instructions[i]<<endl;
    out<<params[i][1]<<" = "<<reg[params[i][1]]<<endl;
}
void beq(int i){
    if (label.find(params[i][3]) == label.end()){
        cout << "Invalid Label on line "<<i+1<<endl;
        exit(-1);
    }
    out <<instructions[i]<<endl;
    int y = (params[i][2][0]=='$') ? reg[params[i][2]] : stoi(params[i][2]);
    if (reg[params[i][1]] == y){
        pc = label[params[i][3]];
    }else{
        pc++;
    }
}
void bne(int i){
    if (label.find(params[i][3]) == label.end()){
        cout << "Invalid Label on line "<<i+1<<endl;
        exit(-1);
    }
    out <<instructions[i]<<endl;
    int y = (params[i][2][0]=='$') ? reg[params[i][2]] : stoi(params[i][2]);
    if (reg[params[i][1]] != y){
        pc = label[params[i][3]];
    }else{
        pc++;
    }
}
void j(int i){
    if (label.find(params[i][1]) == label.end()){
        cout << "Invalid Label on line "<<i+1;
        exit(-1);
    }
    out <<instructions[i]<<endl;
    pc = label[params[i][1]];
}
int main(int argc, char *argv[]) {
    reg["$zero"] = 0;
    for(int i = 0;i<256;i++){Row_buffer.push_back(0);}
    for(int i = 0;i<512;i++){Dram_Memory.push_back(Row_buffer);}
    file1.open(argv[1]);
    if(!file1.is_open()){cout<<"File not found"<<endl;exit(-1);}
    string input_file = argv[1];
    out.open(input_file+"_part2_output.txt");
    string line;
    size_t x;
 
    string Instruction;
    while(getline(file1,line)){
        line = trim(line);
        if(line == ""){
            continue;
        }
        instructions.push_back(line);
        x = line.find_first_of('$');
        Instruction = line;
        if(x==string::npos){
            // The line does not contain $ symbol hence it can either be a label or a jump instruction
            if (Instruction.substr(0,2) == "j "){
                params[num].push_back("j");
                string oprd = trim(Instruction.substr(2));
                params[num].push_back(oprd);
            }else if(Instruction.back() == ':'){
                string b = trim(Instruction.substr(0,Instruction.size()-1));
                if (regex_match(b,l)){
                    if(label.find(b) != label.end()){
                        cout<<"Same Label can't be defined twice"<<endl;
                        cout<<"Label "<<b<<" is defined on lines "<<label[b]+1<<" and "<<num+1<<endl;
                        exit(-1);
                    }
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
            // The line contains $ symbol
            Instruction = trim(line.substr(0,x));           
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
        num++;
        // Throws an error if number of commands is more than Instruction storage limit.
        if (num>INSTRUCTION_MEMORY){
            cout<<"Error! Instruction memory limit exceeded"<<endl;
            exit(-1);
        }
        // instruction memory nums X 4 bytes
    }
    file1.close();
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
    //      cout<<j.first<<"->"<<j.second<<"   ";
    // }
    // cout<<endl;
    // Program Counter that iterates over the instruction set and points to the instruction being executed
    while (true){
        if (pc>=num && Dram_queue.empty()){break;}
        clock_cycle++;
        out<<"Clock Cycle"<<clock_cycle<<":"<<endl;

        //DRAM 
        if (!Dram_queue.empty()){
            vector<string> Curr_executed = Dram_queue[0];
            int address = stoi(Curr_executed[1]);
            int row = address/512;
            int  col = address%512;
            if(address>INSTRUCTION_MEMORY || col%4 !=0){
                cout<<"Program tried to access invalid memory address "<<address<<endl;
                exit(-1);
            }
            while(true){
                Curr_executed[3] = to_string(stoi(Curr_executed[3])+1);
                if (Curr_executed[3]=="1"){
                    int curr = 0;
                    if(last_buffer_row!=row){
                        curr += ROW_ACCESS_DELAY;
                        if (last_buffer_row!=-1){
                            curr += ROW_ACCESS_DELAY;
                        }
                    }
                    curr += COL_ACCESS_DELAY;
                    Curr_executed[4] = to_string(curr);
                    Curr_executed[3] = "2";
                    break;
                }
                if (Curr_executed[3]==to_string(ROW_ACCESS_DELAY)){
                    if(Curr_executed[4]==to_string(2*ROW_ACCESS_DELAY+COL_ACCESS_DELAY)){
                        for (int j = 0;j<256;j++){
                            if (Dram_Memory[last_buffer_row][j] != Row_buffer[j]){
                                out<<"Memory at "<<row*256+col<<" = "<<to_string(Row_buffer[j])<<endl;
                            }
                        }
                        Dram_Memory[last_buffer_row] = Row_buffer;
                    }
                    if(Curr_executed[4]==to_string(ROW_ACCESS_DELAY+COL_ACCESS_DELAY)){
                        Row_buffer = Dram_Memory[row];
                        last_buffer_row = row;
                    }
                    break;
                }
                if (Curr_executed[3]==to_string(2*ROW_ACCESS_DELAY) && Curr_executed[4]==to_string(2*ROW_ACCESS_DELAY+COL_ACCESS_DELAY)){
                    Row_buffer = Dram_Memory[row];
                    last_buffer_row = row;
                    break;
                }

                if(Curr_executed[3]==Curr_executed[4]){
                    if(Curr_executed[0]=="lw"){
                        reg[Curr_executed[2]] = Row_buffer[col/4];
                        Blocking_registers[Curr_executed[2]] -=1;
                    }
                    if(Curr_executed[0] =="sw"){
                        Row_buffer[col/4] = stoi(Curr_executed[2]);
                    }
                    Dram_queue.erase(Dram_queue.begin());
                    break;
                }
                break;
            }
        }
        //Normal Commands 
        if (checker(pc)){
            Instruction = params[pc][0];
            if (Instruction == "lw"){
                string relevant_registor = params[pc][1];
                int offset = stoi(params[pc][2]);
                int address = offset+reg[params[pc][3]];
                Dram_queue.push_back({"lw",to_string(address),relevant_registor,"0","0"});   
                Blocking_registers[relevant_registor] ++;        
                pc++;
                continue;
            }else if(Instruction == "sw"){
                string relevant_registor = params[pc][1];
                int offset = stoi(params[pc][2]);
                int address = offset+reg[params[pc][3]];
                Dram_queue.push_back({"lw",to_string(address),to_string(reg[relevant_registor]),"0","0"});           
                pc++;
                continue;
            }else if (Instruction == "add"){ 
                add(pc);
                pc++;
                continue;
            }else if (Instruction == "sub"){
                sub(pc);
                pc++;
                continue;
            }else if (Instruction == "mul"){
                mul(pc);
                pc++;
                continue;
            }else if (Instruction == "addi"){
                addi(pc);
                pc++;
                continue;
            }else if (Instruction == "bne"){ 
                bne(pc);
                continue;
            }else if (Instruction == "beq"){ 
                beq(pc);
                continue;
            }else if (Instruction == "slt"){
                slt(pc);
                pc++;
                continue;
            }else if(Instruction == "j"){
                j(pc);
                continue;
            }else{
                pc++;
                continue;
            }
        }


    }
	return 0;
}
//------------------------------------Execution Ends---------------------------------------------------------------------
