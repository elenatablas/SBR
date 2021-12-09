#include <iostream>
#include <iterator>
#include <string>
#include <regex>

#define NUM_REGLA 3
#define ANTECEDENTES_REGLA 4
#define CONSECUENTE_REGLA 9
#define FACTOR_CERTEZA_REGLA 11
 
int main()
{
    std::string s = "7"
"R1: Si A o Ped y er Entonces EXP, FC=0.5"
"R2: SI B Entonces EXP, FC=0.9"
"R3: Si Ce ENTONCES CAN, FC=0.5"
"R4: Si D o D Entonces CAN, FC=1"
"R5: Si EXP y E Entonces CACC, FC=-0.5"
"R6: Si CAN Entonces CACC, FC=0.5"
"R7: Si F y G Entonces CACC, FC=0.7";
 
    const std::regex regla_regex("(R((\\d+)\\s*:\\s*Si\\s+))(\\w+((\\s*y\\s*\\w+)+|(\\s*o\\s*\\w+)+)?)(\\s+Entonces\\s+)(\\w+)(,\\s+FC\\s*=\\s*)(-?\\d+(\\.\\d+)?)", std::regex::icase);

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