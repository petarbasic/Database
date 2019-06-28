#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Math.h>
#include <conio.h>
#include <ctype.h>

#ifndef HEDER_H_INCLUDED
#define HEDER_H_INCLUDED


#define BLOCKING_FACTOR_PZ 5
#define BLOCKING_FACTOR_IZ 2
#define BLOCKING_FACTOR_OZ 1

#define DUZINA_DATUM 8
#define DUZINA_VREME 4
#define DUZINA_ID 7
#define DUZINA_VODITELJ 50
#define DUZINA_VRSTA 30
#define DUZINA_TRAJANJE 210
#define DUZINA_ZONE_PREKORACENJA 20

#define MAX_KLJUC 9999999
#define MIN_KLJUC 0000000

char aktivna[30];
typedef struct Slog{
    int id;
    char voditelj[DUZINA_VODITELJ + 1];
    char datum[DUZINA_DATUM + 1];
    char vreme[DUZINA_VREME + 1];
    int trajanje;
    char vrsta[DUZINA_VRSTA +1];
    int active;
} Slog;

typedef struct Blok{
    Slog b[BLOCKING_FACTOR_PZ];
    int prekoracioci;
} Blok;

typedef struct IZ_slog{
    int kljuc;
    int adresa;
}IZ_slog;

typedef struct IZ_Blok2{
    int kljuc[BLOCKING_FACTOR_IZ];
    int adresa[BLOCKING_FACTOR_IZ];
}IZ_Blok2;

typedef struct Zona_cvor2{
    IZ_Blok2 blokZi;
    struct Zona_cvor2 *levi;
    struct Zona_cvor2 *desni;

}Zona_cvor2;

typedef struct Temp{
    Slog *din_niz;
    int iskoristeno;
    int velicina;
}Temp;

typedef struct Pre_primarne{
    int adresa_primarne;
    int adresa_indeksne;
    int adresa_prekoracenja;
    int primarna_br_blokova;
    int indeksna_br_blokova;
    int prvi_slobodni_prekoracenje;
}Pre_primarne;
typedef struct Blok_Preko{
    Slog prekoracilac;
    int sledeci;
}Blok_Preko;

typedef struct Trazenje_povratna{
    Slog s;
    int uspesno;
    int blok;
    int slog;
    int u_bloku;
}Trazenje_povratna;

void napravi_fajl();
void odaberi_aktivnu();
void prikazi_aktivnu();
void formiraj_serijsku();
void formiraj_sekvencijalnu();
void formiraj_sekvencijalnu2();
void formiraj_indeks_seq2();
void formiraj_indeks_seq();
void prikazi_sadrzaj_aktivne();
void prikazi_sekvencijalnu();
void prikazi_serijsku();
void shellSort();
int nadji_min(Blok bl);
void ispisi_stablo(Zona_cvor2 *root);
void clrscr();
Zona_cvor2 * ucitaj_stablo();
Trazenje_povratna trazenje_slucajnog(int id2,int upis);
int trazi_u_stablu(int id,Zona_cvor2 *cvor);
Trazenje_povratna trazi_u_prekoraciocima(int ,int,int,FILE *);
void izbrisi();
int upis_u_aktivnu();
int proveri_duzinu(char* string,int d);
int proveri_id(int *niz,int id,int n);
#endif // HEDER_H_INCLUDED

