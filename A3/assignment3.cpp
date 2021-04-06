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
vector<string> instructions; //Vector containing all instructions 
regex n("[-]?[0-9]+");  //for checking if a string is convertible to an Integer
regex l("([A-Z|a-z])[A-Z|a-z|0-9|_]*"); // for Label
int n_total;
int INSTRUCTION_MEMORY = pow(2,17); // Memory is word Addressable hence it has 2^19 Bytes
int DATA_MEMORY = pow(2,19)-4;
int ROW_ACCESS_DELAY  = 10;
int COL_ACCESS_DELAY = 2;
int clock_cycle = 0;
int last_buffer_row = -1;
int pc = 0;
int num=0;


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
    //cout<<"Invalid input "<<s<<" found on line "<<l<<endl;
    //exit(-1);
}
string dectohex(int n){
    stringstream ss; 
    ss << hex << n; 
    string res = ss.str(); 
    return res;    
}
// Prints the registers in decimal format
void print(){
    out<<" "<<reg["$zero"];
    out<<" "<<reg["$r1"];
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

void DRAM_activity_manager(int count, int expectation, int row, int last_row, int col){
    if (expectation == 2*ROW_ACCESS_DELAY+COL_ACCESS_DELAY){
        if(count<=ROW_ACCESS_DELAY){
            out<<"Write back to row "<<last_row<<" Completed "<<count<<"/"<<ROW_ACCESS_DELAY<<endl;
        }else if(count<=2*ROW_ACCESS_DELAY){
            out<<"Activating row "<<row<<" Completed "<<count-ROW_ACCESS_DELAY<<"/"<<ROW_ACCESS_DELAY<<endl;
        }else if(count < 2*ROW_ACCESS_DELAY+COL_ACCESS_DELAY){
            out<<"Accessing column "<<col<<" Completed "<<count-2*ROW_ACCESS_DELAY<<"/"<<COL_ACCESS_DELAY<<endl;
        }else{
            out<<"Accessing column "<<col<<" Completed "<<count-2*ROW_ACCESS_DELAY<<"/"<<COL_ACCESS_DELAY<<endl;
            out<<"Completed DRAM accsess!!"<<endl;
        }
    }else if(expectation == ROW_ACCESS_DELAY+COL_ACCESS_DELAY){
        if(count<=ROW_ACCESS_DELAY){
            out<<"Activating row "<<row<<" Completed "<<count<<"/"<<ROW_ACCESS_DELAY<<endl;
        }else if(count < ROW_ACCESS_DELAY+COL_ACCESS_DELAY){
            out<<"Accessing column "<<col<<" Completed "<<count-ROW_ACCESS_DELAY<<"/"<<COL_ACCESS_DELAY<<endl;
        }else{
            out<<"Accessing column "<<col<<" Completed "<<count-ROW_ACCESS_DELAY<<"/"<<COL_ACCESS_DELAY<<endl;
            out<<"Completed DRAM accsess!!"<<endl;
        }
    }else{
        if(count < COL_ACCESS_DELAY){
            out<<"Accessing column "<<col<<" Completed "<<count<<"/"<<COL_ACCESS_DELAY<<endl;
        }else{
            out<<"Accessing column "<<col<<" Completed "<<count<<"/"<<COL_ACCESS_DELAY<<endl;
            out<<"Completed DRAM accsess!!"<<endl;;
        }
    }
    return;
}

bool checker(int l, string s){
    if(l>=num){return false;}
    string ins = params[l][0];
    if (ins == "j"){
        return true;
    }else if (ins == "lw"||ins == "sw"){
        return false;
    }else if(ins == "add"||ins =="addi"||ins =="sub"||ins == "mul"||ins == "bne"||ins =="beq"){
        if (params[l][1] == s){return false;}
        else if(params[l][2]== s){return false;}
        else if(params[l][3]== s){return false;}
        else {return true;}
    }else{
        return true;
    }
}
bool checker_sw(int l){
    if(l>=num){return false;}
    string ins = params[l][0];
    if (ins == "lw"||ins == "sw"){
        return false;
    }else{
        return true;
    }
}
void add(int pc){
    int y = (params[pc][3][0]=='$') ? reg[params[pc][3]] : stoi(params[pc][3]);
    reg[params[pc][1]]=reg[params[pc][2]]+y;
    out <<instructions[pc]<<endl;
    out<<params[pc][1]<<" = "<<reg[params[pc][1]]<<endl;
}
void sub(int pc){
    int y = (params[pc][3][0]=='$') ? reg[params[pc][3]] : stoi(params[pc][3]);
    reg[params[pc][1]]=reg[params[pc][2]]-y;
    out <<instructions[pc]<<endl;
    out<<params[pc][1]<<" = "<<reg[params[pc][1]]<<endl;
}
void mul(int pc){
    int y = (params[pc][3][0]=='$') ? reg[params[pc][3]] : stoi(params[pc][3]);
    reg[params[pc][1]]=reg[params[pc][2]]*y;
    out <<instructions[pc]<<endl;
    out<<params[pc][1]<<" = "<<reg[params[pc][1]]<<endl;
}
void addi(int pc){
    reg[params[pc][1]]=reg[params[pc][2]]+stoi(params[pc][3]);
    out <<instructions[pc]<<endl;
    out<<params[pc][1]<<" = "<<reg[params[pc][1]]<<endl;
}
void slt(int pc){
    int y = (params[pc][3][0]=='$') ? reg[params[pc][3]] : stoi(params[pc][3]);
    if(reg[params[pc][2]]< y ){
        reg[params[pc][1]] = 1;
    }else {
        reg[params[pc][1]] = 0;
    }
    out <<instructions[pc]<<endl;
    out<<params[pc][1]<<" = "<<reg[params[pc][1]]<<endl;
}
void beq(int pc){
    if (label.find(params[pc][3]) == label.end()){
        cout << "Invalid Label on line "<<pc+1<<endl;
        exit(-1);
    }
    int y = (params[pc][2][0]=='$') ? reg[params[pc][2]] : stoi(params[pc][2]);
    if (reg[params[pc][1]] == y){
        pc = label[params[pc][3]];
    }else{
        pc++;
    }
    out <<instructions[pc]<<endl;
}
void bne(int pc){
    if (label.find(params[pc][3]) == label.end()){
        cout << "Invalid Label on line "<<pc+1<<endl;
        exit(-1);
    }
    int y = (params[pc][2][0]=='$') ? reg[params[pc][2]] : stoi(params[pc][2]);
    if (reg[params[pc][1]] != y){
        pc = label[params[pc][3]];
    }else{
        pc++;
    }
    out <<instructions[pc]<<endl;
}
void j(int pc){
    if (label.find(params[pc][1]) == label.end()){
        cout << "Invalid Label on line "<<pc+1;
        exit(-1);
    }
    pc = label[params[pc][1]];
    out <<instructions[pc]<<endl;
}
int main(int argc, char *argv[]) {
    file1.open(argv[1]);
    if(!file1.is_open()){cout<<"File not found"<<endl;exit(-1);}
    string input_file = argv[1];
    out.open(input_file+"_output_test.txt");
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
    for (auto const& j : params){
         int c = j.first;
         vector<string> v = j.second;
         cout<<c<<"\n";
         for(auto j: v){
             cout<<string(j)<<" ";
         }
         cout<<"\n";
    }
    // for(auto const& j : label){
    //     cout<<j.first<<"->"<<j.second<<"   ";
    // }
    // cout<<endl;
    // Program Counter that iterates over the instruction set and points to the instruction being executed
    while(pc<num){
        cout<<pc<<endl;
        if(instructions[pc] == ""){pc++;continue;}
        Instruction = params[pc][0];
        if(Instruction == "lw"){
            cout<<params[pc][0]<<" "<<params[pc][1]<<" "<<params[pc][2]<<" "<<params[pc][3]<<endl;
            int expected_clock_cycle = 0;
            int offset = stoi(params[pc][2]);
            int address = offset+reg[params[pc][3]];
            if(address%4 != 0 || address<0  || address>DATA_MEMORY){
                cout<<"Program tried to access invalid memory location."<<endl;
                exit(-1);
            }
            int row = address/1024;
            int col = address%1024;
            if (row != last_buffer_row){
                expected_clock_cycle += ROW_ACCESS_DELAY;
                if (last_buffer_row!= -1){
                    expected_clock_cycle += ROW_ACCESS_DELAY;
                }
            }
            expected_clock_cycle += COL_ACCESS_DELAY;
            string relevant_registor = params[pc][1];
            cout<<"SSSSSSSSSSSS"<<relevant_registor<<endl;
            bool lookahead  = true;
            clock_cycle++;
            pc++;
            out<<"Clock cycle "<<clock_cycle<<":"<<endl;print();
            out<<"DRAM request issued by Instruction "<<instructions[pc]<<endl;
            for(int i = 0;i<expected_clock_cycle;i++){
                if (lookahead){
                    lookahead = checker(pc+1,relevant_registor);
                }
                cout<<lookahead<<"  "<<params[pc][0]<<endl;
                if(lookahead){
                    clock_cycle++;
                    out<<"Clock cycle "<<clock_cycle<<":"<<endl;print();
                    pc++;
                    Instruction = params[pc][0];
                    if (Instruction=="add"){add(pc);}
                    else if (Instruction=="sub"){sub(pc);}
                    else if (Instruction=="mul"){mul(pc);}
                    else if (Instruction=="addi"){addi(pc);}
                    else if (Instruction=="beq"){beq(pc);}
                    else if (Instruction=="bne"){bne(pc);}
                    else if (Instruction=="j"){j(pc);}
                    else {continue;}
                }else{
                    clock_cycle++;
                    out<<"Clock cycle "<<clock_cycle<<":"<<endl;print(); 
                }
                DRAM_activity_manager(i+1,expected_clock_cycle,row,last_buffer_row,col);
            }
            last_buffer_row = row;
            reg[relevant_registor] = data_memory[address];
            out<< relevant_registor<<" = "<<data_memory[address]<<endl;
        }else if (Instruction =="sw"){
            cout<<params[pc][0]<<" "<<params[pc][1]<<" "<<params[pc][2]<<" "<<params[pc][3]<<endl;
            int expected_clock_cycle = 0;
            int offset = stoi(params[pc][2]);
            int address = offset+reg[params[pc][3]];
            if(address%4 != 0 || address<0  || address>DATA_MEMORY){
                cout<<"Program tried to access invalid memory location."<<endl;
                exit(-1);
            }
            int row = address/1024;
            int col = address%1024;
            if (row != last_buffer_row){
                expected_clock_cycle += ROW_ACCESS_DELAY;
                if (last_buffer_row!= -1){
                    expected_clock_cycle += ROW_ACCESS_DELAY;
                }
            }
            expected_clock_cycle += COL_ACCESS_DELAY;
            int to_be_stored = reg[params[pc][1]];
            bool lookahead  = true;
            clock_cycle++;
            pc++;
            out<<"Clock cycle "<<clock_cycle<<":"<<endl;print();
            out<<"DRAM request issued by Instruction "<<instructions[pc]<<endl;
            cout<<expected_clock_cycle<<"  "<<pc<<endl;
            for(int i = 0;i<expected_clock_cycle;i++){
                if (lookahead){
                    lookahead = checker_sw(pc+1);
                }
                cout<<lookahead<<"  "<<pc<<"  "<<params[pc][0]<<endl;
                if(lookahead){
                    clock_cycle++;
                    out<<"Clock cycle "<<clock_cycle<<":"<<endl;print();
                    pc++;
                    Instruction = params[pc][0];
                    if (Instruction=="add"){add(pc);}
                    else if (Instruction=="sub"){sub(pc);}
                    else if (Instruction=="mul"){mul(pc);}
                    else if (Instruction=="addi"){addi(pc);}
                    else if (Instruction=="beq"){beq(pc);}
                    else if (Instruction=="bne"){bne(pc);}
                    else if (Instruction=="j"){j(pc);}
                    else {cout<<"wtf "<<num<<endl;continue;}
                }else{
                    clock_cycle++;
                    out<<"Clock cycle "<<clock_cycle<<":"<<endl;print(); 
                }
                DRAM_activity_manager(i+1,expected_clock_cycle,row,last_buffer_row,col);
            }
            last_buffer_row = row;
            data_memory[address] = to_be_stored;
            out<<"Memory at " <<address<<" = "<<to_be_stored<<endl; 
        }else if (Instruction == "add"){
            clock_cycle++;
            out<<"Clock cycle "<<clock_cycle<<":"<<endl;print(); 
            add(pc);
            pc++;
        }else if (Instruction == "sub"){
            clock_cycle++;
            out<<"Clock cycle "<<clock_cycle<<":"<<endl;print(); 
            sub(pc);
            pc++;
        }else if (Instruction == "mul"){
            clock_cycle++;
            out<<"Clock cycle "<<clock_cycle<<":"<<endl;print(); 
            mul(pc);
            pc++;
        }else if (Instruction == "addi"){
            clock_cycle++;
            out<<"Clock cycle "<<clock_cycle<<":"<<endl;print(); 
            addi(pc);
            pc++;
        }else if (Instruction == "bne"){
            clock_cycle++;
            out<<"Clock cycle "<<clock_cycle<<":"<<endl;print(); 
            bne(pc);
        }else if (Instruction == "beq"){
            clock_cycle++;
            out<<"Clock cycle "<<clock_cycle<<":"<<endl;print(); 
            beq(pc);
        }else if (Instruction == "slt"){
            clock_cycle++;
            out<<"Clock cycle "<<clock_cycle<<":"<<endl;print(); 
            slt(pc);
            pc++;
        }else if(Instruction == "j"){
            clock_cycle++;
            out<<"Clock cycle "<<clock_cycle<<":"<<endl;print(); 
            j(pc);
        }else{
            pc++;
        }
    }
    out.close();
	return 0;
}
//------------------------------------Execution Ends---------------------------------------------------------------------
