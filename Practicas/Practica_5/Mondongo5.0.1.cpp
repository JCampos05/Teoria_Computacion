#include <iostream>
#include <stdlib.h>
#include <limits>
#include <algorithm>
#include <string> //manipulación de stings
#include <cctype>
#include <regex>
#include <stack>
#include <vector>
using namespace std;

string MainInput = "";

struct AP {
    char caracter;
    int posicion;
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
// funciones booleanas auxiliares
bool symbol(char simb) { // verifica si el caracter es un simbolo
    return simb == '+' || simb == '-' || simb == '*' || simb == '/' || simb == '^' || simb == '%';
}
bool Digito(char caracter) { // verifica si es un dígito
    return isdigit(caracter);
}
//funcion para determinar si el numero es positivo, negativo, entero o real
void auxNum(const string& Maininput, int& i){
    string numero = "";
    int inicio = i;
    bool negativo = false,  real = false;
    // Verificar si es negativo o positivo
    if (MainInput[i] == '-') {
        negativo = true;
        numero += MainInput[i];
        i++;
    } else if (MainInput[i] == '+') {
        numero += MainInput[i];
        i++;
    }
    while (i < MainInput.length() && (Digito(MainInput[i]) || MainInput[i] == '.')) {
        if (MainInput[i] == '.') {
            if (real) {
                cout << "ERROR: Múltiples puntos decimales en posición " << i << endl;
                return;
            }
            real = true;
        }
        numero += MainInput[i];
        i++;
    }
    i--; // Retroceder uno porque el bucle principal incrementará
    if (real) {
        cout << "Posición: " << inicio << "\t " << numero << " \t <-- Número float ";
        cout << (negativo ? " negativo." : " positivo.") << numero << endl;
    } else {
        cout << "Posición: " << inicio << "\t " << numero << " \t <-- Número entero ";        
        cout << (negativo ? " negativo." : " positivo.") <<  endl;
    }
}

bool MainProcess(string Maininput){
    stack <AP> AFP;
    for (int i = 0; i < MainInput.length(); i++) {
        char carac = MainInput[i];
        if (carac == ' ') continue;
        if (carac == '(') {
            cout << "Posición: "<< i << "\t " << carac <<" \t <-- Parentesis que abre" << endl;
            AP p;
            p.caracter = carac;
            p.posicion = i;
            AFP.push(p);
            // <----------------------- Funcion de mostrar pila
        } else if (carac == ')') {
            cout << "Posición: "<< i << "\t " << carac <<" \t <-- Parentesis que cierra" << endl;
            if (AFP.empty()) {
                cout << "Error. Parentesis que cierra inválido en la posición #:  " << i << endl;
                return false;
            }
            AFP.pop();
            // <----------------------- Funcion de mostrar pila
        } else if (symbol(carac)) {
            if ((carac == '-' || carac == '+') && (i == 0 || MainInput[i-1] == '(' || symbol(MainInput[i-1]))) {
                if (i + 1 < MainInput.length() && (Digito(MainInput[i+1]) || MainInput[i+1] == '.')) {
                    auxNum(MainInput, i);
                    continue;
                }
            }
            cout << "Posición: "<< i << "\t " << carac <<" \t <-- Símbolo" << endl;
        } else if (Digito(carac) || carac == '.') {
            auxNum(MainInput, i);
        } else {
            cout << "Error. Carácter inválido: " << carac << " en la posición: " << i << endl;
            return false; 
        }
    }
    if (!AFP.empty()) {
        AP p = AFP.top();
        cout << "Error. Parentesis que cierra inválido en la posición:  " << p.posicion << endl;
        return false;
    }
    cout << "Expresión válida. El uso de parentesis es correcto." << endl;
    return true;
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