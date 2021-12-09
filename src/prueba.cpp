#include <iostream>
#include <iterator>
#include <string>
#include <regex>
#include <set>

#define NUM_REGLA 3
#define ANTECEDENTES_REGLA 4
#define CONSECUENTE_REGLA 9
#define FACTOR_CERTEZA_REGLA 11
enum 
TipoRep { literal, conjuncion, disyuncion };

struct 
ReglaRep
{
    int numRegla;
    std::set<std::string> antecedentes;
    int numAntecedentes;
    enum TipoRep tipo;
    std::string consecuente;
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

int main()
{
    std::string s = "7"
"R1: Si A o Ped y er Entonces EXP, FC=0.5"
"R2: SI Bea y D Entonces EXP, FC=0.9"
"R3: Si Ce ENTONCES CAN, FC=0.5"
"R4: Si D o D Entonces CAN, FC=1"
"R5: Si EXP y E Entonces CACC, FC=-0.5"
"R6: Si CAN Entonces CACC, FC=0.5"
"R7: Si F y G Entonces CACC, FC=0.7";
 
    Regla regla;
    std::set<Regla> bc;

    const std::regex regla_regex("(R((\\d+)\\s*:\\s*Si\\s+))(\\w+((\\s*y\\s*\\w+)+|(\\s*o\\s*\\w+)+)?)(\\s+Entonces\\s+)(\\w+)(,\\s+FC\\s*=\\s*)(-?\\d+(\\.\\d+)?)", std::regex::icase);
    std::sregex_token_iterator num_regla(s.begin(), s.end(), regla_regex, NUM_REGLA);
    std::sregex_token_iterator antecedentes_regla(s.begin(), s.end(), regla_regex, ANTECEDENTES_REGLA);
    std::sregex_token_iterator consecuente_regla(s.begin(), s.end(), regla_regex, CONSECUENTE_REGLA);
    std::sregex_token_iterator factor_certeza_regla(s.begin(), s.end(), regla_regex, FACTOR_CERTEZA_REGLA);
    regla.numRegla = stoi(num_regla->str());
    
    std::string antecedentes = antecedentes_regla->str();
    const std::regex regla_literal("\\w+((\\s*y\\s*\\w+)+|(\\s*o\\s*\\w+)+)");
    std::smatch literal;
    if(!std::regex_search(antecedentes, literal, regla_literal)){
        regla.antecedentes.insert(antecedentes);
        regla.tipo = Tipo::literal;
        regla.numAntecedentes=1;
        std::cout << "regla.literal " << antecedentes << "\n";
    }
    else
    {
        const std::regex regla_conjuncion("\\w+(\\s*y\\s*\\w+)+");
        std::smatch conjuncion;
        int numAntecedentes = 0;
        if(std::regex_search(antecedentes, conjuncion, regla_conjuncion)){
            regla.tipo = Tipo::conjuncion;  
        }
        else{
            regla.tipo = Tipo::disyuncion;
        }
        const std::regex regex_ant("\\w+");
        auto dis_begin = std::sregex_iterator(antecedentes.begin(), antecedentes.end(), regex_ant);
        auto dis_end = std::sregex_iterator();
        for (std::sregex_iterator i = dis_begin; i != dis_end; ++i) {
            std::smatch match = *i; 
            if(match.str() != "y" && match.str() != "o") {                                               
                regla.antecedentes.insert(match.str());
                std::cout << "regla.conj/dis " << match.str() << "\n";
                numAntecedentes++;
            }
        }
        regla.numAntecedentes = numAntecedentes;
    }

    regla.consecuente = consecuente_regla->str();
    regla.facCerBC = stof(factor_certeza_regla->str());
    std::cout << "regla.numRegla " << regla.facCerBC << "\n";
    bc.insert(regla);


    //std::sregex_token_iterator(s.begin(), s.end(), regla_regex, NUM_REGLA);
    std::copy( std::sregex_token_iterator(s.begin(), s.end(), regla_regex, NUM_REGLA),
               std::sregex_token_iterator(),
               std::ostream_iterator<std::string>(std::cout, "\n"));
    std::copy( std::sregex_token_iterator(s.begin(), s.end(), regla_regex, ANTECEDENTES_REGLA),
               std::sregex_token_iterator(),
               std::ostream_iterator<std::string>(std::cout, "\n"));
    std::copy( std::sregex_token_iterator(s.begin(), s.end(), regla_regex, CONSECUENTE_REGLA),
               std::sregex_token_iterator(),
               std::ostream_iterator<std::string>(std::cout, "\n"));
    std::copy( std::sregex_token_iterator(s.begin(), s.end(), regla_regex, FACTOR_CERTEZA_REGLA),
               std::sregex_token_iterator(),
               std::ostream_iterator<std::string>(std::cout, "\n"));
    
    auto regla_begin = 
        std::sregex_iterator(s.begin(), s.end(), regla_regex);

    auto regla_end = std::sregex_iterator();
 
    std::cout << "Encontrado "
              << std::distance(regla_begin, regla_end)
              << " reglas\n";
 
    std::cout << "Reglas:\n";
    for (std::sregex_iterator i = regla_begin; i != regla_end; ++i) {
        std::smatch match = *i;
        std::string match_str = match.str();
        std::cout << "  " << match_str << '\n';
    }
}