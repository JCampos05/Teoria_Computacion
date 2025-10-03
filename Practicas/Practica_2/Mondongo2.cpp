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

int Transition_table [13][10] = {
        {1, 2, 6, 12, 12, 2, 12, 12, 4, -5},
        {2, 2, 3, -10, -10, 2, -10, -10, -10, 101}, 
        {3, -20, 3, -10, -10, 2, -10, -10, -10, 101},
        {4, 4, 4, 4, 4, 4, 4, 4, 5, -5},
        {5, 0, 0, 0, 0, 0, 0, 0, 0, 102}, 
        {6, -30, 6, -40, 7, 9, -40, -40, -40, 103},
        {7, -30, 8, -40, 7, 9, -40, -40, -40, -5},
        {8, -30, 8, -40, -40, 9, -40, -40, -40, 104},
        {9, -30, -50, -50, -50, -50, 10, 10, -50, -5},
        {10, -30, 11, -40, -40, -30, -40, -40, -40, -5},
        {11, -30, 11, -40, -40, -30, -40, -40, -40, 105},
        {12, -30, -60, 13, 13, -60, 13, 13, 13, 106},
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 107},
    };

//sets con los caracteres y palabras reservadas
set <char> valid_symbols = {
    '+', '-', '*', '/', '%', '<', '>', '=', '!', 
    '&', '|', '^', '~', ';', ',', '.', ':', '(', 
    ')', '[', ']', '{', '}', '?', '"', '\'', '\\', '#'
};
const set <string> palabras_reservadas= {
    // De C
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int", "long", "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while",
    // Específicas de C++
    "asm", "bool", "catch", "class", "const_cast", "delete", "dynamic_cast",
    "explicit", "export", "false", "friend", "inline", "mutable", "namespace",
    "new", "operator", "private", "protected", "public", "reinterpret_cast",
    "static_cast", "template", "this", "throw", "true", "try", "typeid",
    "typename", "using", "virtual", "wchar_t","where"
};

void error(){
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    system("cls");
    cout << endl;
    cout << "Ingrese un dato válido" << endl;
    cout << "Error -90. Tipo desconocido. No es posible la acción" << endl;
    system("pause");
    system("cls");
}
//funcion de asignacion de error
string errores(int NoEstate) {
    //int aux = Transition_table[NoEstate][token];
    switch (NoEstate) {
        case 0: return "Error -70. No es posible la acción. Entrada no válida.";
        case -10: return "Error -10. No se permite el uso de caracteres especiales.";
        case -20: return "Error -20. No se permite el uso de letras posterior a un digito.";
        case -30: return "Error -30. No se permite el uso de letras.";
        case -40: return "Error -40. No se perimte el uso de caracteres especiales.";
        case -50: return "Error -50. No se permite el uso de digitos ni letras en los caracteres de notacion cientifica. Solo esta permitido los signos + y -";
        case -60: return "Error -60. No se permite el uso de letras ni digitos.";
        case -5: return "Error -5. Hubo un error de reconocimiento.";
        default: return  "Error -5. Hubo un error de reconocimiento.";
    }
}

//funcion de identificador final de la entrada
string identfinal(int NoEstate, int token){
    int aux = Transition_table[NoEstate][token];
    cout << aux << endl;
    switch (aux) {
        case 101: return "variable";
        case 102: return "string";
        case 103: return "integer";
        case 104: return "float";
        case 105: return "double";
        case 106: return "simbolo simple";
        case 107: return "simbolo compuesto";
        default:  return  errores(NoEstate) ;//"Error -90. Tipo desconocido. No es posible la acción";
    }
}
bool Palabra(const string& input) { // funcion para detectar la posible palabra reservada 
    for (const string& palabra : palabras_reservadas) {
        // Crear patrón: palabra completa (no substring)
        string patron = "\\b" + palabra + "\\b";
        regex reg(patron);
        if (regex_search(input, reg)) {
            cout << "Encontrada palabra reservada: " << palabra << endl;
            return true;
        }
    }
    return false;
}

//funcion para determinar si el caracter analizado es de un conjunto
bool symbol(char simbolo) {
    return valid_symbols.find(simbolo) != valid_symbols.end();
}
int tokens(char simbolo) { // funcion para asignar token en las filas de la matriz (eje j)
    //cout << simbolo << endl;
    if (simbolo == '.') return 4;  
    if (simbolo == 'E') return 5;
    if (simbolo == '+') return 6;  
    if (simbolo == '-') return 7;   
    if (simbolo == '"') return 8;
    if (isalpha(simbolo)) return 1;
    if (isdigit(simbolo)) return 2;
    if (symbol(simbolo)) return 3;
    return 0;
}
//inicia Table [0,0]
// funcion principal llamada en la interfaz de consola donde realiza todo el trabajo algoritmico
string MainInput = ""; // entrada principal de lectura
int Estate = 0; // En la matriz es la coordenada [0][0]; siendo el estado #1
void Mainprocess() {
    int NoEstate = 0, token = 0; // eje i,j correspondientemente -> las dos variables que recorren la matriz
    Estate = 0; // limpia el #estado en caso de que se haya presentado un error en la anterior evaluacion
    for (int a = 0; a < MainInput.length(); a++) { // hace el ciclo de cada caracter del input
        if (Estate < 0) break; // detecta si directamente es un error
        char simbolo = MainInput[a]; 
        token = tokens( simbolo ); //asigna el token del eje j
        Estate = Transition_table[NoEstate][token]; // asigna el # estado correspondiente
        /* Ojo las variables i,j son las que recorren pero recordar que estas se les resta 
        una posicion porque la matriz comienza en [0,0], independientemente de que esten declaradas como [13][10] */
        /*cout << "# Estado Actual :" << NoEstate   << endl;
        cout << "# Estado Siguiente:" << Estate << endl;
        cout << "# Token :" << token << endl; 
        cout << endl; */
        NoEstate = Estate - 1 ; //resta la posicion para el siguiente recorrido de la matriz
    }
    //NoEstate ++;
    string resul;
    /*cout << "===================" << endl;    
    cout << "# Estado Actual :" << NoEstate   << endl;
    cout << "# Estado Siguiente:" << Estate << endl;
    cout << "# Token :" << token << endl; */
    if (Estate > 0) {
        if (NoEstate == 1 || NoEstate == 2) {
            if (Palabra(MainInput)) {
                cout << "Error -80. No es posible la acción. Palabra reservada." << endl;
                return;
            }
        }
        token = 9;
        resul = identfinal(NoEstate, token);
        cout << "    ┌─────────────────────────────────────┐" << endl;
        cout << "    |---| Resultado de la evaluación: |---|" << endl;
        cout << "    └─────────────────────────────────────┘" << endl;
        cout << "     La entrada corresponde a: " << resul << endl;
        cout << endl;
    } else {
        resul = errores(NoEstate);
        cout << "    ┌─────────────────────────────────────┐" << endl;
        cout << "    |---| Resultado de la evaluación: |---|" << endl;
        cout << "    └─────────────────────────────────────┘" << endl;
        cout << "     La entrada corresponde a: " << resul << endl;
        cout << endl;
    }
}
void interfaz() {
    int opc = 0 , seguir = 1;
    system("cls");
    do{
        do {
            system("cls");
            cout << "    ┌───────────────────────────────────────┐" << endl;
            cout << "    |---| Práctica 2. Análizador léxico |---|" << endl;
            cout << "    └───────────────────────────────────────┘" << endl;
            cout << endl;
            cout << "    Ingrese la entrada a evaluar:"<<endl;
            cout << endl;
            cout << "Entrada:  ";
            cin >> MainInput , cout << " \t";
            cout << endl;
            if (cin.fail()){
                error();
            }
            Mainprocess();
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