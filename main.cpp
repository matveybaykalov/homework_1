#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <fstream>

using std::cout;
using std::cin;
using std::string;
using std::endl;
using std::vector;
using std::ifstream;
using std::ofstream;

string GenerateGamma(){
    string gamma;
    for(int i = 0; i<16; ++i){
        gamma += static_cast<char>(std::rand()%64+192);
    }
    return gamma;
}

string XORencoder (string text, const vector<string>& gamma){
    int CharIter = 0;
    int VectorIter = 0;
    for (auto& i: text){
        i ^= gamma[VectorIter][CharIter];
        ++CharIter;
        if (CharIter%16 == 0){
            ++VectorIter;
            CharIter %= 16;
        }
    }
    return text;
}

struct Blok{
    unsigned long long first = 0;
    unsigned long long second = 0 ;
};

string LeftShift (const string& text){
    string chifrotext;
    for (int i = 0; i < (text.length()/16)+1; ++i){
        Blok blok;
        for(int j = 0; j < 8; ++j){
            if (i*16+j<text.length()) blok.first = blok.first*256+static_cast<unsigned char>(text[i*16+j]);
            if (i*16+8+j<text.length()) blok.second = blok.second*256+static_cast<unsigned char>(text[i*16+8+j]);
        }
        unsigned char temp = blok.first >> 58;
        unsigned char temp2 = blok.second >> 58;
        blok.first = blok.first << 6;
        blok.second = blok.second << 6;
        blok.first += temp2;
        blok.second += temp;
        string SubStr;
        for (int j = 0; j < 8; ++j){
            string s1;
            s1 += static_cast<char>(blok.first%256);
            SubStr.insert(0, s1);
            blok.first /= 256;
            string s2;
            s2 += static_cast<char>(blok.second%256);
            SubStr.insert(j+1, s2);
            blok.second /= 256;
        }
        chifrotext +=SubStr;
    }
    return chifrotext;
}

string RightShift (const string& text) {
    string chifrotext;
    for (int i = 0; i < text.length() / 16; ++i) {
        Blok blok;
        for (int j = 0; j < 8; ++j) {
            blok.first = blok.first * 256 + static_cast<unsigned char>(text[i * 16 + j]);
            blok.second = blok.second * 256 + static_cast<unsigned char>(text[i * 16 + 8 +j]);
        }
        unsigned long long temp = blok.first << 58;
        unsigned long long temp2 = blok.second << 58;
        blok.first = blok.first >> 6;
        blok.second = blok.second >> 6;
        blok.first += temp2;
        blok.second += temp;
        string SubStr;
        for (int j = 0; j < 8; ++j) {
            if (blok.first % 256 != 0){
                string s1;
                s1 += static_cast<char>(blok.first % 256);
                SubStr.insert(0, s1);
            }
            blok.first /= 256;
            if (blok.second % 256 != 0){
                string s2;
                s2 += static_cast<char>(blok.second % 256);
                SubStr.insert(j + 1, s2);
            }
            blok.second /= 256;
        }
        chifrotext += SubStr;
    }
    return chifrotext;
}
int main (){
    string AdressInput, AdressOutput;
    int operation;
    cout << "Select the program mode" << endl;
    cout << "1. encoding" << endl << "2. decoding" << endl;
    cin >> operation;
    if (operation == 1) {
        vector <string> gammas;
        string text, str;
        cout << "Specify the full path to the file input.txt" << endl;
        cin >> AdressInput;
        cout << "Specify the full path to the file output.txt" << endl;
        cin >> AdressOutput;
        ifstream input(AdressInput);
        while (std::getline(input, str)) {
            text += str + '\n';
        }
        std::srand(std::time(nullptr));
        int key = std::rand();
        std::srand(key);
        cout << "Remember this key: " << key;
        for (int i = 0; i < ceil(text.length() / 16.); ++i) {
            gammas.push_back(GenerateGamma());
        }
        string XORtext = XORencoder(text, gammas);
        string chifrotext = LeftShift(XORtext);
        ofstream output(AdressOutput);
        output << chifrotext;
    } else {
        string text, chifrotext, str, gamma;
        vector <string> gammas;
        cout << "Specify the full path to the file input.txt" << endl;
        cin >> AdressInput;
        cout << "Specify the full path to the file output.txt" << endl;
        cin >> AdressOutput;
        ifstream input(AdressInput);
        while (std::getline (input, str)){
            chifrotext += str;
        }
        cout << "Enter the key" << endl;
        int key;
        cin >> key;
        std::srand(key);
        for (int i = 0; i < chifrotext.length()/16; ++i){
            gammas.push_back(GenerateGamma());
        }
        string unchifrotext = RightShift(chifrotext);
        text = XORencoder(unchifrotext, gammas);
        ofstream output(AdressOutput);
        output << text;
    }
    return 0;
}