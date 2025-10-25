#include <aputs_agets>

bool beca = false;
float porcentaje = 0;
int prom = 0, years = 0, porcBeca = 0;
string alumno;

write("Sistema de becas");
write("Ingrese el nombre del alumno: ");
input(alumno);
write("Ingrese su promedio sin punto decimal: ");
input(prom);
write("Ingrese los años de estudio: ");
input(years);

if(prom >= 80){
    for(int i = 0; i < years ; i++){
        float porcentaje += 0.05;
    }
    bool beca = true;
    int porcBeca = porcentaje * 100;
    write("El alumno " , alumno , " tiene: " , porcBeca , " % de beca por obtener el promedio de: " , prom);
} else if ((prom >= 60) && (prom < 80)){
    for(i = 0; i < years ; i++){
        float porcentaje += 0.02;
    }
    bool beca = true;
    int porcBeca = porcentaje * 100;
    write("El alumno " , alumno , " tiene: " , porcBeca , " % de beca por obtener el promedio de: " , prom);
} else {
    bool beca = false;
    write("El alumno no pudo obtener beca.");
}
system("pause");
return 0;