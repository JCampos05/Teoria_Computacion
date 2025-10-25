#include <iostream>
#include <stdlib.h>
#include <limits>
#include <algorithm>
#include <string> //manipulación de stings
#include <cctype>
#include <regex>
#include <vector>
#include <fstream> //librerias .csv
#include <sstream> //librerias .csv
#include <iomanip>
#include <set>  
#define archivo1 "Program.txt"
using namespace std;

struct MainProgram {
    string linea;
};
vector <MainProgram> v_MP;

struct token {
    string lexema, tipo;
};
vector <token> v_tokens;

string palabrasReservadas[] = {
    "if", "else", "for", "while", "int", "float", "bool", 
    "char", "string", "true", "false", "input", "write",
    "return", "include", "system", "aputs_agets"
};
set <char> valid_symbols = {
    '+', '-', '*', '/', '%', '<', '>', '=', '!', 
    '&', '|', '^', '~', ';', ',', '.', ':', '(', 
    ')', '[', ']', '{', '}', '?', '"', '#'
};
set <string> valid_symbols2 = {
    "++" , "--" , "==" , "!=" , "<=" , ">=" , 
    "&&" , "||" , "+=" , ":=" 
};

void error() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    system("cls");
    cout << endl;
    cout << "Ingrese un dato válido" << endl;
    system("pause");
    system("cls");
}
/*
void sentence(string NoTerminal){
    if ("<>");
    
}
void set_sentc(string NoTerminal) { // <program> -> <set_sentc>
    if (NoTerminal == "<set_sentc>;<sentence>;") set_sentc("<set_sentc>"); 
    else if(NoTerminal == "<sentence>;");
    else return;
}*/
// Función para verificar si es palabra reservada
bool esPalabraReservada(string palabra) {
    int numPalabras = sizeof(palabrasReservadas) / sizeof(palabrasReservadas[0]);
    for(int i = 0; i < numPalabras; i++) {
        if(palabrasReservadas[i] == palabra) return true;
    }
    return false;
}
// Función para verificar si es un número
bool esNumero(string str) {
    if(str.empty()) return false;
    for(int i = 0; i < str.length(); i++) {
        if(!isdigit(str[i]) && str[i] != '.') return false;
    }
    return true;
}

// Función para verificar si es un identificador válido
bool esIdentificador(string str) {
    if(str.empty() || !isalpha(str[0])) return false;
    for(int i = 0; i < str.length(); i++) {
        if(!isalnum(str[i]) && str[i] != '_') return false;
    }
    return true;
}

string setType(string lexema){
    if(lexema.empty()) return "Desconocido";
    
    if(lexema.length() == 2 && valid_symbols2.find(lexema) != valid_symbols2.end()) 
        return "DoubleSimb";
    if(lexema.length() == 1 && valid_symbols.find(lexema[0]) != valid_symbols.end()) 
        return "SimpleSimb";
    // Palabras reservadas
    if(esPalabraReservada(lexema)) return "PR";
    // Números
    if(esNumero(lexema)) return "Num";
    // Cadenas entre comillas
    if(lexema.length() >= 2 && lexema[0] == '"' && lexema[lexema.length()-1] == '"') 
        return "Cadena";
    // Identificadores (variables)
    if(esIdentificador(lexema)) 
        return "Var";
    return "Desconocido";
}
void addToken(string lexema, string tipo){
    if(lexema.empty()) return;
    token t;
    t.lexema = lexema;
    t.tipo = tipo;
    v_tokens.push_back(t);
}
void analisisLexico(string line){
    bool comillas = false;
    string palabra = "" , cadena = "";
    for (int i = 0; i < line.length(); i++) {
        char car = line[i];

        if (car == '"') {
            if (!comillas) {
                if (!palabra.empty()) {
                    addToken(palabra, setType(palabra));
                    palabra = "";
                }
                comillas = true;
                cadena = "\"";
            } else {
                cadena += "\"";
                addToken(cadena, "Cadena");
                cadena = "";
                comillas = false;
            }
            continue;
        }
        if(comillas) {
            cadena += car;
            continue;
        }

        if (i < line.length() - 1) {
            string dobleChar = "";
            dobleChar += car;
            dobleChar += line[i+1];

            if(valid_symbols2.find(dobleChar) != valid_symbols2.end()) {
                if(!palabra.empty()) {
                    addToken(palabra, setType(palabra));
                    palabra = "";
                }
                if(dobleChar != "//") {
                    addToken(dobleChar, setType(dobleChar));  // ← CORREGIDO: usa dobleChar
                }
                i++;
                continue;
            }
        }
        if(valid_symbols.find(car) != valid_symbols.end()) {
            if(!palabra.empty()) {
                addToken(palabra, setType(palabra));
                palabra = "";
            }
            string simbolo = "";
            simbolo += car;
            addToken(simbolo, setType(simbolo));  // ← CORREGIDO: usa simbolo
        } else if (isspace(car)) {
            if(!palabra.empty()) {
                addToken(palabra, setType(palabra));
                palabra = "";
            }
        } else {
            palabra += car;
        }
    }
    if (!palabra.empty()) {
        addToken(palabra, setType(palabra));
        palabra = "";
    }
}
void tokenTable(){
    cout << endl;
    cout << "┌──────────────────┬───────────────────────────────────────────────┐" << endl;
    cout << "|------ Tipo ------|-------------------  Token  -------------------|"  << endl;
    cout << "├──────────────────┼───────────────────────────────────────────────┤"<<endl;
    for (int i = 0; i < v_tokens.size(); i++) {
        int n = 0, nfijo = 0, n2 = 0, nfijo2 = 0;
        n = v_tokens[i].tipo.length();
        nfijo = 19 - n;
        n2 = v_tokens[i].lexema.length();
        nfijo2 = 45 - n2;
        cout << "|   " << v_tokens[i].tipo << right << setw(nfijo) << " |   " << v_tokens[i].lexema << "" << right << setw(nfijo2) << " |" << endl ;
    }
    cout << "└──────────────────┴───────────────────────────────────────────────┘"<<endl;
}
void MainProcess(){
    tokenTable();
    
}
void loadPGM() {
    cout << "Cargando el programa..." << endl;
    ifstream archPGM(archivo1);
    if (!archPGM.is_open()) {
        cout<< "Error: No se pudo abrir el archivo." << endl;
        return;
    }
    string linea;
    getline(archPGM, linea);
    while (getline(archPGM, linea)) {
        stringstream cadena(linea); // Convertir la cadena a un stream
        string line;
        MainProgram data;

        getline(cadena, line);
        data.linea = line;

        v_MP.push_back(data);
        cout << data.linea << endl;
        analisisLexico(linea);
    }
    cout << endl;
    cout << "¡Programa cargado exitosamente!" << endl;
    system("pause");
}
void interfaz() { // bloque principal donde se solicita la entrada, posterior se manda a evaluar devolviendo un resultado
    int opc = 0 , seguir = 1;
    system("cls");
    do{
        do {
            system("cls");
            cout << "    ┌───────────────────────────────────────────┐" << endl;
            cout << "    |---| Práctica 7. Analizador sintáctico |---|" << endl;
            cout << "    └───────────────────────────────────────────┘" << endl;
            cout << endl;
            loadPGM();
            MainProcess();
            cout << endl;
            if (cin.fail()) error();
            cout<<"  ¿Desea volver a evaluar el programa? 1)Sí 2)No?  ", cin >> seguir;
            cout << endl;
            if (cin.fail()) error(); 
        } while (cin.fail() || seguir != 1 && seguir != 2);
        system("cls");
    } while (seguir == 1);
}
int main(int argc, char const *argv[]) {
    cout << "Bienvenido..." << endl;
	cout << endl;
    interfaz();
    system("pause");
    return 0;
}