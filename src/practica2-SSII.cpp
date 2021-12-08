#include <iostream> // Variables cin y cout
#include <set>
#include <regex>
using namespace std;

enum 
TipoRep { conjuncion, disyuncion };

struct 
ReglaRep
{
    int numRegla;
    std::set<std::string> antecedentes;
    enum TipoRep tipo;
    std::string consecuentes;
    float facCerBC;
};

struct 
HechoRep
{
    std::set<std::string> hecho;
    float facCerBH;
};

typedef enum TipoRep Tipo;
typedef struct ReglaRep Regla;
typedef struct HechoRep Hecho;

//regexHecho = ()

std::string
entradaBH()
{
    //std::regex regexHecho ("([^,]+)");
    //std::regex regexFactor ("(\d+(\.\d+)?)$");
    std::regex regexLinea("[^,]+)(,\s+FC\s*=\s*)(\d+(\.\d+)?$");
    Hecho hecho;
    std::set<Hecho> bh;
    unsigned numBH;
    cin >> numBH; // Número de casos
    
    cin.get(); //por el retorno de carrol
    for(int i=0; i<numBH; i++)
    {
        std::string linea = "conduce>3, FC=1";
        //getline(cin, linea);
        std::smatch match;
        if (std::regex_search(linea, match, regexLinea)) {
            std::cout << match.str(1) << std::endl;
            //hecho.hecho.insert(match.str(1));
            //hecho.facCerBH = 2;
           // bh.insert(hecho);
        }
    }
    std::string objetivo = "Hola";
    //getline(cin, objetivo);
    cin >> objetivo;
    return objetivo;
}
/*

void 
entradaBC()
{
    Regla regla;
    std::set<Regla> bc;
    unsigned numBC;
    unsigned factor;
    cin >> numBC; // Número de casos
    
    cin.get(); //por el retorno de carrol
    for(int i=0; i<numBC; i++)
    {
        std::string linea;
        getline(cin, linea);
        cout <<  strtok(char*(linea), ":") << endl;  
        cout <<  strtok(char*(linea), "Entonces") << endl;  
        cout <<  strtok(char*(linea), ",") << endl;  

        strtok(NULL, "=");
        cout <<  stof(strtok(NULL, "=")) << endl;    

        if(factor < -1 || factor > 1){
            cerr << "ERROR" << factor << endl;
            exit(0);
        }
        regla.facCerBC = factor;
        bc.insert(regla);
        
    } 
}

*/

int 
main( int argc, char * argv[] )
{

    entradaBH();
    return 0;
}

/* Encadenamiento hacia atrás o Dirigido por Metas
    Determina si se verifica una cierta meta con los hechos disponibles.
    La inferencia progresa en la red de derecha a izquierda
    bool 
    motchar * orInferencias(char *meta, char **BH, char **BC)
    {
        //BH = entradaBH;
        //return Verificar(meta, BH, BC);
    }

    bool
    contenida(char *meta, char **BH)
    {
        return true;
    }bool 
    motorInferencias(char *meta, char **BH, char **BC)

    char **
    consecuentes(char **BC)
    {

    }

    char ** 
    equiparar(char **consecuentes, char *meta)
    {

    }

    char **
    consecuentes(char **BC)
    {

    }

    bool
    noVacio(char **conjuntoVacio)
    {

    }

    char *
    resolver(char **conjuntoConflicto)
    {

    }

    void
    eliminar(char *r, char **conjuntoConflicto)
    {

    }

    char **
    extraerAntecedentes(char **r)
    {

    }

    char *
    seleccionarMeta(char **nuevasMetas)
    {


    }

    void
    añadir(char *meta, char **BH)
    {

    }

    bool 
    verificar(char *meta, char **BH, char **BC)
    {
        
        bool verificado = false;
        char **conjuntoConflicto = NULL;
        char **conjuntoVacio = NULL;
        char *r = NULL;
        char **nuevasMetas = NULL;
        char * nMet = NULL;

        if(contenida(meta, BH))
        {
            return true;

        }
        conjuntoConflicto= equiparar(consecuentes(BC), meta);
        while(noVacio(conjuntoVacio))
        {
            r=resolver(conjuntoConflicto);
            eliminar(r,conjuntoConflicto);
            nuevasMetas= extraerAntecedentes(r);
            verificado=true;
            while(noVacio(nuevasMetas) && verificado)
            {
                nMet=seleccionarMeta(nuevasMetas);
                eliminar(nMet, nuevasMetas);
                verificado=verificar(nMet,BH);
            }
            if (verificado)
            {
                añadir(meta, BH);
            }
        }
        return(verificado);
    }
*/


