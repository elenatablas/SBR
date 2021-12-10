#include <iostream> // Variables cin y cout
#include <fstream>
#include <iterator>
#include <regex>
#include <string>
#include <set>
#include <functional>

using namespace std;

#define NUM_REGLA 3
#define ANTECEDENTES_REGLA 4
#define CONSECUENTE_REGLA 9
#define FACTOR_CERTEZA_REGLA 11

#define NOM_HECHO 1
#define FACTOR_CERTEZA_HECHO 3

enum TipoRep
{
    literal,
    conjuncion,
    disyuncion
};
typedef enum TipoRep Tipo;

class
    Regla
{
    public:
        int numRegla;
        std::set<std::string> antecedentes;
        int numAntecedentes;
        enum TipoRep tipo;
        std::string consecuente;
        float facCerBC;
    bool operator < (const Regla& rhs) const {return numRegla<rhs.numRegla;}
};

class
    Hecho
{
    public:
        std::string nomHecho;
        float facCerBH;
    bool operator < (const Hecho& rhs) const {return facCerBH<rhs.facCerBH;}
};



std::string entradaBH(std::set<Hecho> bh, std::ifstream &ficheroBH);
void entradaBC(std::set<Regla> bc, std::ifstream &ficheroBC);
bool motorInferencias(std::string objetivo, std::set<Regla> bc, std::set<Hecho> bh);

int 
main(int argc, char *argv[])
{
    std::set<Regla> bc;
    std::set<Hecho> bh;
    std::ifstream ficheroBC, ficheroBH;
    std::string fichBC = string(argv[1]);
    std::string fichBH = string(argv[2]);
    ficheroBC.open(fichBC, ifstream::in);
    ficheroBH.open(fichBH, ifstream::in);
    if (!ficheroBC.is_open() || !ficheroBH.is_open())
    {
        cerr << "ERROR: fichero de entrada inexistente" << endl;
        exit(0);
    }
    std::ofstream ficheroSalida;
    ficheroSalida.open(fichBC+fichBH, ofstream::out);
    ficheroSalida << "Fichero BC: " << fichBC << endl;
    ficheroSalida << "Fichero BH: " << fichBH << endl;

    //entradaBC(bc, ficheroBC);
    std::string objetivo = entradaBH(bh, ficheroBH);

    ficheroSalida << "Objetivo: " << objetivo << endl;
    ficheroSalida << "Proceso de Inferencia: " << endl;
    ficheroSalida << "Cómo se activa la red: " << endl;
    ficheroSalida << "CASO-i de inferencia que se va aplicando: " << endl;
    ficheroSalida << "Hecho Objetivo con su FC: " << endl;

    //motorInferencias(objetivo, bc, bh);

    ficheroBC.close();
    ficheroBH.close();
    ficheroSalida.close();
    return 0;
}

void 
entradaBC(std::set<Regla> bc, std::ifstream &ficheroBC)
{
    const std::regex regla_regex("(R((\\d+)\\s*:\\s*Si\\s+))(\\w+((\\s*y\\s*\\w+)+|(\\s*o\\s*\\w+)+)?)(\\s+Entonces\\s+)(\\w+)(,\\s+FC\\s*=\\s*)(-?\\d+(\\.\\d+)?)", std::regex::icase);
    const std::regex regla_no_literal("\\w+((\\s*y\\s*\\w+)+|(\\s*o\\s*\\w+)+)");
    const std::regex regla_conjuncion("\\w+(\\s*y\\s*\\w+)+");
    const std::regex regex_ant("\\w+");

    Regla regla;
    unsigned numBC;
    std::string linea;
    ficheroBC >> numBC; // Número de casos
    ficheroBC.get();    // por el retorno de carrol
    for (int i = 0; i < numBC && !ficheroBC.eof(); i++)
    {
        getline(ficheroBC, linea);
        std::sregex_token_iterator num_regla(linea.begin(), linea.end(), regla_regex, NUM_REGLA);
        std::sregex_token_iterator antecedentes_regla(linea.begin(), linea.end(), regla_regex, ANTECEDENTES_REGLA);
        std::sregex_token_iterator consecuente_regla(linea.begin(), linea.end(), regla_regex, CONSECUENTE_REGLA);
        std::sregex_token_iterator factor_certeza_regla(linea.begin(), linea.end(), regla_regex, FACTOR_CERTEZA_REGLA);

        regla.numRegla = stoi(num_regla->str());

        std::string antecedentes = antecedentes_regla->str();

        std::smatch literal;
        if (!std::regex_search(antecedentes, literal, regla_no_literal))
        {
            regla.antecedentes.insert(antecedentes);
            regla.tipo = Tipo::literal;
            regla.numAntecedentes = 1;
            std::cout << "regla.literal " << antecedentes << "\n";
        }
        else
        {
            std::smatch conjuncion;
            int numAntecedentes = 0;
            if (std::regex_search(antecedentes, conjuncion, regla_conjuncion))
            {
                regla.tipo = Tipo::conjuncion;
            }
            else
            {
                regla.tipo = Tipo::disyuncion;
            }
            auto dis_begin = std::sregex_iterator(antecedentes.begin(), antecedentes.end(), regex_ant);
            auto dis_end = std::sregex_iterator();
            for (std::sregex_iterator i = dis_begin; i != dis_end; ++i)
            {
                std::smatch match = *i;
                if (match.str() != "y" && match.str() != "o")
                {
                    regla.antecedentes.insert(match.str());
                    std::cout << "regla.conj/dis " << match.str() << "\n";
                    numAntecedentes++;
                }
            }
            regla.numAntecedentes = numAntecedentes;
        }

        regla.consecuente = consecuente_regla->str();
        regla.facCerBC = stof(factor_certeza_regla->str());
        if (regla.facCerBC < -1 || regla.facCerBC > 1)
        {
            cerr << "ERROR: factor de certeza incorrecto" << regla.facCerBC << endl;
            exit(0);
        }
        std::cout << "regla.numRegla " << regla.facCerBC << "\n";
        bc.insert(regla);
    }
}

std::string
entradaBH(std::set<Hecho> bh, std::ifstream &ficheroBH)
{
    const std::regex hecho_linea("([^,]+)(,\\s+FC\\s*=\\s*)(\\d+(\\.\\d+))?$");

    Hecho hecho;
    unsigned numBH;
    std::string linea;
    ficheroBH >> numBH; // Número de casos
    ficheroBH.get();    // por el retorno de carrol
    for (int i = 0; i < numBH && !ficheroBH.eof(); i++)
    {
        getline(ficheroBH, linea);
        std::sregex_token_iterator nombre_hecho(linea.begin(), linea.end(), hecho_linea, NOM_HECHO);
        std::sregex_token_iterator factor_certeza_hecho(linea.begin(), linea.end(), hecho_linea, FACTOR_CERTEZA_HECHO);
        hecho.nomHecho = nombre_hecho->str();
        hecho.facCerBH = stof(factor_certeza_hecho->str());
        if (hecho.facCerBH < -1 || hecho.facCerBH > 1)
        {
            cerr << "ERROR: factor de certeza incorrecto" << hecho.facCerBH << endl;
            exit(0);
        }
        std::cout << "regla.numRegla " << hecho.facCerBH << "\n";
        bh.insert(hecho);
    }
    std::string objetivo;
    getline(ficheroBH, objetivo);
    getline(ficheroBH, objetivo);
    return objetivo;
}
/*
//Encadenamiento hacia atrás o Dirigido por Metas
//   Determina si se verifica una cierta meta con los hechos disponibles.
//    La inferencia progresa en la red de derecha a izquierda 
bool verificar(std::string objetivo, std::set<Regla> bc, std::set<Hecho> bh);

bool 
motorInferencias(std::string objetivo, std::set<Regla> bc, std::set<Hecho> bh)
{
    bool encontrado = verificar(objetivo, bc, bh);
    return encontrado;
}

bool 
contenida(std::string objetivo, std::set<Hecho> bh)
{
    return true;
}

std::string
consecuentes(std::set<Regla> bc)
{
    return NULL;
}

std::string
equiparar(std::set<Regla> bc, std::string objetivo)
{
    return NULL;
}

std::string
consecuentes(std::set<Regla> bc)
{
    return NULL;
}

bool 
noVacio(std::set<Regla> conjuntoVacio)
{
    return true;
}

std::string
resolver(std::set<Regla> conjuntoConflicto)
{
    return NULL;
}

void 
eliminar(std::string r, std::set<Regla> conjuntoConflicto)
{
}

std::string
extraerAntecedentes(std::set<std::string> r)
{
}

std::string
seleccionarMeta(std::set<Hecho> nuevasMetas)
{
}

void 
añadir(std::string objetivo, std::set<Hecho> bh)
{
}

bool 
verificar(std::string objetivo, std::set<Regla> bc, std::set<Hecho> bh ))
{

    bool verificado = false;
    std::set<Regla> conjuntoConflicto = NULL;
    std::set<Regla> conjuntoVacio = NULL;
    std::stringr = NULL;
    std::set<Hecho>nuevasMetas = NULL;
    std::string nMet = NULL;

    if (contenida(objetivo, bh))
    {
        return true;
    }
    conjuntoConflicto = equiparar(consecuentes(bc), objetivo);
    while (noVacio(conjuntoVacio))
    {
        r = resolver(conjuntoConflicto);
        eliminar(r, conjuntoConflicto);
        nuevasMetas = extraerAntecedentes(r);
        verificado = true;
        while (noVacio(nuevasMetas) && verificado)
        {
            nMet = seleccionarMeta(nuevasMetas);
            eliminar(nMet, nuevasMetas);
            verificado = verificar(nMet, bh);
        }
        if (verificado)
        {
            añadir(objetivo, bh);
        }
    }
    return (verificado);
}
*/