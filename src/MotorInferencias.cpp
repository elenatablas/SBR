#include "MotorInferencias.hpp"
#include <queue>

#define CONSTANTE 0;

namespace MotorInferencias
{

    /* Encadenamiento hacia atrás o Dirigido por Metas
    Determina si se verifica una cierta meta con los hechos disponibles.
    La inferencia progresa en la red de derecha a izquierda
    */
    bool 
    motorInferencias(char *meta, char **BH, char **BC)
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



}
