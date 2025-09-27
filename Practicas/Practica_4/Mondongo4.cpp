#include <iostream>
#include <stdlib.h>
#include <limits>
#include <algorithm>
#include <string> //manipulación de stings
#include <cctype>
#include <vector>
#include <set>  
#include <regex>
using namespace std;

string MainInput = "";

void error(){
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    system("cls");
    cout << endl;
    cout << "Ingrese un dato válido" << endl;
    system("pause");
    system("cls");
}

bool NomApe(const string& Maininput){
    regex patron (R"(^[A-Z][a-z]+(\s[A-Z][a-z]+)*$)");
    if (regex_match(MainInput, patron)) return true; else return false;
}
bool Edad(const string& Maininput){
    regex patron (R"(^(1[8-9]|[2-9][0-9])$)");
    if (regex_match(MainInput, patron)) return true; else return false;
}
bool telefono(const string& Maininput){
    regex patron (R"(^(\d{2}\s?\d{4}\s?\d{4}|\d{3}\s?\d{3}\s?\d{4})$)");
    if (regex_match(MainInput, patron)) return true; else return false;
}
bool salario(const string& Maininput){
    regex patron (R"(^(278\.(1[8-9]|[2-9]\d*)|27[9](\.\d+)?|28[0-9](\.\d+)?|29\d(\.\d+)?|[3-9]\d{2}(\.\d+)?|\d{4,}(\.\d+)?)$)");
    if (regex_match(MainInput, patron)) return true; else return false;
}
string Mainprocess(const string& Maininput){
    if (NomApe(MainInput)) return "Coincidencia. Nombre y/o apellido encontrado";
    if (Edad(MainInput)) return "Coincidencia. Edad encontrada";
    if (telefono(MainInput)) return "Coindicenia. Número de télefono encontrado";
    if (salario(MainInput)) return "Coincidencia. Salario mínimo";
    return "";
}

void interfaz() {
    int opc = 0 , seguir = 1;
    system("cls");
    do{
        do {
            system("cls");
            cout << "    ┌───────────────────────────────────────────┐" << endl;
            cout << "    |---| Práctica 4. Expresiones Regulares |---|" << endl;
            cout << "    └───────────────────────────────────────────┘" << endl;
            cout << endl;
            cout << "    Ingrese la entrada a evaluar:"<<endl;
            cout << endl;
            cout << "Entrada:  ";
            cin >> MainInput , cout << " \t";
            cout << endl;
            if (cin.fail()){
                error();
            }
            cout << Mainprocess(MainInput) << endl;
            cout<<"  ¿Desea realizar otra evaluación? 1)Sí 2)No?  ", cin >> seguir;
            cout << endl;
            if (cin.fail()){
                error();
            }
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