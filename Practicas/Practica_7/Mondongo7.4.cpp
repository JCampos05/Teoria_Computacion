#include <iostream>
#include <stdlib.h>
#include <limits>
#include <algorithm>
#include <string>
#include <cctype>
#include <regex>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <set>
#define archivo1 "test.txt"
using namespace std;

struct MainProgram {
    string linea;
};
vector <MainProgram> v_MP;

struct token {
    string lexema, tipo;
    int linea; // Número de línea donde aparece el token
};
vector <token> v_tokens;
int currentToken = 0; // Índice del token actual
vector<string> errores; // Vector para almacenar errores
int lineaActual = 1; // Contador de líneas para el análisis léxico

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

// ============= FUNCIONES AUXILIARES PARA EL PARSER =============

// Obtener el token actual sin avanzar
token peek() {
    if (currentToken < v_tokens.size()) {
        return v_tokens[currentToken];
    }
    token empty = {"", "EOF"};
    return empty;
}

// Avanzar al siguiente token
void advance() {
    if (currentToken < v_tokens.size()) {
        currentToken++;
    }
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

// Agregar error al vector de errores
void reportError(string mensaje) {
    token t = peek();
    string error = "Error sintáctico en token '" + t.lexema + "': " + mensaje;
    errores.push_back(error);
}

// ============= FUNCIONES DEL ANALIZADOR SINTÁCTICO =============

// Forward declarations
bool parseSetSentc();
bool parseSentence();
bool parseVarAsign();
bool parseInputVar();
bool parseOutputVar();
bool parseSentExpMath();
bool parseTerm();
bool parseElement();
bool parseExpLog();
bool parseExpAnd();
bool parseExpNot();
bool parseCmp();
bool parseSentIf();
bool parseSentFor();
bool parseId();

// <program> ::= <set_sentc>
bool parseProgram() {
    cout << "\n=== Iniciando Análisis Sintáctico ===" << endl;
    bool result = parseSetSentc();
    
    if (currentToken < v_tokens.size()) {
        reportError("Tokens adicionales después del final del programa");
        return false;
    }
    
    return result;
}

// <set_sentc> ::= <set_sentc> ; <sentence>; | <sentence>;
// Modificado: if/for no requieren ; al final
bool parseSetSentc() {
    if (!parseSentence()) {
        return false;
    }
    
    // Verificar punto y coma (excepto para if/for que ya tienen sus bloques)
    token prev = v_tokens[currentToken - 1];
    if (prev.lexema != "}") {  // Si no terminó en }, necesita ;
        if (!consume(";")) {
            reportError("Se esperaba ';' después de la sentencia");
            return false;
        }
    }
    
    // Recursión: puede haber más sentencias
    while (peek().tipo != "EOF" && peek().lexema != "}") {
        if (!parseSentence()) {
            return false;
        }
        
        // Verificar punto y coma según el tipo de sentencia
        token prev = v_tokens[currentToken - 1];
        if (prev.lexema != "}") {  // Si no terminó en }, necesita ;
            if (!consume(";")) {
                reportError("Se esperaba ';' después de la sentencia");
                return false;
            }
        }
    }
    
    return true;
}

// <sentence> ::= <var_asign> | <input_var> | <output_var> | <sent_exp_math> | <sent_if> | <sent_for> | <id>
bool parseSentence() {
    token t = peek();
    
    // Verificar declaración de variable (int, float, bool, char, string)
    if (t.lexema == "int" || t.lexema == "float" || t.lexema == "bool" || 
        t.lexema == "char" || t.lexema == "string") {
        return parseVarAsign();
    }
    
    // Verificar input
    if (t.lexema == "input") {
        return parseInputVar();
    }
    
    // Verificar write
    if (t.lexema == "write") {
        return parseOutputVar();
    }
    
    // Verificar if
    if (t.lexema == "if") {
        return parseSentIf();
    }
    
    // Verificar for
    if (t.lexema == "for") {
        return parseSentFor();
    }
    
    // Puede ser una expresión matemática o un identificador
    if (matchType("Var") || matchType("Num") || match("(")) {
        return parseSentExpMath();
    }
    
    reportError("Sentencia no reconocida");
    return false;
}

// <var_asign> ::= <var_type>
// Soporta declaración múltiple: int x = 5, y = 10, z = 15;
bool parseVarAsign() {
    token t = peek();
    string tipo = t.lexema;
    
    if (!consume(tipo)) {
        return false;
    }
    
    // Parsear primera variable
    do {
        // Debe seguir un identificador
        if (!matchType("Var")) {
            reportError("Se esperaba un identificador después del tipo '" + tipo + "'");
            return false;
        }
        advance(); // consumir identificador
        
        // Debe seguir '='
        if (!consume("=")) {
            reportError("Se esperaba '=' en la asignación");
            return false;
        }
        
        // Valor de asignación
        if (tipo == "int" || tipo == "float") {
            if (!parseSentExpMath()) {
                reportError("Se esperaba una expresión numérica");
                return false;
            }
        } else if (tipo == "bool") {
            if (!match("true") && !match("false") && !parseExpLog()) {
                reportError("Se esperaba 'true', 'false' o expresión lógica");
                return false;
            }
            if (match("true") || match("false")) {
                advance();
            }
        } else if (tipo == "char" || tipo == "string") {
            if (!matchType("Cadena") && !matchType("Var")) {
                reportError("Se esperaba una cadena o variable");
                return false;
            }
            advance();
        }
        
        // Si hay coma, continuar con más declaraciones
    } while (consume(","));
    
    return true;
}

// <input_var> ::= input(<ListIds>);
bool parseInputVar() {
    if (!consume("input")) {
        return false;
    }
    
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

// <output_var> ::= write(<List_output>);
bool parseOutputVar() {
    if (!consume("write")) {
        return false;
    }
    
    if (!consume("(")) {
        reportError("Se esperaba '(' después de 'write'");
        return false;
    }
    
    // Primer elemento
    if (!parseId() && !matchType("Cadena")) {
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
        } else if (!parseId()) {
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

// <sent_exp_math> ::= <sent_exp_math> + <term> | <sent_exp_math> - <term> | <term>
bool parseSentExpMath() {
    if (!parseTerm()) {
        return false;
    }
    
    while (match("+") || match("-")) {
        advance();
        if (!parseTerm()) {
            reportError("Se esperaba un término después del operador");
            return false;
        }
    }
    
    return true;
}

// <term> ::= <term> * <element> | <term> / <element> | <term> % <element> | <element>
bool parseTerm() {
    if (!parseElement()) {
        return false;
    }
    
    while (match("*") || match("/") || match("%")) {
        advance();
        if (!parseElement()) {
            reportError("Se esperaba un elemento después del operador");
            return false;
        }
    }
    
    return true;
}

// <element> ::= (<sent_exp_math>) | <id> | <num> | -<term> | +<term>
bool parseElement() {
    // Paréntesis
    if (consume("(")) {
        if (!parseSentExpMath()) {
            reportError("Expresión inválida dentro de paréntesis");
            return false;
        }
        if (!consume(")")) {
            reportError("Se esperaba ')' para cerrar la expresión");
            return false;
        }
        return true;
    }
    
    // Operadores unarios
    if (consume("+") || consume("-")) {
        return parseTerm();
    }
    
    // Número o variable
    if (matchType("Num") || matchType("Var")) {
        advance();
        return true;
    }
    
    reportError("Se esperaba un número, variable o expresión entre paréntesis");
    return false;
}

// <exp_log> ::= (<exp_log>) || (<exp_and>) | <exp_and>
bool parseExpLog() {
    if (consume("(")) {
        if (!parseExpLog()) {
            return false;
        }
        if (!consume(")")) {
            reportError("Se esperaba ')' para cerrar expresión lógica");
            return false;
        }
        
        if (consume("||")) {
            if (!consume("(")) {
                reportError("Se esperaba '(' después de '||'");
                return false;
            }
            if (!parseExpAnd()) {
                return false;
            }
            if (!consume(")")) {
                reportError("Se esperaba ')' después de expresión AND");
                return false;
            }
        }
        return true;
    }
    
    return parseExpAnd();
}

// <exp_and> ::= (<exp_and>) && (<exp_not>) | <exp_not>
bool parseExpAnd() {
    if (consume("(")) {
        if (!parseExpAnd()) {
            return false;
        }
        if (!consume(")")) {
            reportError("Se esperaba ')' para cerrar expresión AND");
            return false;
        }
        
        if (consume("&&")) {
            if (!consume("(")) {
                reportError("Se esperaba '(' después de '&&'");
                return false;
            }
            if (!parseExpNot()) {
                return false;
            }
            if (!consume(")")) {
                reportError("Se esperaba ')' después de expresión NOT");
                return false;
            }
        }
        return true;
    }
    
    return parseExpNot();
}

// <exp_not> ::= !<exp_not> | <cmp>
bool parseExpNot() {
    if (consume("!")) {
        return parseExpNot();
    }
    return parseCmp();
}

// <cmp> ::= <id> <log_ope> <id> | <exp_log> | true | false
bool parseCmp() {
    if (match("true") || match("false")) {
        advance();
        return true;
    }
    
    // Guardar posición para backtracking
    int savedPos = currentToken;
    
    // Intentar parsear como comparación
    if (parseId()) {
        if (match("<") || match(">") || match("<=") || match(">=") || 
            match("==") || match("!=")) {
            advance(); // consumir operador
            if (parseId()) {
                return true;
            }
        }
    }
    
    // Si falla, restaurar y probar expresión lógica recursiva
    currentToken = savedPos;
    return parseExpLog();
}

// <sent_if> ::= if (<exp_log>) {<set_sentc>} | if (<exp_log>) {<set_sentc>} else {<set_sentc>}
bool parseSentIf() {
    if (!consume("if")) {
        return false;
    }
    
    if (!consume("(")) {
        reportError("Se esperaba '(' después de 'if'");
        return false;
    }
    
    if (!parseExpLog()) {
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
    
    if (!parseSetSentc()) {
        reportError("Bloque de sentencias inválido en 'if'");
        return false;
    }
    
    if (!consume("}")) {
        reportError("Se esperaba '}' para cerrar bloque if");
        return false;
    }
    
    // Opcional: else
    if (consume("else")) {
        if (match("if")) {
            return parseSentIf(); // else if
        }
        
        if (!consume("{")) {
            reportError("Se esperaba '{' para abrir bloque else");
            return false;
        }
        
        if (!parseSetSentc()) {
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

// <sent_for> ::= for (<numeric> ; <exp_log> ; <inc>){<set_sentc>}
bool parseSentFor() {
    if (!consume("for")) {
        return false;
    }
    
    if (!consume("(")) {
        reportError("Se esperaba '(' después de 'for'");
        return false;
    }
    
    // Inicialización (declaración de variable)
    if (!parseVarAsign()) {
        reportError("Inicialización inválida en 'for'");
        return false;
    }
    
    if (!consume(";")) {
        reportError("Se esperaba ';' después de la inicialización");
        return false;
    }
    
    // Condición
    if (!parseExpLog()) {
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
    
    if (!parseSetSentc()) {
        reportError("Bloque de sentencias inválido en 'for'");
        return false;
    }
    
    if (!consume("}")) {
        reportError("Se esperaba '}' para cerrar bloque for");
        return false;
    }
    
    return true;
}

// <id> ::= <nums> | <num> | var | "chain" | <float>
bool parseId() {
    if (matchType("Num") || matchType("Var") || matchType("Cadena")) {
        advance();
        return true;
    }
    return false;
}

// ============= FUNCIONES ORIGINALES DEL ANALIZADOR LÉXICO =============

bool esPalabraReservada(string palabra) {
    int numPalabras = sizeof(palabrasReservadas) / sizeof(palabrasReservadas[0]);
    for(int i = 0; i < numPalabras; i++) {
        if(palabrasReservadas[i] == palabra) return true;
    }
    return false;
}

bool esNumero(string str) {
    if(str.empty()) return false;
    for(int i = 0; i < str.length(); i++) {
        if(!isdigit(str[i]) && str[i] != '.') return false;
    }
    return true;
}

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
    if(esPalabraReservada(lexema)) return "PR";
    if(esNumero(lexema)) return "Num";
    if(lexema.length() >= 2 && lexema[0] == '"' && lexema[lexema.length()-1] == '"') 
        return "Cadena";
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
                    addToken(dobleChar, setType(dobleChar));
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
            cout << "Error léxico en token "  << i+1 << ": " << v_tokens[i].lexema << endl;
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
        stringstream cadena(linea);
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

void mostrarErrores() {
    if (errores.empty()) {
        cout << "\n✓ Análisis sintáctico completado sin errores" << endl;
        cout << "✓ El programa es sintácticamente correcto" << endl;
    } else {
        cout << "\n✗ Se encontraron " << errores.size() << " errores sintácticos:" << endl;
        cout << "================================================" << endl;
        for (int i = 0; i < errores.size(); i++) {
            cout << (i+1) << ". " << errores[i] << endl;
        }
        cout << "================================================" << endl;
    }
}

void MainProcess(){
    // Limpiar vectores
    v_tokens.clear();
    v_MP.clear();
    errores.clear();
    currentToken = 0;
    
    loadPGM();
    tokenTable();
    
    // Realizar análisis sintáctico
    bool resultado = parseProgram();
    
    // Mostrar resultados
    mostrarErrores();
    
    system("pause");
}

void interfaz() {
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