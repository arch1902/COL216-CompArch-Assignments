#include <iostream>
#include<string>
#include<algorithm>
#include<fstream>
#include<vector>
#include<map>
using namespace std;

fstream file1 ;
map<string,int> reg;

void print(){
    cout<<"R0  [r0] = "<<reg["$r0"]<<endl;
    cout<<"R1  [at] = "<<reg["$at"]<<endl;
    cout<<"R2  [v0] = "<<reg["$v0"]<<endl;
    cout<<"R3  [v1] = "<<reg["$v1"]<<endl;
    cout<<"R4  [a0] = "<<reg["$a0"]<<endl;
    cout<<"R5  [a1] = "<<reg["$a1"]<<endl;
    cout<<"R6  [a2] = "<<reg["$a2"]<<endl;
    cout<<"R7  [a3] = "<<reg["$a3"]<<endl;
    cout<<"R8  [t0] = "<<reg["$t0"]<<endl;
    cout<<"R9  [t1] = "<<reg["$t1"]<<endl;
    cout<<"R10 [t2] = "<<reg["$t2"]<<endl;
    cout<<"R11 [t3] = "<<reg["$t3"]<<endl;
    cout<<"R12 [t4] = "<<reg["$t4"]<<endl;
    cout<<"R13 [t5] = "<<reg["$t5"]<<endl;
    cout<<"R14 [t6] = "<<reg["$t6"]<<endl;
    cout<<"R15 [t7] = "<<reg["$t7"]<<endl;
    cout<<"R16 [s0] = "<<reg["$s0"]<<endl;
    cout<<"R17 [s1] = "<<reg["$s1"]<<endl;
    cout<<"R18 [s2] = "<<reg["$s2"]<<endl;
    cout<<"R19 [s3] = "<<reg["$s3"]<<endl;
    cout<<"R20 [s4] = "<<reg["$s4"]<<endl;
    cout<<"R21 [s5] = "<<reg["$s5"]<<endl;
    cout<<"R22 [s6] = "<<reg["$s6"]<<endl;
    cout<<"R23 [s7] = "<<reg["$s7"]<<endl;
    cout<<"R24 [t8] = "<<reg["$t8"]<<endl;
    cout<<"R25 [t9] = "<<reg["$t9"]<<endl;
    cout<<"R26 [k0] = "<<reg["$k0"]<<endl;
    cout<<"R27 [k1] = "<<reg["$k1"]<<endl;
    cout<<"R28 [gp] = "<<reg["$gp"]<<endl;
    cout<<"R29 [sp] = "<<reg["$sp"]<<endl;
    cout<<"R30 [s8] = "<<reg["$s8"]<<endl;
    cout<<"R31 [ra] = "<<reg["$ra"]<<endl;
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
        cout << hexaDeciNum[j];
}



int main(int argc, char *argv[]) {
	// your code goes here

    file1.open(argv[1]);

    vector<string> instructions;

    string line;
    int num=0;
    while(getline(file1,line)){
        instructions.push_back(line);
        num++;
    }
    file1.close();

    string curr;
    for(int i=0;i<num;){
        curr=instructions[i];
        //Process
        i++;
        print(); 
    }
  
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