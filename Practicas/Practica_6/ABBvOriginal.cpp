#include <iostream>
#include <limits>
#include <clocale>
#include <iomanip>
#include <stdlib.h>
#include <conio.h>
#include <string>
using namespace std;

struct nodo 
{
    string info;
    nodo *der;
    nodo *izq;
    nodo *padre;
    int posicion = 0;
};

void menu();
nodo *crear_nodo(string, nodo *);
void ins(nodo *&arbol, string, nodo *);
bool busqueda(nodo *arbol, string);
void preord(nodo *arbol);
nodo *min_izq(nodo *arbol);
void replace (nodo* arbol, nodo* n_nodo);
void destr_nodo(nodo* nodo);
void del_nodo(nodo *nodo_del);
bool sucesor(nodo *arbol);
void del(nodo *&arbol, string );

nodo *arbol_g = NULL;
nodo *aux = NULL;
int cuenta = 10, acum = 0, acum2 = 0;

nodo *crear_nodo(string dato, nodo *padre)
{
    nodo *nuevo_nodo = new nodo();
    nuevo_nodo -> info = dato;
    nuevo_nodo -> der = NULL;    
    nuevo_nodo -> izq = NULL;
    nuevo_nodo -> padre = padre;
    acum ++;
    nuevo_nodo -> posicion = acum2;
    return nuevo_nodo;
}

void error()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    system("cls");
    cout<<endl;
    cout<<"Ingrese un dato válido"<<endl;
    system("pause");
    system("cls");
}

void ins(nodo *&arbol, string dato, nodo *padre)
{
    if (arbol ==  NULL) 
    {
        nodo *nuevo_nodo =  crear_nodo(dato, padre);
        arbol = nuevo_nodo;
    } else 
    {
        string val_raiz = arbol -> info;
        if (dato < val_raiz) 
        {
            ins(arbol -> izq, dato, arbol);
        } else 
        {
            ins(arbol -> der , dato, arbol);
        }
    }
}

void pre_ins()
{
    string dato;
    int opc, seguir = 1;
    do 
    {
        acum2 ++;
        if (acum == 12) 
        {
            cout<<"Árbol lleno. Acción inválida."<<endl;
            break;
            return;
        }
        system("cls");
        do 
        {
            cout<<"Inserte el dato a agregar al árbol.",cin>> dato,cout<<endl;
            if (cin.fail()) 
            {
                error();
            }   
            ins(arbol_g,dato, NULL);
        } 
        while (cin.fail());
        //acum ++;
        cout<<"¿Desea ingresar otro dato al árbol? 1)Sí 2)No "<<endl;cin>>seguir;cout<<endl;
        if (cin.fail())
        {
            error();
        }
        
    } while (seguir == 1);
}
bool busqueda(nodo *arbol, string dato)
{
    if (arbol == NULL)
    {
        return false;
    } else if(arbol -> info == dato) 
    {
        aux = arbol;
        return true;
    } else if ( dato < arbol ->info)
    {
        return busqueda(arbol -> izq, dato);
    } else 
    {
        return busqueda(arbol -> der, dato);
    }
}

void pre_busq()
{
    string dato;
    int seguir = 1;
    do {
        system("cls");
        if (arbol_g == NULL) 
        {
            cout<<"Árbol vacío."<<endl;
            return;
        }
        do 
        {
            cout<<"Inserte el dato a buscar al árbol.",cin>> dato,cout<<endl;
            if (cin.fail()) 
            {
                error();
            }   
            if (busqueda(arbol_g, dato) == true)
            {
                cout<<"Elemento "<< aux -> info <<" se encontró en la posición "<< aux -> posicion<<"."<<endl;
            } else 
            {
                cout<<"Elemento NO encontrado"<<endl;
            }
        } while (cin.fail());
        cout<<"¿Desea ingresar otro dato al árbol? 1)Sí 2)No "<<endl;cin>>seguir;cout<<endl;
        if (cin.fail())
        {
            error();
        }
    } while (seguir == 1);
}

void preord(nodo *arbol)
{
    if (arbol ==  NULL) 
    {
        //cout<<"El árbol esta vacío."<<endl;
        return; 
    } else 
    {
        cout<< arbol -> info<<endl;
        preord(arbol -> izq);
        preord(arbol -> der);
    }
}

void inord(nodo *arbol)
{
    if (arbol ==  NULL) 
    {
        //cout<<"El árbol esta vacío."<<endl;
        return; 
    } else 
    {
        inord(arbol -> izq);
        cout<< arbol -> info <<endl;
        inord(arbol -> der);
    }
}

void postord(nodo *arbol)
{
    if (arbol ==  NULL) 
    {
        //cout<<"El árbol esta vacío."<<endl;
        return; 
    } else 
    {
        postord(arbol -> izq);
        postord(arbol -> der);
        cout<< arbol -> info <<endl;
    }
}
//--ELIMINACIONES--//
nodo *min_izq(nodo *arbol) 
{
    if (arbol == NULL) 
    {
        return NULL;
    }
    if (arbol->izq) 
    {
        return min_izq(arbol->izq);
    }
    else 
    {
        return arbol;
    }
}
// Reemplazar
void replace (nodo* arbol, nodo* n_nodo) 
{
    if (arbol -> padre) 
    {
        if (arbol == arbol -> padre -> izq) 
        {
            arbol -> padre -> izq = n_nodo;
        } else 
        {
            arbol -> padre -> der = n_nodo;
        }
    }
    if (n_nodo) 
    {
        n_nodo -> padre = arbol -> padre;
    }
}
// Destruir nodo
void destr_nodo(nodo* nodo) 
{
    nodo-> izq = NULL;
    nodo-> der = NULL;
    delete nodo;
}
// Eliminar la conexión total del nodo
void del_nodo(nodo *nodo_del) 
{
    if (nodo_del -> izq && nodo_del -> der) 
    { // Nodo con dos hijos
        nodo *menor = min_izq(nodo_del -> der);
        nodo_del -> info = menor -> info;
        del_nodo(menor);
    }
    else if (nodo_del->izq) 
    {  // hijo izquierdo
        replace(nodo_del, nodo_del -> izq);
        destr_nodo(nodo_del);
    } else if (nodo_del->der) 
    {  // hijo derecho
        replace(nodo_del, nodo_del -> der);
        destr_nodo(nodo_del);
    } else 
    {  // Nodo hoja
        replace(nodo_del, NULL);
        destr_nodo(nodo_del);
    }
}
// nodo actual tiene  sucesor in-order
bool sucesor(nodo *arbol) 
{
    if (arbol == NULL) return false;
    return arbol->der != NULL;
}

void del(nodo *&arbol, string dato) 
{
    if (arbol == NULL) 
    {
        cout << "Arbol vacío." << endl;
        return;
    } else if (dato < arbol -> info) 
    {
        del(arbol -> izq, dato);
    } else if (dato > arbol -> info) 
    {
        del(arbol -> der, dato);
    } else 
    { // nodo encontrado
        // Caso de uso.- Si el nodo es la raíz y es el único nodo en el árbol <<-------------------- CHECAR
        if (arbol == arbol_g && arbol -> izq == NULL && arbol -> der == NULL) 
        {
            delete arbol;
            arbol = NULL;       
            arbol_g = NULL;    
            cout << "Árbol vacío." << endl;
            return;
        }
        // verifica si hay intento de eliminar la raíz y si no tiene sucesor in-order
        if (arbol == arbol_g && !sucesor(arbol)) 
        {
            nodo *menor = min_izq(arbol_g -> izq);
            arbol -> info = menor -> info;
            del_nodo(menor);
            //cout << "Acción inválida.No se puede eliminar la raíz." << endl;
            return;
        }
        if (arbol == arbol_g) 
        {
            if (arbol -> izq && arbol -> der) 
            {  // dos hijos
                nodo *menor = min_izq(arbol -> der);
                arbol -> info = menor -> info;
                del_nodo(menor);
            } else if (arbol -> izq) 
            { // hijo izq
                replace(arbol, arbol -> izq);  
                arbol_g = arbol -> izq;     
                destr_nodo(arbol);
            } else if (arbol -> der) 
            { // hijo der
                replace(arbol, arbol -> der);  
                arbol_g = arbol -> der;      
                destr_nodo(arbol);
            }
        } else 
        {
            del_nodo(arbol);  // Para nodos no raíz se elimina con normalidad
        }
    }
}

void eliminaciones()
{
    string dato;
    int seguir = 1;
    system("cls");
    if (arbol_g == NULL) 
    {
        cout<<"Árbol vacío."<<endl;
        return;
    }
    do 
    {
        cout<<"Inserte el dato a eliminar del árbol.",cin>> dato,cout<<endl;
        if (cin.fail()) 
        {
            error();
        }   
        if (!busqueda(arbol_g, dato)) 
        {
            cout << "El elemento no existe en el arbol."<<endl;
            //dato.clear();
        }
        del(arbol_g, dato);
    } while (cin.fail());
    acum --;
    acum2 --;
}

void impresion(const string& prefix, const nodo* arbol, bool isLeft) 
{   if( arbol != NULL ) 
    {
        cout<<prefix;
        cout<<(isLeft ? "├──" : "└──" );
        cout<<arbol -> info<<endl;
        impresion( prefix + (isLeft ? "│   " : "    "), arbol -> izq, true);
        impresion( prefix + (isLeft ? "│   " : "    "), arbol -> der, false);
    }
}

void impresion(const nodo* nodo) 
{
    impresion("", nodo, false);    
}

void menu()
{
    int seguir,opc = 0;
    system("cls");
    do 
    {
        system("cls");
        do 
        {
            cout << acum <<endl;
            cout<<"_______________________"<<endl;
            cout<<"Programa Árbol Binario"<<endl;
            cout<<"_______________________"<<endl;
            cout<<"Escoja una opción: "<<endl;
            cout<<endl;
            cout<<"_____________________________________________"<<endl;
            cout<<"1) Insertar nuevo nodo en el árbol."<<endl;
            cout<<"2) Recorrido preorden del árbol"<<endl;
            cout<<"3) Recorrido inorden del árbol."<<endl;
            cout<<"4) Recorrido postorden del árbol."<<endl;
            cout<<"5) Buscar un dato en el árbol."<<endl;
            cout<<"6) Eliminar un nodo del árbol."<<endl;
            cout<<"7) Mostrar árbol"<<endl;
            cout<<"_____________________________________________"<<endl;
            cin>>opc;cout<<endl;
            if (cin.fail())
            {
                error();
            }
        switch (opc) 
            {
                case 1:
                    pre_ins();
                break;
            
                case 2:
                    preord(arbol_g);
                break;

                case 3:
                    inord(arbol_g);
                break;

                case 4:
                    postord(arbol_g);
                break;

                case 5:
                    pre_busq();
                break;

                case 6:
                    eliminaciones();
                break;

                case 7:
                    impresion("",arbol_g,true);
                break;

                default:
                    break;
            }
        } while (opc <= 0 || opc > 9);
        do 
        {
            cout<<"¿Desea realizar alguna otra busqueda y/o consulta de otro dato? 1)Sí 2)No "<<endl,cin>>seguir,cout<<endl;
            if (cin.fail())
            {
                error();
            }
        } while (cin.fail()|| seguir!=1&&seguir!=2);
    } while (seguir == 1);
}

int main(int argc, char const *argv[])
{
    //data();
    setlocale(LC_ALL, "es_ES.UTF-8");
	cout<<"Bienvenido..."<<endl;
    menu();
	cout<<endl;
    system("pause");
    return 0;
}