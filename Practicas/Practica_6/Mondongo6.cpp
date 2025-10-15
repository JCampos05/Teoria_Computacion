#include <iostream>
using namespace std;

int main(int argc, char const *argv[]) {
    bool beca = false;
    float porcentaje = 0;
    int prom = 0, years = 0, porcBeca = 0;
    string alumno = "";
    cout << "Sistema de becas" << endl;
    cout << "Ingrese el nombre del alumno: " << endl;
    cin >> alumno;
    cout << "Ingrese su promedio sin punto decimal: " << endl;
    cin >> prom;
    cout << "Ingrese los años de estudio: " << endl;
    cin >> years;
    if (prom >= 80) {
        for (int i = 0; i < years; i++) {
            porcentaje += 0.05;
        }
        beca = true;
        porcBeca = porcentaje * 100;
        cout << "El alumno " << alumno << " tiene: " << porcBeca << " % de beca por obtener el promedio de: " << prom << endl;
    } else if ((prom >= 60) && (prom < 80)) {
        for (int i = 0; i < years; i++) {
            porcentaje += 0.02;
        }
        beca = true;
        porcBeca = porcentaje * 100;
        cout << "El alumno " << alumno << " tiene: " << porcBeca << " % de beca por obtener el promedio de: " << prom << endl;
    } else {
        beca = false;
        cout << "El alumno no pudo obtener beca." << endl;
    }
    system("pause");
    return 0;
}