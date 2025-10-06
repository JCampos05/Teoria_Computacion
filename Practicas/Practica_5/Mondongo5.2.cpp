#include <iostream>
#include <string>
#include <stack>
#include <cctype>
using namespace std;

// Estructura para almacenar información de un paréntesis
struct Parentesis {
    char caracter;
    int posicion;
};

// Función para verificar si un carácter es un operador
bool esOperador(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '%';
}

// Función para verificar si un carácter es un dígito
bool esDigito(char c) {
    return isdigit(c);
}

// Función para analizar y clasificar un número
void analizarNumero(string expresion, int& i) {
    string numero = "";
    int inicio = i;
    bool esNegativo = false;
    bool esReal = false;
    
    // Verificar si es negativo o positivo
    if (expresion[i] == '-') {
        esNegativo = true;
        numero += expresion[i];
        i++;
    } else if (expresion[i] == '+') {
        numero += expresion[i];
        i++;
    }
    
    // Leer dígitos y punto decimal
    while (i < expresion.length() && (esDigito(expresion[i]) || expresion[i] == '.')) {
        if (expresion[i] == '.') {
            if (esReal) {
                cout << "ERROR: Múltiples puntos decimales en posición " << i << endl;
                return;
            }
            esReal = true;
        }
        numero += expresion[i];
        i++;
    }
    i--; // Retroceder uno porque el bucle principal incrementará
    
    // Clasificar e imprimir el número
    if (esReal) {
        cout << "Posición " << inicio << ": Número REAL ";
        cout << (esNegativo ? "NEGATIVO" : "POSITIVO") << " -> " << numero << endl;
    } else {
        cout << "Posición " << inicio << ": Número ENTERO ";
        cout << (esNegativo ? "NEGATIVO" : "POSITIVO") << " -> " << numero << endl;
    }
}

// Función para mostrar el contenido de la pila
void mostrarPila(stack<Parentesis> pila) {
    if (pila.empty()) {
        cout << "   [Pila vacía]" << endl;
        return;
    }
    
    cout << "   [Pila: ";
    stack<Parentesis> temp;
    
    // Pasar elementos a pila temporal para mostrarlos
    while (!pila.empty()) {
        temp.push(pila.top());
        pila.pop();
    }
    
    // Mostrar la pila (de abajo hacia arriba)
    while (!temp.empty()) {
        cout << "1";
        temp.pop();
        if (!temp.empty()) cout << ", ";
    }
    cout << "]" << endl;
}

// Función principal para validar y analizar la expresión
bool validarYAnalizar(string expresion) {
    stack<Parentesis> pila;
    
    cout << "\n=== ANÁLISIS DE LA EXPRESIÓN ===" << endl;
    cout << "Expresión: " << expresion << endl << endl;
    cout << "=== COMPORTAMIENTO DE LA PILA ===" << endl;
    
    for (int i = 0; i < expresion.length(); i++) {
        char c = expresion[i];
        
        // Ignorar espacios
        if (c == ' ') continue;
        
        // Detectar paréntesis que abre
        if (c == '(') {
            cout << "Posición " << i << ": PARÉNTESIS QUE ABRE '('" << endl;
            Parentesis p;
            p.caracter = c;
            p.posicion = i;
            pila.push(p);
            cout << "   → Acción: PUSH (meter 1 a la pila)" << endl;
            mostrarPila(pila);  // ← AQUÍ se llama
        }
        // Detectar paréntesis que cierra
        else if (c == ')') {
            cout << "Posición " << i << ": PARÉNTESIS QUE CIERRA ')'" << endl;
            if (pila.empty()) {
                cout << "\n❌ ERROR: Paréntesis que cierra sin correspondiente que abre en posición " << i << endl;
                return false;
            }
            cout << "   → Acción: POP (sacar 1 de la pila)" << endl;
            pila.pop();
            mostrarPila(pila);  // ← AQUÍ se llama
        }
        // Detectar operadores
        else if (esOperador(c)) {
            // Si es '-' o '+' al inicio o después de '(' o de otro operador, es parte de un número
            if ((c == '-' || c == '+') && 
                (i == 0 || expresion[i-1] == '(' || esOperador(expresion[i-1]))) {
                if (i + 1 < expresion.length() && (esDigito(expresion[i+1]) || expresion[i+1] == '.')) {
                    analizarNumero(expresion, i);
                    continue;
                }
            }
            cout << "Posición " << i << ": OPERADOR '" << c << "'" << endl;
        }
        // Detectar números
        else if (esDigito(c) || c == '.') {
            analizarNumero(expresion, i);
        }
        // Carácter inválido
        else {
            cout << "\n❌ ERROR: Carácter inválido '" << c << "' en posición " << i << endl;
            return false;
        }
    }
    
    // Verificar si quedaron paréntesis sin cerrar
    if (!pila.empty()) {
        Parentesis p = pila.top();
        cout << "\n❌ ERROR: Paréntesis que abre sin cerrar en posición " << p.posicion << endl;
        return false;
    }
    
    cout << "\n✅ La expresión es VÁLIDA - Todos los paréntesis están balanceados" << endl;
    return true;
}

int main() {
    string expresion;
    
    cout << "╔════════════════════════════════════════════════════╗" << endl;
    cout << "║   VALIDADOR DE EXPRESIONES MATEMÁTICAS            ║" << endl;
    cout << "╚════════════════════════════════════════════════════╝" << endl;
    cout << "\nIngresa una expresión matemática: ";
    getline(cin, expresion);
    
    validarYAnalizar(expresion);
    
    system("pause");
    return 0;
}