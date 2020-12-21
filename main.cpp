#include <iostream>
#include <string>
#include <ctime>
#include <fstream>

using std::cout;
using std::cin;
using std::string;
using std::endl;
using std::ifstream;
using std::ofstream;

struct Blok{
    unsigned long long first = 0;
    unsigned long long second = 0 ;
};

Blok GenerateGamma(){
    Blok gamma;
    for(int i = 0; i<8; ++i){
        gamma.first <<= 8;
        gamma.first += rand() % 256;
        gamma.second <<= 8;
        gamma.second += rand() % 256;
    }
    return gamma;
}

void operator<< (unsigned long long& number, const string& str){
    for (unsigned char ch : str){
        number <<= 8;
        number += ch;
    }
}

void operator<< (string& str, unsigned long long& number){
    for (int i = 0; i < 8; ++i){
        string s1;
        s1 = static_cast<char>(number % 256); //пришлось создать строку из 1 символа, чтобы воспользоваться функцией insert
        str.insert(0, s1);
        number >>= 8;
    }
}

void operator<< (Blok& blok, const int& n){
    int a = 64-n;
    unsigned char temp = blok.first >> a;
    unsigned char temp2 = blok.second >> a;
    blok.first = blok.first << n;
    blok.second = blok.second << n;
    blok.first += temp2;
    blok.second += temp;
}

void operator>> (Blok& blok, const int& n){
    int a = 64-n;
    unsigned long long temp = blok.first << a;
    unsigned long long temp2 = blok.second << a;
    blok.first = blok.first >> 6;
    blok.second = blok.second >> 6;
    blok.first += temp2;
    blok.second += temp;
}

void operator^(Blok& blok, const Blok& gamma){
    blok.first ^= gamma.first;
    blok.second ^= gamma.second;
}

void encoding (string& text, string& AddressOutput){
    ofstream output (AddressOutput, std::ios::binary);
    //эта функция сразу будет проводить побитовое ИЛИ с 16-байтовыми числами и побитовый сдвиг
    while (text.length()%16 != 0){
        text += '\n';
    }
    for (int i = 0; i < text.length()/16; ++i){
        Blok blok;
        blok.first << text.substr(i*16, 8);
        blok.second << text.substr(i*16+8, 8);
        blok ^ GenerateGamma();
        blok << 6;
        output.write(reinterpret_cast<const char *>(&blok.first), sizeof(blok.first));
        output.write(reinterpret_cast<const char *>(&blok.second), sizeof(blok.second));
    }
}

string decoding (const string& AddressInput) {
    ifstream input (AddressInput, std::ios::binary);
    Blok blok;
    string text;
    while (input.read(reinterpret_cast<char *>(&blok.first), sizeof(blok.first))){
        input.read(reinterpret_cast<char *>(&blok.second), sizeof(blok.second));
        blok >> 6;
        blok ^ GenerateGamma();
        string s1, s2;
        s1 << blok.first;
        s2 << blok.second;
        text += s1 + s2;
    }
    while (text[text.length()-1] == '\n'){
        text.erase(text.length()-1, 1);
    }
    return text;
}
int main (){
    string AddressInput, AddressOutput;
    int operation;
    cout << "Select the program mode" << endl;
    cout << "1. encoding" << endl << "2. decoding" << endl;
    cin >> operation;
    if (operation == 1) {
        string text, str;
        cout << "Specify the full path to the file input.txt" << endl;
        cin >> AddressInput;
        cout << "Specify the full path to the file output.txt" << endl;
        cin >> AddressOutput;
        ifstream input(AddressInput);
        while (std::getline(input, str)) {
            text += str + '\n';
        }
        std::srand(std::time(nullptr));
        int key = std::rand();
        std::srand(key);
        cout << "Remember this key: " << key;
        encoding(text, AddressOutput);
    } else {
        string text;
        cout << "Specify the full path to the file input.txt" << endl;
        cin >> AddressInput;
        cout << "Specify the full path to the file output.txt" << endl;
        cin >> AddressOutput;
        cout << "Enter the key" << endl;
        int key;
        cin >> key;
        std::srand(key);
        text = decoding(AddressInput);
        ofstream output (AddressOutput);
        output << text;
    }
    return 0;
}
