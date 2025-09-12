#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;

typedef char str30[31];
typedef char str20[21];
typedef char str10[11];
typedef unsigned short ushort;

const ushort MAX_ART = 10000;
const ushort MAX_CPRA = 100;
const ushort RUBROS = 15;
const ushort OFERTAS = 7;
const ushort TAM_LINEA = 104;

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

typedef sIndArt tvrIndArt[MAX_ART];
typedef sRubArt tvrRubArt[MAX_ART];
typedef sCpra tvrListCpra[MAX_CPRA];
typedef sRub tvrRub[RUBROS];

void Abrir(fstream &Art, ifstream &IndArt, ifstream &Rub, ifstream &ListCpra) {
    Art.open("Articulos.txt", ios::in | ios::out);
    IndArt.open("IndDescripArt.txt");
    Rub.open("Rubros.txt");
    ListCpra.open("ListaCompras.txt");
}

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

bool LeerArt(fstream &Art, sArt &rArt) {
    Art >> rArt.codArt >> rArt.codRub;
    Art.ignore();
    Art.get(rArt.desc, 31);
    Art >> rArt.stock >> rArt.preUni;
    Art.ignore();
    Art.get(rArt.uniMed, 11);

    for (ushort i = 0; i < OFERTAS * 2; i++)
        Art >> rArt.ofertas[i];

    Art.ignore();
    return Art.good();
}

bool LeerIndArt(ifstream &IndArt, sIndArt &rIndArt) {
    IndArt.get(rIndArt.desc, 31);
    IndArt >> rIndArt.pos >> rIndArt.estado;
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
                    ifstream &Rub, tvrIndArt &vrIndArt, tvrRubArt &vrRubArt,
                    tvrListCpra &vrListCpra, tvrRub &vrRub, ushort &cantArt,
                    ushort &cantCpra) {
    sArt rArt;
    sIndArt rIndArt;
    sCpra rCpra;
    sRub rRub;

    while (LeerIndArt(IndArt, rIndArt) && cantArt < MAX_ART)
        vrIndArt[cantArt++] = rIndArt;

    for (ushort i = 0; LeerArt(Art, rArt) && i < cantArt; i++) {
        vrRubArt[i].codRub = rArt.codRub;
        vrRubArt[i].pos = i;
    }

    while (LeerCpra(ListCpra, rCpra) && cantCpra < MAX_CPRA)
        vrListCpra[cantCpra++] = rCpra;

    for (ushort i = 0; LeerRub(Rub, rRub) && i < RUBROS; i++)
        vrRub[i] = rRub;

    OrdxBur(vrRubArt, cantArt);
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

void ProcCompras(fstream &Art, tvrListCpra &vrListCpra, tvrIndArt &vrIndArt,
                 ushort cantArt, ushort cantCpra) {
    sArt rArt;

    for (ushort i = 0; i < cantCpra; i++) {
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

long GetDate(int &year, int &mes, int &dia, int &ds) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    year = 1900 + timeinfo->tm_year;
    mes = 1 + timeinfo->tm_mon;
    dia = timeinfo->tm_mday;
    ds = 1 + timeinfo->tm_wday;
    return (1900 + timeinfo->tm_year) * 10000 + (1 + timeinfo->tm_mon) * 100 +
           timeinfo->tm_mday;
}

long GetTime(int &hh, int &mm, int &ss) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    hh = timeinfo->tm_hour;
    mm = timeinfo->tm_min;
    ss = timeinfo->tm_sec;
    return timeinfo->tm_hour * 10000 + timeinfo->tm_min * 100 +
           timeinfo->tm_sec;
}

string Replicate(char car, unsigned n) {
    string cad = "";
    for (unsigned i = 1; i <= n; i++)
        cad += car;
    return cad;
}

void CabeceraTicket(int &ds) {
    int year, mes, dia, hh, mm, ss;

    GetDate(year, mes, dia, ds);
    GetTime(hh, mm, ss);

    str10 nomDia;

    switch (ds) {
        case 1:
            strcpy(nomDia, "Domingo");
            break;
        case 2:
            strcpy(nomDia, "Lunes");
            break;
        case 3:
            strcpy(nomDia, "Martes");
            break;
        case 4:
            strcpy(nomDia, "Miercoles");
            break;
        case 5:
            strcpy(nomDia, "Jueves");
            break;
        case 6:
            strcpy(nomDia, "Viernes");
            break;
        case 7:
            strcpy(nomDia, "Sabado");
            break;
    }

    cout << "K O T T O\n";
    cout << "Yo te reconozco\n";
    cout << "SUC 170\n";
    cout << "XXXXXX…X 9999\n";
    cout << "XX…X\n";
    cout << "C.U.I.T. 99-99999999-9\n";
    cout << "Fecha: " << setw(10) << left << nomDia << ' ' << setw(2)
         << setfill('0') << right << dia << '/' << setw(2) << setfill('0')
         << right << mes << '/' << year << setfill(' ') << '\n';
    cout << "Hora: " << setw(2) << setfill('0') << right << hh << ':' << setw(2)
         << setfill('0') << right << mm << ':' << setw(2) << setfill('0')
         << right << ss << setfill(' ') << '\n';
    cout << "Nro. Ticket: 9999-99999999\n";
    cout << "Nro. Caja: 9999\n";
    cout << Replicate('-', 45) << '\n';
    cout << "F A C T U R A - B\n";
    cout << "ORIGINAL\n";
    cout << Replicate('-', 45) << '\n';
}

void PieTicket(float impTot, float impTotDesto, float impTotConDesto) {
    cout << setw(30) << left << "Su pago con Tipo Pago:" << " $ " << setw(10)
         << right << impTotConDesto << '\n';
    cout << setw(30) << left << "Su vuelto:" << " $ " << setw(10) << right << 0
         << '\n';
    cout << "  G R A C I A S  P O R  S U  C O M P R A\n";
    cout << "  Para consultas, sugerencias o reclamos\n";
    cout << "  comunicarse al correo infoKotto.com.ar\n";
}

void EmitirTicket(fstream &Art, tvrListCpra &vrListCpra, tvrIndArt &vrIndArt,
                  ushort cantCpra, ushort cantArt) {
    int ds;
    sArt rArt;
    float impTot = 0.0f, impTotDesto = 0.0f;

    freopen("Ticket.txt", "w", stdout);
    CabeceraTicket(ds);

    for (ushort i = 0; i < cantCpra; i++) {
        if (vrListCpra[i].cant == 0)
            continue;

        short posInd = BusBinVec(vrIndArt, vrListCpra[i].desc, cantArt);

        Art.clear();
        Art.seekg(vrIndArt[posInd].pos * TAM_LINEA);
        LeerArt(Art, rArt);

        ushort tipoPromo = rArt.ofertas[(ds - 1) * 2];
        ushort promoDesto = rArt.ofertas[(ds - 1) * 2 + 1];
        float impTotItem = vrListCpra[i].cant * rArt.preUni;
        float ImpTotDesc = (impTotItem * promoDesto) / 100;
        str10 nomPromo;

        switch (tipoPromo) {
            case 1:
                strcpy(nomPromo, "SinPromo");
                break;
            case 2:
                strcpy(nomPromo, "Promo");
                break;
            case 3:
                strcpy(nomPromo, "Marca");
                break;
            case 4:
                strcpy(nomPromo, "Jub.");
                break;
            case 5:
                strcpy(nomPromo, "Comunid.");
                break;
            case 6:
                strcpy(nomPromo, "MercPago");
                break;
            case 7:
                strcpy(nomPromo, "ANSES");
                break;
        }

        cout << setw(2) << right << vrListCpra[i].cant << " x $ "
             << setprecision(2) << fixed << setw(8) << right << rArt.preUni
             << '\n';
        cout << setw(30) << left << rArt.desc << ' ' << setw(10) << left
             << rArt.uniMed << '\n';
        cout << setw(8) << right << rArt.codArt << ' ' << setw(21) << ' '
             << " $ " << setw(10) << right << impTotItem << '\n';

        if (strcmp(nomPromo, "SinPromo") != 0) {
            cout << setw(10) << left << nomPromo << ' ' << tipoPromo << setw(18)
                 << ' ' << " $ " << setw(10) << right << ImpTotDesc << '\n';
            impTotDesto += ImpTotDesc;
        }

        impTot += impTotItem;
        cout << '\n';
    }

    cout << setw(30) << left << "SubTot. sin descuentos....:" << " $ "
         << setw(10) << right << impTot << '\n';
    cout << setw(30) << left << "Descuentos por promociones:" << " $ "
         << setw(10) << right << impTotDesto << '\n';

    float impTotConDesto = impTot - impTotDesto;

    cout << Replicate('=', 45) << '\n';
    cout << setw(30) << left << "T O T A L" << " $ " << setw(10) << right
         << impTotConDesto << '\n';
    cout << Replicate('=', 45) << '\n';

    PieTicket(impTot, impTotDesto, impTotConDesto);
}

void EmitirArt_x_Rubro(fstream &Art, tvrRubArt &vrRubArt, tvrRub &vrRub,
                       ushort cantArt) {
    sArt rArt;
    string titulo = "Listado de Articulos ordenados por Codigo de Rubro";
    int espacios = (TAM_LINEA - titulo.length()) / 2;

    cout << "\n\n" << Replicate('-', TAM_LINEA) << '\n';
    cout << setw(espacios + titulo.length()) << titulo << '\n';
    cout << Replicate('=', TAM_LINEA) << '\n';

    short rubroActual = -1;

    for (ushort i = 0; i < cantArt; i++) {
        Art.clear();
        Art.seekg(vrRubArt[i].pos * TAM_LINEA);
        LeerArt(Art, rArt);

        if (rArt.codRub != rubroActual) {
            rubroActual = rArt.codRub;

            for (ushort j = 0; j < RUBROS; j++) {
                if (vrRub[j].codRub != rubroActual)
                    continue;

                cout << "\nCod. Rubro: " << setw(2) << right << vrRub[j].codRub
                     << ' ' << vrRub[j].descRub << '\n';

                cout << "Cod.Art." << ' ' << setw(30) << left << "Descripcion"
                     << ' ' << "Stk." << ' ' << "Pre.Uni." << ' ' << setw(10)
                     << left << "U.Med.";

                for (ushort k = 0; k < OFERTAS; k++)
                    cout << ' ' << "TD" << right << ' ' << "%";

                cout << '\n' << Replicate('-', TAM_LINEA) << '\n';
            }
        }

        cout << setw(8) << right << rArt.codArt << ' ' << setw(30) << left
             << rArt.desc << ' ' << setw(4) << right << rArt.stock << ' '
             << setprecision(2) << fixed << setw(8) << right << rArt.preUni
             << ' ' << setw(10) << left << rArt.uniMed;

        for (ushort j = 0; j < OFERTAS; j++)
            cout << ' ' << rArt.ofertas[2 * j] << ' ' << setw(2) << right
                 << rArt.ofertas[2 * j + 1];

        cout << '\n';
    }
    fclose(stdout);
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
    tvrRub vrRub;

    fstream Art;
    ifstream IndArt, Rub, ListCpra;

    ushort cantArt = 0, cantCpra = 0;

    Abrir(Art, IndArt, Rub, ListCpra);
    VolcarArchivos(Art, IndArt, ListCpra, Rub, vrIndArt, vrRubArt, vrListCpra,
                   vrRub, cantArt, cantCpra);
    ProcCompras(Art, vrListCpra, vrIndArt, cantArt, cantCpra);
    EmitirTicket(Art, vrListCpra, vrIndArt, cantCpra, cantArt);
    EmitirArt_x_Rubro(Art, vrRubArt, vrRub, cantArt);
    Cerrar(Art, IndArt, Rub, ListCpra);

    return 0;
}
