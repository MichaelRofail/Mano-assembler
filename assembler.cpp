/*
 *This is an assembler program for the microprocessor shown in M. Morris Mano Computer
 * System Architecture for Advanced computer achitecture course Dr. Gamal Selim
 * Input files: 1-instructions.txt: holds the instruction set
 *              2-code.txt: holds the assembly code
 * Output file: binary.txt: holds the assembled hex code or machine language
 * Written by: Michael Hany
 */
#include <bits/stdc++.h>
#include <iostream>

#define INSTRUCTION_SET_SIZE 25
#define MAX_LABEL_COUNT 50

using namespace std;

struct instSet{
    string inst;
    int binary;
};

struct labels{
    string label;
    int address;
};

bool isLabel(string s);
int isLegalLabel(string s);
int isInstruction(string s, instSet* iSet, int iSetCount);

int main(){
    //load instruction set
    instSet iSet[INSTRUCTION_SET_SIZE];
    string s;
    stringstream temp;//temp for conversion form hex string to int
    ifstream fin;
    fin.open("instructions.txt", ios::in);
    int i = 0 , iSetCount = 0;
    //read in the instructions into the iSet array
    while(!fin.eof()){
        fin >> s;
        iSet[i].inst = s;
        fin >> s;
        temp.clear();
        temp << s; 
        temp >> hex >> iSet[i].binary;
        i++;
        iSetCount++;
    }
    fin.close();
    //pass 1
    fin.open("code.txt", ios::in);

    labels lTable[MAX_LABEL_COUNT];
    istringstream l;
    //ef is error flag
    bool ef = false;
    int lc = -1, lineCount = 0, labelCount = 0, n;
    i = 0;

    do{
        l.clear();
        lc++;
        lineCount++;
        getline(fin,s);
        l.str(s);
        l >> s;
        if(isLabel(s)){
            if(isLegalLabel(s) == 1){
                lTable[labelCount].label = s.substr(0,s.length()-1);
                lTable[labelCount].address = lc;
                labelCount++;
            }else{
                if(isLegalLabel(s) == 2) cout<<"ERROR:Label is too long at line: "<< lineCount <<endl;
                else if(isLegalLabel(s) == 3) cout<<"ERROR:Stray ',' line: "<< lineCount <<endl;
                else if(isLegalLabel(s) == 4) cout<<"ERROR:First character of a laber has to be a letter at line: "<< lineCount <<endl;
                else if(isLegalLabel(s) == 5) cout<<"ERROR:A Label must consist of only alpha numerical characters at line: "<< lineCount <<endl;
                else cout<<"ERROR:Illegal label at line: "<< lineCount <<endl;
                ef = true;
                continue;
            }
            l >> s;
        }
        
        if(!s.compare("ORG")){
            l >> std::hex >> lc;
        }else if(isInstruction(s, iSet, iSetCount) > -1 || !s.compare("END") || !s.compare("DEC") || !s.compare("HEX"));
        else if(s.empty()|| s.compare(" ")){
            cout<<"ERROR:empty line at line: " << lineCount << endl;
            ef = true;
        }else {
            cout<<"ERROR:Wrong instruction Or label missing a ',' at line: "<< lineCount <<endl;
            ef = true;
            continue;
        }
        if(!l.eof()){
            l >> s;
            for(n = 0; n < s.length(); n++){
                if(!isxdigit(s[n]))break;
            }
            if( n == s.length()|| isLegalLabel(s+',') == 1 || s[0] == '-'){//then it is a number or label or negative
                if(!l.eof()){
                    l >> s;
                    if(!s.compare("I")){
                        if(!l.eof()){
                            l >> s;
                            if(s[0] != '/'){
                                cout << "ERROR: comment missing a '/' at line "<< lineCount <<endl;
                                ef = true;
                            }continue;
                        }
                    }else{
                        if(s[0] != '/'){
                            cout << "ERROR: comment missing a '/' at line "<< lineCount <<endl;
                            ef = true;
                        }continue;
                    }          
                }
            }else{
                if(s[0] != '/')cout << "ERROR: comment missing a '/' at line "<< lineCount <<endl;
                ef = true;
                continue;
            }
        }   
    }while(!fin.eof());
    fin.close();
    //checks if the last instruction is END or not
    if(s.compare("END")){
        cout<<"ERROR:Program missing END or not complete at line: "<< lineCount <<endl;
        ef = true;
    }
    //end program if the first pass found errors in the codeS
    if(ef)return 0;
    else{
        lc = 0;
        lineCount = 1;
        int j = 0;
        fin.open("code.txt", ios::in);
        ofstream fout;
        fout.open("binary.txt");
        do{
            getline(fin,s);
            l.clear();
            l.str(s);
            s.clear();
            l >> s;
            if(isLabel(s)){
                s.clear();
                l >> s;
                if(!s.compare("DEC")){
                    s.clear();
                    l >> s;
                    temp.clear();
                    temp.str(s);
                    temp >> dec >> j;
                    if(j > 0)fout << hex << lc << ' ' << hex << setfill('0') << setw(4) << j << endl;
                    else { //incase of negative number truncate into 16 bits
                        temp.clear();
                        temp << hex << j;
                        temp >> s;
                        fout << hex << lc << ' ' << s.substr(s.length()-4,s.length()) << endl;
                    }
                }else if(!s.compare("HEX")){
                    s.clear();
                    l >> s;
                    temp.clear();
                    temp.str(s);
                    temp >> hex >> j;
                    fout << hex << lc << ' ' << setfill('0') << setw(4) << hex << j << endl;
                }
            }else if(!s.compare("ORG")){
                l >> hex >> lc;
                lc--;
            }else if(!s.compare("END")){
                break;
            }else if(!s.compare("DEC")){
                s.clear();
                l >> s;
                temp.clear();
                temp.str(s);
                temp >> dec >> j;
                if(j >= 0)fout << hex << lc << ' ' << hex << setfill('0') << setw(4) << j << endl;
                else { //in case of negative number truncate into 16 bits
                    temp.clear();
                    temp << hex << j;
                    temp >> s;
                    fout << hex << lc << ' ' << s.substr(s.length()-4,s.length()) << endl;
                }
            }else if(!s.compare("HEX")){
                s.clear();
                l >> s;
                temp.clear();
                temp.str(s);
                temp >> hex >> j;
                fout << hex << lc << ' ' << setfill('0') << setw(4) << hex << j << endl;
            }else{
                i = isInstruction(s, iSet, iSetCount);
                if(i < 7){
                    j = iSet[i].binary;
                    j = j << 12;
                    l >> s;
                    //if there is a label after instruction
                    if(isalpha(s[0])){
                        int k = 0;
                        for(; k < labelCount; k++){
                            if(!s.compare(lTable[k].label)){
                                j += lTable[k].address;
                                break;
                            }
                        }
                        if(k == labelCount)cout << "Label "<< s <<" not found in label table at line: " << lc << endl;
                    }else{
                        temp.clear();
                        temp.str(s);
                        temp >> hex >> i;
                        j += i;
                    }
                    //check if there is an i
                    if(!l.eof()){
                        l >> s;
                        if(s[0] == 'i' || s[0] == 'I' ) j += 0x8000;
                    }
                    fout << hex << lc << ' ' << hex << j << endl;
                }else{
                    fout << hex << lc << ' ' << hex << iSet[i].binary <<endl;
                }
            }

            l.clear();
            lc++;
            lineCount++;
        }while(!fin.eof());
        fout.close();
    }
    
}

bool isLabel(string s){
    if(s[s.length()-1] == ',')return true;
    else return false;
}

/*Returns 1 if label is legal
else: 2 if label is too long
else: 3 for stray ","
else: 4 for the first character is not a letter
else: 5 characters must be alpha numerical
**/
int isLegalLabel(string s){
    if(s.length() < 5 && s.length() > 1 && isalpha(s[0])){
        if(s.length() == 2)return true;
        else if(s.length() == 3 && isalnum(s[1]))return true;
        else if(s.length() == 4 && isalnum(s[1]) && isalnum(s[2]))return true;
        else return 5;
    }
    else if (s.length() >= 5 ){
        return 2;
    }else if(s.length() == 1 ){
        return 3;
    }else if (!isalpha(s[0])){
        return 4;
    }
    return false;
}
//return negative value if not found and a 0 or postive value if found and it representes the location
int isInstruction(string s,instSet* iSet, int iSetCount){
    for(int i = 0; i < iSetCount; i++){
        if(!s.compare(iSet[i].inst))return i;
    }
    return -1;
}