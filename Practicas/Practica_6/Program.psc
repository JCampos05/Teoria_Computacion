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
    for(i = 0; i < years ; i++){
        porcentaje += 0.05;
    }
    beca = true;
    porcBeca = porcentaje * 100;
    write("El alumno " , nombre , " tiene: " , porcBeca , " % de beca por obtener el promedio de: " , prom);
} else if ((prom >= 6) && (prom < 8)){
    for(i = 0; i < years ; i++){
        porcentaje += 0.02;
    }
    beca = true;
    porcBeca = porcentaje * 100;
    write("El alumno " , nombre , " tiene: " , porcBeca , " % de beca por obtener el promedio de: " , prom);
} else {
    beca = false;
    write("El alumno no pudo obtener beca.");
}
system("pause");
return 0;