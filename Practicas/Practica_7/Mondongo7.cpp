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
int acumToken = 0;
vector <string> Error;


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

// Obtener el token actual sin avanzar
token peek() {
    if (acumToken < v_tokens.size()) return v_tokens[acumToken];
    token empty = {"", "FDC"};
    return empty;
}

// Agregar error al vector de errores
void reportError(string mensaje) {
    token t = peek();
    string err = "Error sintáctico en token '" + t.lexema + "': " + mensaje;
    Error.push_back(err);
}

// Avanzar al siguiente token
void advance() {
    if (acumToken < v_tokens.size()) acumToken++;
}

// Verificar si el token actual coincide con el esperado
bool match(string lexema) {
    token t = peek();
    return t.lexema == lexema;
}
// Verificar si el token actual es de cierto tipo
bool matchType(string tipo) {
    token t = peek();
    return t.tipo == tipo;
}

// Consumir un token esperado
bool consume(string lexema) {
    if (match(lexema)) {
        advance();
        return true;
    }
    return false;
}

// FUNCIONES
bool program();
bool set_sentc();
bool sentence();
bool varAsign();
bool inputVar();
bool outputVar();
bool sentExpMath();
bool expLogic();
bool sentTerm();
bool sentElement();
bool sentExpAnd();
bool sentExpNot();
bool sentCmp();
bool sentId();
bool sentExpIf();
bool sentExpFor();
//

bool sentExpFor() {
    if (!consume("for")) return false;

    if (!consume("(")) {
        reportError("Se esperaba '(' después de 'for'");
        return false;
    }
    
    // Inicialización (declaración de variable)
    if (!varAsign()) {
        reportError("Inicialización inválida en 'for'");
        return false;
    }
    
    if (!consume(";")) {
        reportError("Se esperaba ';' después de la inicialización");
        return false;
    }
    
    // Condición
    if (!expLogic()) {
        reportError("Condición inválida en 'for'");
        return false;
    }
    
    if (!consume(";")) {
        reportError("Se esperaba ';' después de la condición");
        return false;
    }
    
    // Incremento/Decremento
    if (!matchType("Var")) {
        reportError("Se esperaba una variable en el incremento");
        return false;
    }
    advance();
    
    if (!consume("++") && !consume("--")) {
        reportError("Se esperaba '++' o '--'");
        return false;
    }
    
    if (!consume(")")) {
        reportError("Se esperaba ')' después del incremento");
        return false;
    }
    
    if (!consume("{")) {
        reportError("Se esperaba '{' para abrir bloque for");
        return false;
    }
    
    if (!set_sentc()) {
        reportError("Bloque de sentencias inválido en 'for'");
        return false;
    }
    
    if (!consume("}")) {
        reportError("Se esperaba '}' para cerrar bloque for");
        return false;
    }
    
    return true;
}
bool sentExpIf() {
    if (!consume("if")) return false;

    if (!consume("(")) {
        reportError("Se esperaba '(' después de 'if'");
        return false;
    }
    
    if (!expLogic()) {
        reportError("Expresión lógica inválida en 'if'");
        return false;
    }
    
    if (!consume(")")) {
        reportError("Se esperaba ')' después de la condición");
        return false;
    }

    if (!consume("{")) {
        reportError("Se esperaba '{' para abrir bloque if");
        return false;
    }
    
    // ✓ Parsear conjunto de sentencias dentro del bloque
    if (!set_sentc()) {
        reportError("Bloque de sentencias inválido en 'if'");
        return false;
    }
    
    if (!consume("}")) {
        reportError("Se esperaba '}' para cerrar bloque if");
        return false;
    }
    
    // Manejar else/else if
    if (consume("else")) {
        if (match("if")) {
            return sentExpIf(); // Recursión para else if
        }

        if (!consume("{")) {
            reportError("Se esperaba '{' para abrir bloque else");
            return false;
        }
        
        if (!set_sentc()) {
            reportError("Bloque de sentencias inválido en 'else'");
            return false;
        }
        
        if (!consume("}")) {
            reportError("Se esperaba '}' para cerrar bloque else");
            return false;
        }   
    }
    
    return true;
}
bool sentId(){ // <id> ::= <nums> | <num> | var | "chain" | <float>
    if (matchType("Num") || matchType("Var") || matchType("Cadena")) {
        advance();
        return true;
    }
    return false;
}

bool sentCmp(){ // <cmp> ::= <id> <log_ope> <id> | <exp_log> | true | false
    if (match("true") || match("false")) {
        advance();
        return true;
    }

    if (consume("(")) {
        if (!expLogic()) {
            reportError("Expresión lógica inválida entre paréntesis");
            return false;
        }
        if (!consume(")")) {
            reportError("Se esperaba ')' para cerrar expresión lógica");
            return false;
        }
        return true;
    }

    int savePos = acumToken;

    if (sentId()) {
        if (match("<") || match(">") || match("<=") || match(">=") || 
            match("==") || match("!=")) {
            advance(); //consume el operador
            if (sentId()) return true;
        }
    }
    // Si falla, restaurar y probar expresión lógica recursiva
    acumToken = savePos;
    return expLogic();
}
bool sentExpNot(){ // <exp_not> ::= !<exp_not> | <cmp>
    if (consume("!")) return sentExpNot();
    return sentCmp();
}

// <exp_and> ::= <exp_not> ( && <exp_not> )*
bool sentExpAnd() {
    if (!sentExpNot()) {
        return false;
    }
    
    while (consume("&&")) {
        if (!sentExpNot()) {
            reportError("Se esperaba expresión después de '&&'");
            return false;
        }
    }
    
    return true;
}

bool sentElement(){ // <element> ::= (<sent_exp_math>) | <id> | <num> | -<term> | +<term>
    if (consume("(")) {
        if (!sentExpMath()) {
            reportError("Expresión inválida dentro de paréntesis");
            return false;
        }
        if (!consume(")")) {
            reportError("Se esperaba ')' para cerrar la expresión");
            return false;
        }
        return true;
    }
    
    if (consume("+") || consume("-")) return sentTerm(); // operadores +  y -

    if (matchType("Num") || matchType("Var")){
        advance();
        return true;
    }

    reportError("Se esperaba un número, variable o expresión entre paréntesis");
    return false;
}

bool sentTerm(){
    if (!sentElement()) return false;

    while (match("*") || match("/") || match("%")) {
        advance();
        if (!sentElement()) {
            reportError("Se esperaba un elemento después del operador");
            return false;
        }
    }
    return true;
}

bool expLogic() {
    if (!sentExpAnd()) {
        return false;
    }
    
    while (consume("||")) {
        if (!sentExpAnd()) {
            reportError("Se esperaba expresión después de '||'");
            return false;
        }
    }
    
    return true;
}

bool sentExpMath(){
    if (!sentTerm()) return false;

    while (match("+") || match("-")) {
        advance();
        if (!sentTerm()) {
            reportError("Se esperaba un término después del operador");
            return false;
        }
    }
    return true;
}

bool outputVar(){ // <output_var> ::= write(<List_output>);
    if (!consume("write")) return false;
    
    if (!consume("(")) {
        reportError("Se esperaba '(' después de 'write'");
        return false;
    }
    
    // Primer elemento
    if (!sentId() && !matchType("Cadena")) {
        if (matchType("Cadena")) {
            advance();
        } else {
            reportError("Se esperaba un elemento de salida");
            return false;
        }
    }
    if (matchType("Cadena")) {
        advance();
    }
    
    // Puede haber más elementos separados por coma
    while (consume(",")) {
        if (matchType("Cadena")) {
            advance();
        } else if (!sentId()) {
            reportError("Se esperaba un elemento de salida después de ','");
            return false;
        }
    }
    
    if (!consume(")")) {
        reportError("Se esperaba ')' después de la lista de salida");
        return false;
    }
    
    return true;
}

bool inputVar(){ // <input_var> ::= input(<ListIds>);
    if (!consume("input")) return false;

    if (!consume("(")) {
        reportError("Se esperaba '(' después de 'input'");
        return false;
    }
    
    // Al menos un identificador
    if (!matchType("Var")) {
        reportError("Se esperaba un identificador en input");
        return false;
    }
    advance();
    
    // Puede haber más identificadores separados por coma
    while (consume(",")) {
        if (!matchType("Var")) {
            reportError("Se esperaba un identificador después de ','");
            return false;
        }
        advance();
    }
    
    if (!consume(")")) {
        reportError("Se esperaba ')' después de la lista de identificadores");
        return false;
    }
    return true;
}

bool varAsign(){ 
    token t = peek();
    string tipo = t.lexema;

    if (!consume(tipo)) return false; // Consume el tipo

    do {
        if (!matchType("Var")) {
            reportError("Se esperaba un identificador después del tipo '" + tipo + "'");
            return false;
        }
        string varName = peek().lexema; // ✓ Guardar nombre para debug
        advance(); // consumir identificador
        
        if (!consume("=")) {
            reportError("Se esperaba '=' después de '" + varName + "'");
            return false;
        }
        
        if (tipo == "int" || tipo == "float") {
            if (!sentExpMath()) {
                reportError("Se esperaba una expresión numérica");
                return false;
            }
        } else if (tipo == "bool") {
            if (match("true") || match("false")) {
                advance();
            } else if (!expLogic()) {
                reportError("Se esperaba 'true', 'false' o expresión lógica");
                return false;
            }
        } else if (tipo == "char" || tipo == "string") {
            if (matchType("Cadena")) {
                advance();
            } else if (matchType("Var")) {
                advance();
            } else {
                reportError("Se esperaba una cadena o variable");
                return false;
            }
        }
        
    } while (consume(",")); 
    
    return true;
}

bool sentence(){ 
    token t = peek();

    // Verificar fin de archivo
    if (t.tipo == "FDC" || t.lexema == "}") {
        return false;
    }

    // Declaraciones de variables con tipo
    if (t.lexema == "int" || t.lexema == "float" || t.lexema == "bool" ||
        t.lexema == "char" || t.lexema == "string") {
        return varAsign();
    }

    if (t.lexema == "input") return inputVar();
    
    if (t.lexema == "write") return outputVar();

    if (t.lexema == "if") return sentExpIf();

    if (t.lexema == "for") return sentExpFor();

    // ✓ ASIGNACIÓN DE VARIABLE EXISTENTE (sin tipo)
    if (matchType("Var")) {
        int savePos = acumToken;
        advance(); // Consumir variable
        
        if (consume("=")) {
            // Caso 1: Literal booleano
            if (match("true") || match("false")) {
                advance();
                return true;
            }
            
            // Caso 2: Expresión matemática (números, operaciones)
            int savePos2 = acumToken;
            if (sentExpMath()) {
                return true;
            }
            
            // Si falla matemática, no intentar lógica porque causaría problemas
            reportError("Expresión inválida después de '='");
            return false;
        }
        
        // No es asignación, puede ser expresión
        acumToken = savePos;
        if (sentExpMath()) {
            return true;
        }
        
        reportError("No se pudo parsear la sentencia con variable");
        return false;
    }

    // Expresiones matemáticas que empiezan con número o paréntesis
    if (matchType("Num") || match("(")) {
        return sentExpMath();
    }

    reportError("Sentencia no reconocida: '" + t.lexema + "'");
    return false;
}
bool set_sentc() {
    while (peek().tipo != "FDC" && peek().lexema != "}") {
        token antes = peek();
        
        if (!sentence()) return false;
        
        // Verificar si la sentencia fue una estructura de control
        token despues = v_tokens[acumToken - 1];
        
        // Solo pedir ; si NO terminó con }
        if (despues.lexema != "}") {
            if (!consume(";")) {
                reportError("Se esperaba ';' después de la sentencia");
                return false;
            }
        }
    }
    
    return true;
}
bool program() { // <program> ::= <set_sentc> 
    bool result = set_sentc();
    if( acumToken < v_tokens.size()) {
        reportError("Tokens adicionales después del final del programa");
        return false;
    }
    return result;
}

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
    
    bool tienePunto = false;
    int inicio = 0;
    
    // Permitir signo al inicio
    if(str[0] == '+' || str[0] == '-') {
        inicio = 1;
        if(str.length() == 1) return false;
    }
    
    for(int i = inicio; i < str.length(); i++) {
        if(str[i] == '.') {
            if(tienePunto) return false; // Más de un punto
            tienePunto = true;
        } else if(!isdigit(str[i])) {
            return false;
        }
    }
    
    // No puede terminar o empezar solo con punto
    if(str == "." || str == "+." || str == "-.") return false;
    
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

        // ✓ MANEJAR PUNTO DECIMAL
        if (car == '.' && !palabra.empty() && i + 1 < line.length() && isdigit(line[i+1])) {
            // Es un decimal, agregar el punto a la palabra
            palabra += car;
            continue;
        }

        // Verificar operadores de dos caracteres
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
                    addToken(dobleChar, setType(dobleChar));
                }
                i++;
                continue;
            }
        }
        
        // Símbolos simples (excepto punto que puede ser decimal)
        if(valid_symbols.find(car) != valid_symbols.end()) {
            if(!palabra.empty()) {
                addToken(palabra, setType(palabra));
                palabra = "";
            }
            string simbolo = "";
            simbolo += car;
            addToken(simbolo, setType(simbolo));
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
        if(v_tokens[i].tipo == "Desconocido" ){
            cout << "Error en la linea "  << i+1 << endl;
        }
        int n = 0, nfijo = 0, n2 = 0, nfijo2 = 0;
        n = v_tokens[i].tipo.length();
        nfijo = 19 - n;
        n2 = v_tokens[i].lexema.length();
        nfijo2 = 45 - n2;
        cout << "|   " << v_tokens[i].tipo << right << setw(nfijo) << " |   " << v_tokens[i].lexema << "" << right << setw(nfijo2) << " |" << endl ;
    }
    cout << "└──────────────────┴───────────────────────────────────────────────┘"<<endl;
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
void MainProcess(){
    v_MP.clear();
    v_tokens.clear();
    Error.clear();

    acumToken = 0;
    loadPGM();
    tokenTable();
    
    cout << "Total de tokens: " << v_tokens.size() << endl;

    bool resultFinal = program();

    if (Error.empty()) {
        cout << "\n✓ Análisis sintáctico completado sin errores" << endl;
        cout << "✓ El programa es sintácticamente correcto" << endl;
    } else {
        cout << "\nSe encontraron " << Error.size() << " errores sintácticos:" << endl;
        for (int i = 0; i < Error.size(); i++) {
            cout << (i+1) << ". " << Error[i] << endl;
        }
        // ✓ MOSTRAR DÓNDE SE DETUVO
        cout << "\nToken actual (posición " << acumToken << "): ";
        if (acumToken < v_tokens.size()) {
            cout << "'" << v_tokens[acumToken].lexema << "' [" << v_tokens[acumToken].tipo << "]" << endl;
        } else {
            cout << "FIN" << endl;
        }
    }
    
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