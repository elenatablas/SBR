#include <iostream> // Variables cin y cout
#include <fstream>
#include <iterator>
#include <regex>
#include <string>
#include <set>
#include <functional>
#include<algorithm>

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

class
    Regla
{
public:
    int numRegla;
    std::string lineaRegla;
    std::string lineaAntecedentes;
    std::set<std::string> antecedentes;
    int numAntecedentes;
    Tipo tipo;
    float facCertezaAnt;
    std::string consecuente;
    float facCertezaCons;
    float facCerBC;
    bool operator < (const Regla& rhs) const
    {
        return numRegla<rhs.numRegla;
    }
    void setFacCertezaCons(float facCer)
    {
        facCertezaCons = facCer;
    }
    void imprimirLineaRegla(std::ostream& ostr)
    {
        ostr << "Regla: " << lineaRegla << endl;
    }
    void imprimirRegla(std::ostream& ostr)
    {
        ostr << "--------------------" <<" Regla (" << numRegla << ") " << "---------------------"  << endl;
        ostr << "Regla (numRegla): "<< numRegla << endl;
        ostr << "Regla (tipo): "<< tipo << endl;
        ostr << "Regla (lineaAntecedentes): " << lineaAntecedentes << endl;
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
        ostr << "--------------------" <<" Hecho (" << nomHecho << ") " << "---------------------"  << endl;
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
    ficheroSalida << "-------------------" <<" Ficheros de entrada " << "--------------------"  << endl;
    ficheroSalida << "Fichero BC: '" << fichBC << "'." << endl;
    ficheroSalida << "Fichero BH: '" << fichBH << "'." << endl;
    ficheroSalida << "------------------------------------------------------------" << endl;

    entradaBC(bc, ficheroBC);
    std::string objetivo = entradaBH(bh, ficheroBH);

    motorInferencias(objetivo, bc, bh, ficheroSalida);
    //Para mostrar la salida por la terminal en vez de por el fichero
    //motorInferencias(objetivo, bc, bh, cout);

    ficheroBC.close();
    ficheroBH.close();
    ficheroSalida.close();
    return 0;
}

void
entradaBC(std::set<Regla> &bc, std::ifstream &ficheroBC)
{
    const std::regex regla_linea("(R((\\d+)\\s*:\\s*Si\\s+))([^\\s]+((\\s*y\\s*[^\\s]+)+|(\\s*o\\s*[^\\s]+)+)?)(\\s+Entonces\\s+)(\\w+)(,\\s+FC\\s*=\\s*)(-?\\d+(\\.\\d+)?)", std::regex::icase);
    const std::regex regla_no_literal("[^\\s]+((\\s*y\\s*[^\\s]+)+|(\\s*o\\s*[^\\s]+)+)");
    const std::regex regla_conjuncion("[^\\s]+(\\s*y\\s*[^\\s]+)+");
    const std::regex regex_ant("[^\\s]+");


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
        regla.lineaAntecedentes=antecedentes;
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
    const std::regex hecho_linea("([^\\s]+)(,\\s+FC\\s*=\\s*)(-?\\d+(\\.\\d+)?)");
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
            cerr << "ERROR: factor de certeza incorrecto " << hecho.facCerBH << endl;
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
imprimirEspacio(std::ostream& ostr, int espacios)
{
    for(int i=0; i<espacios*2; i++)
    {
        ostr << " ";
    }
}

//Encadenamiento hacia atrás o Dirigido por Metas
//   Determina si se verifica una cierta meta con los hechos disponibles.
//    La inferencia progresa en la red de derecha a izquierda
bool verificar(std::string objetivo, std::set<Regla> bc, std::set<Hecho> &bh, std::ostream& ostr, int &espacios);

void
motorInferencias(std::string objetivo, std::set<Regla> bc, std::set<Hecho> &bh, std::ostream& ostr)
{
    ostr << "-------------------" <<" Inferencia en un SBR " << "-------------------"  << endl;
    ostr << "(Razonamiento dirigido por Metas)"<< endl;
    ostr << "Objetivo: " << objetivo << endl;
    ostr << "Proceso de Inferencia: " << endl;
    int espacios = 0;
    if(verificar(objetivo, bc, bh, ostr, espacios))
    {
        ostr << "Return TRUE" << endl;
        ostr << "------------------------------------------------------------" << endl;
        for(auto hecho : bh)
            if(!objetivo.compare(hecho.nomHecho))
            {
                hecho.imprimirHecho(ostr);
            }
    }
    else
    {
        ostr << "Return FALSE" << endl;
    }
}

bool
contenida(std::string meta, std::set<Hecho> bh, std::ostream& ostr, int espacios)
{
    std::string bhString;
    imprimirEspacio(ostr, espacios);
    ostr << "Verificar("<< meta<<",{";
    for(auto h: bh)
        bhString+= h.nomHecho + ",";
    for(auto hecho : bh)
    {
        if(!meta.compare(hecho.nomHecho))
        {

            ostr<< bhString << "}) ---> true // Recursion: " << meta << " en BH" <<endl;
            imprimirEspacio(ostr, espacios);
            ostr << "BH={" << bhString << "}" << endl;
            return true;
        }
    }
    ostr<< bhString << "}) // Recursion " << endl;
    return false;
}

void
equiparar(std::set<Regla> &conjuntoConflicto, std::set<Regla> bc, std::string meta)
{
    for(auto regla : bc)
        if(!meta.compare(regla.consecuente))
        {
            conjuntoConflicto.insert(regla);
        }
}

bool
noVacio(std::set<Regla> conjuntoConflicto, std::string objetivo, std::ostream& ostr, int espacios)
{
    if(!conjuntoConflicto.empty())
    {
        imprimirEspacio(ostr, espacios);
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
resolver(std::set<Regla> &conjuntoConflicto, std::ostream& ostr, int espacios)
{
    set<Regla>::iterator it;
    it = conjuntoConflicto.begin();
    Regla resolver = *it;
    conjuntoConflicto.erase(it);
    imprimirEspacio(ostr, espacios);
    ostr << "R={R"<< resolver.numRegla << "} // Seleccionar regla R"<< resolver.numRegla << endl;
    imprimirEspacio(ostr, espacios);
    ostr << "Eliminar R"<< resolver.numRegla << " ---> Conjunto Conflicto={";
    for(auto regla : conjuntoConflicto)
    {
        ostr << "R"<< regla.numRegla << ",";
    }
    ostr << "}" << endl;
    return resolver;
}

std::set<std::string>
extraerAntecedentes(Regla regla, std::ostream& ostr, int espacios)
{
    imprimirEspacio(ostr, espacios);
    ostr << "NuevasMetas={";
    for(auto literal : regla.antecedentes)
    {
        ostr << literal << ",";
    }
    ostr << "} // Antecedentes de R"<< regla.numRegla << "; Verificado = true" << endl;
    return regla.antecedentes;
}

std::string
seleccionarMeta(std::set<string> &nuevasMetas, std::ostream& ostr, int espacios)
{
    std::set<string>::iterator it;
    it = nuevasMetas.begin();
    std::string nMeta = *it;
    imprimirEspacio(ostr, espacios);
    ostr << "Meta="<< nMeta << " // Seleccionar "<< nMeta << " de NuevasMetas" << endl;

    nuevasMetas.erase(it);
    imprimirEspacio(ostr, espacios);
    ostr << "NuevasMetas={";
    for(auto meta : nuevasMetas)
    {
        ostr << meta << ",";
    }
    ostr << "} // Eliminar "<< nMeta << " de NuevasMetas" << endl;
    return nMeta;
}

void
imprimirMensajeVerificar(std::ostream& ostr, int &espacios)
{
    espacios++;
    imprimirEspacio(ostr, espacios);
    ostr << "###########################" << endl;
    imprimirEspacio(ostr, espacios);
    ostr << "# Llamada ("<< espacios <<") a verificar #" << endl;
    imprimirEspacio(ostr, espacios);
    ostr << "###########################" << endl;
}
void
caso1(Regla &regla, std::set<Hecho> bh, std::ostream& ostr, int espacios)
{
    float facCertezaUnion=-2;
    imprimirEspacio(ostr, espacios);
    ostr << "(CASO 1): Combinacion de antecedentes de R" << regla.numRegla << endl;
    imprimirEspacio(ostr, espacios);
    ostr << "   FC(" << regla.lineaAntecedentes << ")=min(";
    if(regla.tipo == Tipo::conjuncion)
    {
        facCertezaUnion=2;
    }
    for(auto ant :regla.antecedentes)
    {
        ostr << "FC(" << ant << "),";
        for(auto hecho: bh)
        {
            if(!ant.compare(hecho.nomHecho))
            {
                if(regla.tipo == Tipo::conjuncion)
                    facCertezaUnion=std::min(facCertezaUnion, hecho.facCerBH);
                if(regla.tipo == Tipo::disyuncion)
                    facCertezaUnion=std::max(facCertezaUnion, hecho.facCerBH);
            }
        }
    }
    ostr << ")=" << facCertezaUnion << endl;
    regla.facCertezaAnt = facCertezaUnion;
}

float
caso2(std::string meta, std::set<Regla> conjuntoResuelto, std::ostream& ostr, int espacios)
{
    set<Regla>::iterator it;
    it = conjuntoResuelto.begin();
    int numReglas= conjuntoResuelto.size();
    Regla regla1 = *it;
    float facCerteza= regla1.facCertezaCons;
    conjuntoResuelto.erase(it);
    for(auto regla2 : conjuntoResuelto)
    {
        imprimirEspacio(ostr, espacios);
        ostr << "(CASO 2): Combinacion de las reglas R" << regla1.numRegla << " y R" << regla2.numRegla << endl;
        if((regla1.facCertezaCons) >= 0 && (regla2.facCertezaCons >= 0))
        {
            facCerteza = regla1.facCertezaCons+regla2.facCertezaCons*(1-regla1.facCertezaCons);
            imprimirEspacio(ostr, espacios);
            ostr << "   FC(" << meta << ")=FC(" << meta << "{R" << regla1.numRegla<<"}) + FC(";
            ostr << meta << "{R" << regla2.numRegla<<"})*(1-FC(" << meta << "{R";
            ostr << regla1.numRegla<<"}))="<< facCerteza << endl;
            regla1.numRegla=numReglas++;
            regla1.facCertezaCons = facCerteza;
        }
        else if((regla1.facCertezaCons) <= 0 && (regla2.facCertezaCons <= 0))
        {
            facCerteza = regla1.facCertezaCons+regla2.facCertezaCons*(1+regla1.facCertezaCons);
            imprimirEspacio(ostr, espacios);
            ostr << "   FC(" << meta << ")=FC(" << meta << "{R" << regla1.numRegla<<"}) + FC(";
            ostr << meta << "{R" << regla2.numRegla<<"})*(1+FC(" << meta << "{R";
            ostr << regla1.numRegla<<"}))="<< facCerteza << endl;
            regla1.numRegla=numReglas++;
            regla1.facCertezaCons = facCerteza;
        }
        else
        {
            facCerteza = (regla1.facCertezaCons+regla2.facCertezaCons)/(1-std::min(std::abs(regla1.facCertezaCons),std::abs(regla2.facCertezaCons)));
            imprimirEspacio(ostr, espacios);
            ostr << "   FC(" << meta << ")=FC(" << meta << "{R" << regla1.numRegla<<"}) + FC(";
            ostr << meta << "{R" << regla2.numRegla<<"})/(1-min{|FC(" << meta << "{R";
            ostr << regla1.numRegla<<"}|), FC(" << meta << "{R" << regla1.numRegla<<"}|})="<< facCerteza << endl;
            regla1.numRegla=numReglas++;
            regla1.facCertezaCons = facCerteza;
        }

    }
    return facCerteza;
}

void
caso3(Regla &regla, std::set<Hecho> bh, std::ostream& ostr, int espacios)
{
    float facCerteza=2;
    float cero=0;
    imprimirEspacio(ostr, espacios);
    ostr << "(CASO 3): Combinacion de la evidencia con la regla R" << regla.numRegla << endl;
    imprimirEspacio(ostr, espacios);
    ostr << "   FC(" << regla.consecuente << "{R" << regla.numRegla<<"})="<<regla.facCerBC<< "*";
    if(regla.numAntecedentes==1)
    {
        for(auto hecho: bh)
        {
            if(!regla.lineaAntecedentes.compare(hecho.nomHecho))
            {
                facCerteza= regla.facCerBC*std::max(cero,hecho.facCerBH);
            }
        }
    }
    else
    {
        facCerteza= regla.facCerBC*std::max(cero,regla.facCertezaAnt);
    }
    ostr << "max(0,FC(" << regla.lineaAntecedentes << "))=" << facCerteza << endl;
    regla.setFacCertezaCons(facCerteza);
}

void
addObjetivo(std::string meta, std::set<Hecho> &bh, std::set<Regla> conjuntoResuelto, std::ostream& ostr, int espacios)
{
    Hecho hecho;
    float facCerBH = caso2(meta, conjuntoResuelto, ostr, espacios);
    if (facCerBH < -1 || facCerBH > 1)
    {
        cerr << "ERROR: factor de certeza incorrecto " << facCerBH << " del hecho calculado " << meta << endl;
        cerr << "[FACTOR_CERTEZA] es un numero entre -1 y 1." << endl;
        exit(ERROR);
    }

    hecho.lineaHecho = meta+", FC="+std::to_string(facCerBH);
    hecho.nomHecho = meta;
    hecho.facCerBH =  facCerBH;
    bh.insert(hecho);
    imprimirEspacio(ostr, espacios);
    ostr << "BH={";
    for(auto hecho : bh)
    {
        ostr << hecho.nomHecho << ",";
    }
    ostr << "} // Insertar "<< meta << " a la Base de Hechos" << endl;
}

bool
verificar(std::string meta, std::set<Regla> bc, std::set<Hecho> &bh, std::ostream& ostr, int &espacios)
{

    bool verificado = false;
    std::set<Regla> conjuntoConflicto;
    std::set<string> nuevasMetas;
    std::string nMet;

    imprimirMensajeVerificar(ostr, espacios);

    if (contenida(meta, bh, ostr, espacios))
        return true;
    else
    {
        equiparar(conjuntoConflicto, bc, meta);
        std::set<Regla> conjuntoResuelto;
        while (noVacio(conjuntoConflicto, meta, ostr, espacios)) // && !verificado)
        {
            Regla r = resolver(conjuntoConflicto, ostr, espacios);
            nuevasMetas = extraerAntecedentes(r, ostr, espacios);
            verificado = true;
            while (!nuevasMetas.empty() && verificado)
            {
                nMet = seleccionarMeta(nuevasMetas, ostr, espacios);
                verificado = verificar(nMet, bc, bh, ostr, espacios);
            }
            if(r.tipo != Tipo::literal)
                caso1(r, bh, ostr, espacios);
            caso3(r, bh, ostr, espacios);
            conjuntoResuelto.insert(r);

        }
        addObjetivo(meta, bh, conjuntoResuelto, ostr, espacios);    // caso 2
    }
    return (verificado);
}
