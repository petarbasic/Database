#include "heder.h"


int main()
{
    char op[2];
    int opcija;
    do
    {
        printf("Meni \n");
        printf("1. Formiranje prazne datoteke \n");
        printf("2. Odabir aktivne datoteke \n");
        printf("3. Prikazi aktivnu \n");
        printf("4. Formiraj serijsku datoteku \n");
        printf("5. Napravi sekvencijalnu od serijske \n");
        printf("6. Formiraj indeks-sekvencijalnu datoteku od sekvencijalne \n");
        printf("7. Upis novog sloga u aktivnu datoteku \n");
        printf("8. Trazenje sloga u aktivnoj datoteci \n");
        printf("9. Logicko brisanje sloga iz aktivne datoteke \n");
        printf("10. Prikaz svih slogova aktivne datoteke \n");
        printf("11. Izlaz \n");
        printf("12. Prikaz serijske \n");
        printf("13. Prikaz sekvencijalne\n");

        scanf("%s",op);
        opcija=atoi(op);
        if(!opcija)
        {
            printf("\n Neispravan Unos!\n");
            clrscr();
        }
        else
        {
            switch (opcija)
            {
            case 1:
                napravi_fajl();
                clrscr();
                break;
            case 2:
                odaberi_aktivnu();
                clrscr();
                break;
            case 3:
                prikazi_aktivnu();
                clrscr();
                break;
            case 4:
                formiraj_serijsku();
                clrscr();
                break;
            case 5:
                formiraj_sekvencijalnu();
                clrscr();
                break;
            case 6:
                formiraj_indeks_seq2();
                clrscr();
                break;
            case 7:
                upis_u_aktivnu();
                clrscr();
                break;
            case 8:
                trazenje_slucajnog(0,0);
                clrscr();
                break;
            case 9:
                izbrisi();
                clrscr();
                break;
            case 10:
                prikazi_sadrzaj_aktivne();
                clrscr();
                break;
            case 11:
                printf("Rad zavrsen! \n");
                return 0;
            case 12:
                prikazi_serijsku();
                clrscr();
                break;
            case 13:
                prikazi_sekvencijalnu();
                clrscr();
                break;
            default:
                printf("\n\nNeispravam unos, mozete uneti samo brojeve od 1 do 11\n");
                clrscr();
                break;
            }
        }
        printf("\n\n\n");
    }
    while(opcija != 11);
    return 0;
}

void napravi_fajl()
{
    FILE *f;
    char ime_fajla[30];
    printf("\nUnesite ime fajla: ");

    scanf("%s",ime_fajla);

    f=fopen(ime_fajla,"wb");
    if(f)
    {
        printf("\nFajl <%s> uspesno napravljen! ",ime_fajla);
    }
    else
    {
        printf("\nNeuspesno pravljenje fajla <%s> ",ime_fajla);
    }
    fclose(f);
}
void odaberi_aktivnu()
{
    FILE *f;
    char ime_fajla[30];
    printf("\nUnesite ime zeljene aktivne datoteke: ");

    scanf("%s",ime_fajla);
    f=fopen(ime_fajla,"rb");
    if(f)
    {
        printf("\nDatoteka <%s> je postavljena kao aktivna. ",ime_fajla);
        strcpy(aktivna,ime_fajla);
    }
    else
    {
        printf("\nNeuspesno postavljanje aktivne datoteke <%s>.\nDatoteka ne postoji! ",ime_fajla);
    }
    fclose(f);

}
void prikazi_aktivnu()
{

    if(strcmp(aktivna,""))
        printf("\nAktivna datoteka je: %s ",aktivna);
    else
        printf("\nNijedna datoteka nije aktivna!");
}

void formiraj_serijsku()
{
    FILE *fs;
    char ime_fajla[30];
    char broj_emisija[5];
    printf("\nUnesite ime serijske datoteke: ");

    scanf("%s",ime_fajla);
    fs=fopen(ime_fajla,"wb+");
    if(fs){
LOOP:   printf("\n Koliko emisija zelite da uneste? ");
        scanf("%s",broj_emisija);
        if(atoi(broj_emisija)){
        int i;
        int niz[atoi(broj_emisija)];
        for(i=0;i<atoi(broj_emisija);i++){
            printf("\nUnesite emisiju u formatu (ID_emisije,Ime_i_Prezime_Voditelja,Datum,Vreme,Trajanje,Vrsta): ");
            Slog s;
            s.active=1;
            scanf("%d %s %s %s %d %s",&(s.id),(s.voditelj),(s.datum),(s.vreme),&(s.trajanje),(s.vrsta));
            if(proveri_id(niz,s.id,i)){
                niz[i]=s.id;
            }else{
                printf("\n\nEmisija sa id: <%d> je vec uneta!\nPonovite unos %d-tog sloga:\n",s.id,i+1);
                i--;
                continue;
            }
            if((s.trajanje>210)|| (s.trajanje<=0) || (s.id<0) || ((s.id/10000000)>=1) || (proveri_duzinu(s.voditelj,50))|| (proveri_duzinu(s.vrsta,30)) ){
                printf("\n\ngreska pri unosu, ponovite unos %d-tog sloga:\n",i+1);
                i--;
                continue;
            }
            if(fwrite(&s,sizeof(Slog),1,fs)==0){
                printf("\nNeuspesno pisanje u datoteku. \n");
                return;
            }
        }
        fclose(fs);
        FILE *fajlp;
        Slog s;
        fajlp=fopen(ime_fajla,"rb");
        for(i=0;i<atoi(broj_emisija);i++){
            fread(&s,sizeof(Slog),1,fajlp);
            printf("\n\n%d  %s  %s  %s  %d  %s ",s.id,s.voditelj,s.datum,s.vreme,s.trajanje,s.vrsta);
        }
        }else{
            printf("\nPogresan unos!\n");
            goto LOOP;
        }
    }
}
int proveri_id(int *niz,int id,int n){
    int j;
    for(j=0;j<n;j++){
        if(niz[j]==id){
            return 0;
        }
    }
    return 1;
}
int proveri_duzinu(char* string,int d){
    int i;
    for(i=0;string[i]!='\0';++i);
    if(i>d){
        return 1;
    }else{
        return 0;
    }


}
void prikazi_serijsku(){
    FILE *ser;
    Slog s;
    char ime[30];
    printf("\n\nUnesite ime serijske datoteke za prikaz: ");
    scanf("%s",ime);
    ser=fopen(ime,"rb");
    if(ser){
        while(fread(&s,sizeof(Slog),1,ser)){
            printf("\n\nID: %d  Voditelj: %s  Datum: %s  Vreme: %s  Trajanje: %d  Vrsta: %s ",s.id,
                   s.voditelj,s.datum,s.vreme,s.trajanje,s.vrsta);
        }
    fclose(ser);
    }else{
        printf("\n\nNeuspesno otvaranje serijske datoteke %s",ime);
    }
}
void prikazi_sekvencijalnu(){
FILE *seq;
    Blok blck;
    Slog s;
    char ime[30];
    int i;
    printf("\n\nUnesite ime sekvencijalne datoteke za prikaz: ");
    scanf("%s",ime);
    seq=fopen(ime,"rb");
    if(seq){
        while(fread(&blck,sizeof(Blok),1,seq)){
            for(i=0;i<5;i++){
                s=blck.b[i];
                if(s.active != -1){
                    printf("\n\nID: %d  Voditelj: %s  Datum: %s  Vreme: %s  Trajanje: %d  Vrsta: %s ",s.id,
                        s.voditelj,s.datum,s.vreme,s.trajanje,s.vrsta);
                }
            }
        }
        fclose(seq);
    }else{
        printf("\n\nNeuspesno otvaranje sekvencijalne datoteke %s",ime);
    }
}




