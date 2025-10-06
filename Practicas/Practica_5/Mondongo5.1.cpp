#include <iostream>
#include <string>
#include <stack>
#include <cctype>
using namespace std;

class ValidadorExpresiones {
private:
    string expresion;
    
    bool esOperador(char c) {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '%';
    }
    
    bool esDigito(char c) {
        return isdigit(c);
    }
    
    void analizarNumero(int& i) {
        string numero = "";
        int inicio = i;
        bool esNegativo = false;
        bool esReal = false;
        
        // Verificar si es negativo
        if (expresion[i] == '-') {
            esNegativo = true;
            numero += expresion[i];
            i++;
        } else if (expresion[i] == '+') {
            numero += expresion[i];
            i++;
        }
        
        // Leer dígitos
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
        
        // Clasificar el número
        if (esReal) {
            cout << "Posición " << inicio << ": Número REAL ";
            cout << (esNegativo ? "NEGATIVO" : "POSITIVO") << " -> " << numero << endl;
        } else {
            cout << "Posición " << inicio << ": Número ENTERO ";
            cout << (esNegativo ? "NEGATIVO" : "POSITIVO") << " -> " << numero << endl;
        }
    }
    
public:
    ValidadorExpresiones(string exp) : expresion(exp) {}
    
    bool validarYAnalizar() {
        stack<pair<char, int>> pila; // Almacena el paréntesis y su posición
        bool numeroAnterior = false;
        
        cout << "\n=== ANÁLISIS DE LA EXPRESIÓN ===" << endl;
        cout << "Expresión: " << expresion << endl << endl;
        
        for (int i = 0; i < expresion.length(); i++) {
            char c = expresion[i];
            
            // Ignorar espacios
            if (c == ' ') continue;
            
            // Detectar paréntesis que abre
            if (c == '(') {
                cout << "Posición " << i << ": PARÉNTESIS QUE ABRE '('" << endl;
                pila.push({c, i});
                numeroAnterior = false;
            }
            // Detectar paréntesis que cierra
            else if (c == ')') {
                cout << "Posición " << i << ": PARÉNTESIS QUE CIERRA ')'" << endl;
                if (pila.empty()) {
                    cout << "\n❌ ERROR: Paréntesis que cierra sin correspondiente que abre en posición " << i << endl;
                    return false;
                }
                pila.pop();
                numeroAnterior = false;
            }
            // Detectar operadores
            else if (esOperador(c)) {
                // Si es '-' o '+' al inicio o después de '(' o de otro operador, es parte de un número
                if ((c == '-' || c == '+') && 
                    (i == 0 || expresion[i-1] == '(' || esOperador(expresion[i-1]))) {
                    if (i + 1 < expresion.length() && (esDigito(expresion[i+1]) || expresion[i+1] == '.')) {
                        analizarNumero(i);
                        numeroAnterior = true;
                        continue;
                    }
                }
                cout << "Posición " << i << ": OPERADOR '" << c << "'" << endl;
                numeroAnterior = false;
            }
            // Detectar números
            else if (esDigito(c) || c == '.') {
                analizarNumero(i);
                numeroAnterior = true;
            }
            // Caracter inválido
            else {
                cout << "\n❌ ERROR: Carácter inválido '" << c << "' en posición " << i << endl;
                return false;
            }
        }
        
        // Verificar si quedaron paréntesis sin cerrar
        if (!pila.empty()) {
            auto [parentesis, pos] = pila.top();
            cout << "\n❌ ERROR: Paréntesis que abre sin cerrar en posición " << pos << endl;
            return false;
        }
        
        cout << "\n✅ La expresión es VÁLIDA - Todos los paréntesis están balanceados" << endl;
        return true;
    }
};

int main() {
    string expresion;
    
    cout << "╔════════════════════════════════════════════════════╗" << endl;
    cout << "║   VALIDADOR DE EXPRESIONES MATEMÁTICAS             ║" << endl;
    cout << "╚════════════════════════════════════════════════════╝" << endl;
    cout << "\nIngresa una expresión matemática: ";
    getline(cin, expresion);
    
    ValidadorExpresiones validador(expresion);
    validador.validarYAnalizar();
    
    system("pause");
    return 0;
}