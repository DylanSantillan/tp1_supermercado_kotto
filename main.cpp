#include <cstring>
#include <fstream>

using namespace std;

typedef char str30[31];
typedef char str20[21];
typedef char str10[11];
typedef unsigned short ushort;

const ushort MAX_ART = 10000;
const ushort MAX_CPRA = 100;
const ushort RUBROS = 15;
const ushort OFERTAS = 7;

struct sArt {
    int codArd;
    short codRub;
    str30 desc;
    ushort stock;
    float preUni;
    str10 uniMed;
    short ofertas[OFERTAS * 2];
};

struct sDescArt {
    str30 desc;
    int pos;
    bool estado;
};

struct sRub {
    short codRub;
    str20 descRub;
};

struct sCpra {
    str30 desc;
    short cant;
};

struct sRubArt {
    short codRub;
    int pos;
};

typedef sDescArt tvrIndArt[MAX_ART];
typedef sCpra tvrListCpra[MAX_CPRA];
typedef sRubArt tvrRubArt[MAX_ART];

void IntCmb(sRubArt &elem1, sRubArt &elem2) {
    sRubArt aux = elem1;
    elem1 = elem2;
    elem2 = aux;
}

void OrdxBur(tvrRubArt vrRubArt, ushort cantArt) {
    ushort k = 0;
    bool ordenado;
    do {
        ordenado = true;
        k++;
        for (ushort i = 0; i < cantArt - k; i++)
            if (vrRubArt[i].codRub > vrRubArt[i + 1].codRub) {
                ordenado = false;
                IntCmb(vrRubArt[i], vrRubArt[i + 1]);
            }
    } while (!ordenado);
}

void Abrir(fstream &Art, ifstream &IndArt, ifstream &Rub, ifstream &ListCpra) {
    Art.open("Articulos.txt");
    IndArt.open("IndDescripArt.txt");
    Rub.open("Rubros.txt");
    ListCpra.open("ListaCompras.txt");
}

bool LeerArt(fstream &Art, sArt &rArt) {
    Art >> rArt.codArd >> rArt.codRub;
    Art.ignore();
    Art.get(rArt.desc, 30);
    Art >> rArt.stock >> rArt.preUni;
    Art.ignore();
    Art.get(rArt.uniMed, 10);

    for (ushort i = 0; i < OFERTAS * 2; i++)
        Art >> rArt.ofertas[i];

    return Art.good();
}

bool LeerDescArt(ifstream &IndArt, sDescArt &rDescArt) {
    IndArt.get(rDescArt.desc, 31);
    IndArt >> rDescArt.pos >> rDescArt.estado;
    IndArt.ignore();
    return IndArt.good();
}

bool LeerRub(ifstream &Rub, sRub &rRub) {
    Rub >> rRub.codRub;
    Rub.ignore();
    Rub.get(rRub.descRub, 21);
    Rub.ignore();
    return Rub.good();
}

bool LeerCpra(ifstream &ListCpra, sCpra &rCpra) {
    ListCpra.get(rCpra.desc, 31);
    ListCpra >> rCpra.cant;
    ListCpra.ignore();
    return ListCpra.good();
}

void VolcarArchivos(fstream &Art, ifstream &IndArt, ifstream &ListCpra,
                    tvrIndArt vrIndArt, tvrRubArt vrRubArt,
                    tvrListCpra vrListCpra, ushort &cantArt, ushort &cantCpra) {
    sArt rArt;
    sDescArt rDescArt;
    sCpra rCpra;

    while (LeerDescArt(IndArt, rDescArt) && cantArt < MAX_ART)
        vrIndArt[cantArt++] = rDescArt;

    for (ushort i = 0; LeerArt(Art, rArt) && i < cantArt; i++) {
        vrRubArt[i].codRub = rArt.codRub;
        vrRubArt[i].pos = i;
    }

    while (LeerCpra(ListCpra, rCpra) && cantCpra < MAX_CPRA)
        vrListCpra[cantCpra++] = rCpra;

    OrdxBur(vrRubArt, cantArt);
}

void Cerrar(fstream &Art, ifstream &IndArt, ifstream &Rub, ifstream &ListCpra) {
    Art.close();
    IndArt.close();
    Rub.close();
    ListCpra.close();
}

int main() {
    tvrIndArt vrIndArt;
    tvrRubArt vrRubArt;
    tvrListCpra vrListCpra;

    fstream Art;
    ifstream IndArt, Rub, ListCpra;

    ushort cantArt = 0, cantCpra = 0;

    Abrir(Art, IndArt, Rub, ListCpra);
    VolcarArchivos(Art, IndArt, ListCpra, vrIndArt, vrRubArt, vrListCpra,
                   cantArt, cantCpra);
    Cerrar(Art, IndArt, Rub, ListCpra);

    return 0;
}
