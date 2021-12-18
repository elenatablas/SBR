#include <iostream> // Variables cin y cout
#include <fstream>
#include <iterator>
#include <regex>
#include <string>
#include <set>
#include <functional>

using namespace std;

#define ERROR   -1
#define NOM_FICHERO 1

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
int espacios = 0;

class
    Regla
{
public:
    int numRegla;
    std::string lineaRegla;
    std::set<std::string> antecedentes;
    int numAntecedentes;
    Tipo tipo;
    std::string consecuente;
    float facCerBC;
    bool operator < (const Regla& rhs) const
    {
        return numRegla<rhs.numRegla;
    }
    void imprimirLineaRegla(std::ostream& ostr)
    {
        ostr << "Regla: " << lineaRegla << endl;
    }
    void imprimirRegla(std::ostream& ostr)
    {
        ostr << "--------------------" <<" Entrada Regla (" << numRegla << ") " << "---------------------"  << endl;
        ostr << "Regla (numRegla): "<< numRegla << endl;
        ostr << "Regla (tipo): "<< tipo << endl;
        ostr << "Regla (numAntecedentes): " << numAntecedentes << endl;
        for (auto literal : antecedentes)
            ostr << "Regla (antecedente [" << literal << "])." << endl;
        ostr << "Regla (consecuente): " << consecuente << endl;
        ostr << "Regla (facCerBC): " << facCerBC << endl;
        ostr << "------------------------------------------------------------" << endl;
    }
};

class
    Hecho
{
public:
    std::string nomHecho;
    float facCerBH;
    std::string lineaHecho;
    bool operator < (const Hecho& rhs) const
    {
        return nomHecho<rhs.nomHecho;
    }
    void imprimirLineaHecho(std::ostream& ostr)
    {
        ostr << "Hecho: " << lineaHecho << endl;
    }
    void imprimirHecho(std::ostream& ostr)
    {
        ostr << "--------------------" <<" Entrada Hecho (" << nomHecho << ") " << "---------------------"  << endl;
        ostr << "Hecho (nomHecho): " << nomHecho << endl;
        ostr << "Hecho (facCerBH): " << facCerBH << endl;
        ostr << "------------------------------------------------------------" << endl;
    }
};

std::string entradaBH(std::set<Hecho> &bh, std::ifstream &ficheroBH);
void entradaBC(std::set<Regla> &bc, std::ifstream &ficheroBC);
void motorInferencias(std::string objetivo, std::set<Regla> bc, std::set<Hecho> &bh, std::ostream& ostr);

int
main(int argc, char *argv[])
{
    if(argc < 3)
    {
        cerr << "USO: "<< argv[0] << " [BC.txt] [BH.txt]."<< endl;
        exit(ERROR);
    }

    std::set<Regla> bc;
    std::set<Hecho> bh;
    std::ifstream ficheroBC, ficheroBH;
    std::string fichBC = string(argv[1]);
    std::string fichBH = string(argv[2]);
    ficheroBC.open(fichBC, ifstream::in);
    ficheroBH.open(fichBH, ifstream::in);
    if (!ficheroBC.is_open() || !ficheroBH.is_open())
    {
        if(!ficheroBC.is_open())
            cerr << "ERROR: fichero de entrada '"  << fichBC << "' inexistente." << endl;
        if(!ficheroBH.is_open())
            cerr << "ERROR: fichero de entrada '"  << fichBH << "' inexistente." << endl;
        exit(ERROR);
    }
    std::ofstream ficheroSalida;
    const std::regex nom_fichero("([^\\s]*).txt");
    if (!std::regex_match(fichBC, nom_fichero) || !std::regex_match(fichBH, nom_fichero))
    {
        if(!std::regex_match(fichBC, nom_fichero))
            cerr << "ERROR: fichero de entrada '"  << fichBC << "' extension incorrecta." << endl;
        if(!std::regex_match(fichBH, nom_fichero))
            cerr << "ERROR: fichero de entrada '"  << fichBH << "' extension incorrecta." << endl;
        exit(ERROR);
    }
    std::sregex_token_iterator nom_fich(fichBC.begin(), fichBC.end(), nom_fichero, NOM_FICHERO);
    ficheroSalida.open(nom_fich->str()+fichBH, ofstream::out);
    ficheroSalida << "Fichero BC: '" << fichBC << "'." << endl;
    ficheroSalida << "Fichero BH: '" << fichBH << "'." << endl;

    entradaBC(bc, ficheroBC);
    std::string objetivo = entradaBH(bh, ficheroBH);

    motorInferencias(objetivo, bc, bh, ficheroSalida);
    //motorInferencias(objetivo, bc, bh, cout);

    ficheroBC.close();
    ficheroBH.close();
    ficheroSalida.close();
    return 0;
}

void
entradaBC(std::set<Regla> &bc, std::ifstream &ficheroBC)
{
    const std::regex regla_linea("(R((\\d+)\\s*:\\s*Si\\s+))(\\w+((\\s*y\\s*\\w+)+|(\\s*o\\s*\\w+)+)?)(\\s+Entonces\\s+)(\\w+)(,\\s+FC\\s*=\\s*)(-?\\d+(\\.\\d+)?)", std::regex::icase);
    const std::regex regla_no_literal("\\w+((\\s*y\\s*\\w+)+|(\\s*o\\s*\\w+)+)");
    const std::regex regla_conjuncion("\\w+(\\s*y\\s*\\w+)+");
    const std::regex regex_ant("\\w+");


    int numBC;
    std::string linea;
    ficheroBC >> numBC; // Número de casos
    ficheroBC.get();    // por el retorno de carrol
    for (int i = 0; i < numBC && !ficheroBC.eof(); i++)
    {
        Regla regla;
        getline(ficheroBC, linea);
        regla.lineaRegla = linea;
        if (!std::regex_match(linea, regla_linea))
        {
            cerr << "ERROR: regla: '"  << linea << "' formato incorrecto." << endl;
            cerr << "FORMATO: '[NUM_REGLA]: Si [ANTECEDENTES] Entonces [CONSECUENTE], FC=[FACTOR_CERTEZA]'." << endl;

            exit(ERROR);
        }
        std::sregex_token_iterator num_regla(linea.begin(), linea.end(), regla_linea, NUM_REGLA);
        std::sregex_token_iterator antecedentes_regla(linea.begin(), linea.end(), regla_linea, ANTECEDENTES_REGLA);
        std::sregex_token_iterator consecuente_regla(linea.begin(), linea.end(), regla_linea, CONSECUENTE_REGLA);
        std::sregex_token_iterator factor_certeza_regla(linea.begin(), linea.end(), regla_linea, FACTOR_CERTEZA_REGLA);

        regla.numRegla = stoi(num_regla->str());

        std::string antecedentes = antecedentes_regla->str();

        std::smatch literal;
        if (!std::regex_search(antecedentes, literal, regla_no_literal))
        {
            regla.antecedentes.insert(antecedentes);
            regla.tipo = Tipo::literal;
            regla.numAntecedentes = 1;
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
                    numAntecedentes++;
                }
            }
            regla.numAntecedentes = numAntecedentes;
        }
        regla.consecuente = consecuente_regla->str();
        regla.facCerBC = stof(factor_certeza_regla->str());
        if (regla.facCerBC < -1 || regla.facCerBC > 1)
        {
            cerr << "ERROR: factor de certeza '" << regla.facCerBC << "' incorrecto."<<endl;
            cerr << "[FACTOR_CERTEZA] es un numero entre -1 y 1." << endl;
            exit(ERROR);
        }
        bc.insert(regla);
        //regla.imprimirLineaRegla(std::cout);
        //regla.imprimirRegla(std::cout);
    }
}

std::string
entradaBH(std::set<Hecho> &bh, std::ifstream &ficheroBH)
{
    const std::regex hecho_linea("([^\\s]*)(,\\s+FC\\s*=\\s*)(-?\\d+(\\.\\d+)?)");
    Hecho hecho;
    int numBH;
    std::string linea;
    ficheroBH >> numBH; // Número de casos

    ficheroBH.get();    // por el retorno de carrol
    for (int i = 0; i < numBH && !ficheroBH.eof(); i++)
    {
        getline(ficheroBH, linea);
        hecho.lineaHecho = linea;
        if (!std::regex_match(linea, hecho_linea))
        {
            cerr << "ERROR: hecho: '"  << linea << "' formato incorrecto." << endl;
            cerr << "FORMATO: '[HECHO], FC=[FACTOR_CERTEZA]'." << endl;
            exit(ERROR);
        }

        std::sregex_token_iterator nombre_hecho(linea.begin(), linea.end(), hecho_linea, NOM_HECHO);
        hecho.nomHecho = nombre_hecho->str();

        std::sregex_token_iterator factor_certeza_hecho(linea.begin(), linea.end(), hecho_linea, FACTOR_CERTEZA_HECHO);
        hecho.facCerBH = std::stof(factor_certeza_hecho->str());
        if (hecho.facCerBH < -1 || hecho.facCerBH > 1)
        {
            cerr << "ERROR: factor de certeza incorrecto" << hecho.facCerBH << endl;
            cerr << "[FACTOR_CERTEZA] es un numero entre -1 y 1." << endl;
            exit(ERROR);
        }
        bh.insert(hecho);
        //hecho.imprimirLineaHecho(std::cout);
        //hecho.imprimirHecho(std::cout);
    }

    std::string objetivo;
    getline(ficheroBH, objetivo);
    getline(ficheroBH, objetivo);

    return objetivo;
}

void
imprimirEspacio(std::ostream& ostr)
{
    for(int i=0; i<espacios*2; i++)
    {
        ostr << " ";
    }
}

//Encadenamiento hacia atrás o Dirigido por Metas
//   Determina si se verifica una cierta meta con los hechos disponibles.
//    La inferencia progresa en la red de derecha a izquierda
bool verificar(std::string objetivo, std::set<Regla> bc, std::set<Hecho> &bh, std::ostream& ostr);

void
motorInferencias(std::string objetivo, std::set<Regla> bc, std::set<Hecho> &bh, std::ostream& ostr)
{
    ostr << "Objetivo: " << objetivo << endl;
    ostr << "Proceso de Inferencia: " << endl;
    if(verificar(objetivo, bc, bh, ostr))
    {
        ostr << "Return TRUE" << endl;
    }else
    {
        ostr << "Return FALSE" << endl;
    }
}

bool
contenida(std::string meta, std::set<Hecho> bh, std::ostream& ostr)
{
    std::string bhString;
    imprimirEspacio(ostr);
    ostr << "Verificar("<< meta<<",{";
    for(auto h: bh)
        bhString+= h.nomHecho + ",";
    for(auto hecho : bh)
    {
        if(!meta.compare(hecho.nomHecho))
        {

            ostr<< bhString << "}) ---> true // Recursion: " << meta << " en BH" <<endl;
            imprimirEspacio(ostr);
            ostr << "BH={" << bhString << "}" << endl;
            return true;
        }
    }
    ostr<< bhString << "}) // Recursion " << endl;
    return false;
}

void
equiparar(std::set<Regla> &conjuntoConflicto, std::set<Regla> bc, std::string objetivo)
{
    for(auto regla : bc)
        if(!objetivo.compare(regla.consecuente))
        {
            conjuntoConflicto.insert(regla);
        }
}

bool
noVacio(std::set<Regla> conjuntoConflicto, std::string objetivo, std::ostream& ostr)
{
    if(!conjuntoConflicto.empty())
    {
        imprimirEspacio(ostr);
        ostr << "Conjunto Conflicto={";
        for(auto regla : conjuntoConflicto)
        {
            ostr << "R"<< regla.numRegla << ",";
        }
        ostr << "} // " << objetivo << " es consecuente de estas reglas" << endl;
        return true;
    }
    return false;
}

Regla
resolver(std::set<Regla> &conjuntoConflicto, std::ostream& ostr)
{
    set<Regla>::iterator it;
    it = conjuntoConflicto.begin();
    Regla resolver = *it;
    conjuntoConflicto.erase(it);
    imprimirEspacio(ostr);
    ostr << "R={R"<< resolver.numRegla << "} // Seleccionar regla R"<< resolver.numRegla << endl;
    imprimirEspacio(ostr);
    ostr << "Eliminar R"<< resolver.numRegla << " ---> Conjunto Conflicto={";
    for(auto regla : conjuntoConflicto)
    {
        ostr << "R"<< regla.numRegla << ",";
    }
    ostr << "}" << endl;
    return resolver;
}

std::set<std::string>
extraerAntecedentes(Regla regla, std::ostream& ostr)
{
    imprimirEspacio(ostr);
    ostr << "NuevasMetas={";
    for(auto literal : regla.antecedentes)
    {
        ostr << literal << ",";
    }
    ostr << "} // Antecedentes de R"<< regla.numRegla << "; Verificado = true" << endl;
    return regla.antecedentes;
}

std::string
seleccionarMeta(std::set<string> &nuevasMetas, std::ostream& ostr)
{
    std::set<string>::iterator it;
    it = nuevasMetas.begin();
    std::string nMeta = *it;
    imprimirEspacio(ostr);
    ostr << "Meta="<< nMeta << " // Seleccionar "<< nMeta << " de NuevasMetas" << endl;

    nuevasMetas.erase(it);
    imprimirEspacio(ostr);
    ostr << "NuevasMetas={";
    for(auto meta : nuevasMetas)
    {
        ostr << meta << ",";
    }
    ostr << "} // Eliminar "<< nMeta << " de NuevasMetas" << endl;
    return nMeta;
}

void
addObjetivo(std::string objetivo, std::set<Hecho> &bh, std::ostream& ostr)
{
    Hecho hecho;
    float facCerBH = 0.9; //TODO: Calcular factor de certeza
    if (facCerBH < -1 || facCerBH > 1)
    {
        cerr << "ERROR: factor de certeza incorrecto" << facCerBH << " del hecho calculado " << objetivo << endl;
        cerr << "[FACTOR_CERTEZA] es un numero entre -1 y 1." << endl;
        exit(ERROR);
    }

    hecho.lineaHecho = objetivo+", FC="+std::to_string(facCerBH);
    hecho.nomHecho = objetivo;
    hecho.facCerBH =  facCerBH;
    bh.insert(hecho);
    imprimirEspacio(ostr);
    ostr << "BH={";
    for(auto hecho : bh)
    {
        ostr << hecho.nomHecho << ",";
    }
    ostr << "} // Insertar "<< objetivo << " a la Base de Hechos" << endl;
}

void imprimirMensajeVerificar(std::ostream& ostr)
{
    espacios++;
    imprimirEspacio(ostr);
    ostr << "###########################" << endl;
    imprimirEspacio(ostr);
    ostr << "# Llamada ("<< espacios <<") a verificar #" << endl;
    imprimirEspacio(ostr);
    ostr << "###########################" << endl;
}
bool
verificar(std::string meta, std::set<Regla> bc, std::set<Hecho> &bh, std::ostream& ostr )
{

    bool verificado = false;
    std::set<Regla> conjuntoConflicto;
    std::set<string> nuevasMetas;
    std::string nMet;

    imprimirMensajeVerificar(ostr);

    if (contenida(meta, bh, ostr))
        return true;
    else
    {
        equiparar(conjuntoConflicto, bc, meta);
        while (noVacio(conjuntoConflicto, meta, ostr) && !verificado)
        {
            Regla r = resolver(conjuntoConflicto, ostr);
            nuevasMetas = extraerAntecedentes(r, ostr);
            verificado = true;
            while (!nuevasMetas.empty() && verificado)
            {
                nMet = seleccionarMeta(nuevasMetas, ostr);
                verificado = verificar(nMet, bc, bh, ostr);
            }
            if (verificado)
            {
                addObjetivo(meta, bh, ostr);
            }
        }
    }
    return (verificado);
}

