#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;

typedef char str30[31];
typedef char str10[11];
typedef unsigned short ushort;

const ushort MAX_ART = 10000;
const ushort TAM_LINEA = 103;
const ushort OFERTAS = 7;

struct sArt {
    int codArt;
    short codRub;
    str30 desc;
    ushort stock;
    float preUni;
    str10 uniMed;
    short ofertas[OFERTAS * 2];
};

struct sIndArt {
    str30 desc;
    int pos;
    bool estado;
};

typedef sIndArt tvrIndArt[MAX_ART];

bool LeerArt(istream &Art, sArt &rArt) {
    Art >> rArt.codArt >> rArt.codRub;
    Art.ignore();
    Art.get(rArt.desc, 30);
    Art >> rArt.stock >> rArt.preUni;
    Art.ignore();
    Art.get(rArt.uniMed, 10);

    for (ushort i = 0; i < OFERTAS * 2; i++)
        Art >> rArt.ofertas[i];

    Art.ignore();
    return Art.good();
}

void IntCmb(sIndArt &elem1, sIndArt &elem2) {
    sIndArt aux = elem1;
    elem1 = elem2;
    elem2 = aux;
}

void OrdxBur(tvrIndArt vrIndArt, ushort card) {
    ushort k = 0;
    bool ordenado;

    do {
        ordenado = true;
        k++;
        for (ushort i = 0; i < card - k; i++)
            if (strcmp(vrIndArt[i].desc, vrIndArt[i + 1].desc) > 0) {
                ordenado = false;
                IntCmb(vrIndArt[i], vrIndArt[i + 1]);
            }
    } while (!ordenado);
}

void VolcarArchivo(ifstream &Art, tvrIndArt vrIndArt, ushort &cantArt) {
    sArt rArt;

    while (LeerArt(Art, rArt) && cantArt < MAX_ART) {
        strcpy(vrIndArt[cantArt].desc, rArt.desc);
        vrIndArt[cantArt].pos = cantArt;
        vrIndArt[cantArt].estado = true;
        cantArt++;
    }
}

void OrdenarIndArt(tvrIndArt vrIndArt, ushort cantArt) {
    OrdxBur(vrIndArt, cantArt);
    freopen("IndDescripArt.txt", "w", stdout);
    for (size_t i = 0; i < cantArt; i++)
        cout << setw(30) << left << vrIndArt[i].desc << ' ' << setw(4) << right
             << vrIndArt[i].pos << ' ' << vrIndArt[i].estado << '\n';

    fclose(stdout);
}

int main() {
    tvrIndArt vrIndArt;
    ushort cantArt = 0;
    ifstream Art("ArticulosMod.txt");

    VolcarArchivo(Art, vrIndArt, cantArt);
    OrdenarIndArt(vrIndArt, cantArt);

    Art.close();
    return 0;
}