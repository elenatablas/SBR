#include <iostream>


using namespace std;

void 
entradaBH(int numBH)
{
    /*for(int istd=0; i<numBH; i++)
    {

    }*/
    char * objetivo;
}


void 
entradaBC(int numBC)
{
    // for(int i=0; i<numBC; i++)
     
}

enum 
TipoRep { conjuncion, disyuncion };

struct 
ReglaRep
{
    char **antecedentes;
    enum TipoRep tipo;
    char *consecuentes;
    float facCerBC;
};

struct 
HechoRep
{
    char **hecho;
    float facCerBH;
};

typedef enum TipoRep Tipo;
typedef struct ReglaRep Regla;
typedef struct HechoRep Hecho;

void 
main( int argc, char * argv[] )
{

    

}


