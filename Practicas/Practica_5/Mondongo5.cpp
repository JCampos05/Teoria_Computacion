#include <iostream>
#include <stdlib.h>
#include <limits>
#include <algorithm>
#include <string> //manipulación de stings
#include <cctype>
#include <regex>
#include <set>
#include <stack>
#include <vector>
using namespace std;

string MainInput = "";
stack <char> AFP;
vector<char> cadena;
vector<string> numbers;

set <char> valid_symbols = {
    '+', '-', '*', '/', '^'
};

void error(){
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    system("cls");
    cout << endl;
    cout << "Ingrese un dato válido" << endl;
    system("pause");
    system("cls");
}
//funcion para determinar si el caracter analizado es de un conjunto
bool symbol(char simbolo) {
    return valid_symbols.find(simbolo) != valid_symbols.end();
}

void prueba() {
    for (int i = 0; i < cadena.size(); i++) {
        cout << cadena.at(i) << endl;
    }
}
void MainProcess(const string& Maininput) {
    cadena.clear();
    if (MainInput[0] != '(') return; else AFP.push('1');
    // mete cada caracter ingresado a un vector
    for (char c : MainInput) {
        cadena.push_back(c);
    }
    prueba();  //imprime caracter por caracter
    string n = "";
    for (int i = 1; i < cadena.size(); i++) { // inicia en 1 porque arriba ya se analizó que inicie con parentesis
        char simbolo = cadena.at(i);
        if (simbolo == '(' || ')') {
            if (simbolo == '(') {
                AFP.push('1');
                cout << simbolo <<" \t <-- Parentesis que abre" << endl;
            } else {
                AFP.pop();
                cout << simbolo <<" \t <-- Parentesis que cierra" << endl;
            }
        } else if (isdigit(simbolo) || ((simbolo == '-') && (isdigit(cadena.at(i + 1))))) {     
            if (simbolo == '-') { // Numero negativo
                n =+ simbolo;
            } else { // Numero positivo
                n =+ simbolo;
            }
            n += simbolo;
        } else if (symbol(simbolo)) {

        }
    }
}
void interfaz() { // bloque principal donde se solicita la entrada, posterior se manda a evaluar devolviendo un resultado
    int opc = 0 , seguir = 1;
    system("cls");
    do{
        do {
            system("cls");
            cout << "    ┌────────────────────────────────────────────────────────────┐" << endl;
            cout << "    |---| Práctica 5. Validación de parentesis mediante pila |---|" << endl;
            cout << "    └────────────────────────────────────────────────────────────┘" << endl;
            cout << endl;
            cout << "    Ingrese la entrada a evaluar:"<<endl;
            cout << endl;
            cout << "Entrada:   ";
            cin >> MainInput , cout << " \t";
            string aux;
            getline(cin,aux); // tambien toma los espacios en blanco \s
            MainInput += aux; // concatena los espacios en blanco
            cout << endl;
            MainProcess(MainInput);
            if (cin.fail()) error();
            cout<<"  ¿Desea realizar otra evaluación? 1)Sí 2)No?  ", cin >> seguir;
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