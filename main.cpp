#include <cstring>
#include <fstream>
#include <iomanip>

using namespace std;

typedef char str30[31];
typedef char str20[21];
typedef char str10[11];
typedef unsigned short ushort;

const ushort MAX_ART = 10000;
const ushort MAX_CPRA = 100;
const ushort RUBROS = 15;
const ushort OFERTAS = 7;
const ushort TAM_LINEA = 103;

struct sArt {
    int codArt;
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

void OrdxBur(tvrRubArt vrRubArt, ushort card) {
    ushort k = 0;
    bool ordenado;

    do {
        ordenado = true;
        k++;
        for (ushort i = 0; i < card - k; i++)
            if (vrRubArt[i].codRub > vrRubArt[i + 1].codRub) {
                ordenado = false;
                IntCmb(vrRubArt[i], vrRubArt[i + 1]);
            }
    } while (!ordenado);
}

int BusBinVec(tvrIndArt vrIndArt, str30 clave, int card) {
    int ult = card - 1;
    int prim = 0;
    int med;

    while (prim <= ult) {
        med = (prim + ult) / 2;
        if (strcmp(vrIndArt[med].desc, clave) == 0)
            return med;
        else if (strcmp(vrIndArt[med].desc, clave) < 0)
            prim = med + 1;
        else
            ult = med - 1;
    }
    return -1;
}

void Abrir(fstream &Art, ifstream &IndArt, ifstream &Rub, ifstream &ListCpra) {
    Art.open("Articulos.txt");
    IndArt.open("IndDescripArt.txt");
    Rub.open("Rubros.txt");
    ListCpra.open("ListaCompras.txt");
}

bool LeerArt(fstream &Art, sArt &rArt) {
    Art >> rArt.codArt >> rArt.codRub;
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

void ActLinea(fstream &Art, sArt rArt, short posArt) {
    Art.clear();
    Art.seekp(posArt * TAM_LINEA);

    Art << setw(8) << right << rArt.codArt << ' ' << setw(2) << right
        << rArt.codRub << ' ' << setw(30) << left << rArt.desc << ' ' << setw(4)
        << right << rArt.stock << ' ' << setprecision(2) << fixed << setw(8)
        << right << rArt.preUni << ' ' << setw(10) << left << rArt.uniMed;

    for (ushort j = 0; j < OFERTAS; j++)
        Art << ' ' << rArt.ofertas[2 * j] << ' ' << setw(2) << right
            << rArt.ofertas[2 * j + 1];
}

void ProcCompras(fstream &Art, tvrListCpra vrListCpra, tvrIndArt vrIndArt,
                 ushort cantArt, ushort cantCpra) {
    sArt rArt;

    for (ushort i = 0; i < cantArt; i++) {
        short posInd = BusBinVec(vrIndArt, vrListCpra[i].desc, cantArt);

        if (posInd == -1 || vrIndArt[posInd].estado == false) {
            vrListCpra[i].cant = 0;
            continue;
        }

        Art.clear();
        Art.seekg(vrIndArt[posInd].pos * TAM_LINEA);
        LeerArt(Art, rArt);

        if (rArt.stock >= vrListCpra[i].cant) {
            rArt.stock -= vrListCpra[i].cant;
        } else {
            vrListCpra[i].cant = rArt.stock;
            rArt.stock = 0;
        }

        ActLinea(Art, rArt, vrIndArt[posInd].pos);
    }
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
    ProcCompras(Art, vrListCpra, vrIndArt, cantArt, cantCpra);
    Cerrar(Art, IndArt, Rub, ListCpra);

    return 0;
}
