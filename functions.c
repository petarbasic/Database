#include "heder.h"
void formiraj_sekvencijalnu()
{
    FILE *serijska;
    int kraj=1;
    char ime_ser[30];
    char ime_seq[30];

    printf("\nUnesite ime serijske datoteke: ");
    scanf("%s",ime_ser);
    serijska=fopen(ime_ser,"rb");
    if(!serijska)
    {
        printf("\nSerijska datoteka <%s> ne postoji... ",ime_ser);
        return;
    }
    printf("\nUnesite ime nove sekvencijalne datoteke: ");
    scanf("%s",ime_seq);

    Temp t;
    int a=0;
    int i=0;
    t.din_niz=(Slog *)malloc(5*sizeof(Slog));
    t.iskoristeno=0;
    t.velicina=5;
    do{
        if(t.iskoristeno==t.velicina){
            t.velicina *= 2;
            t.din_niz=(Slog *)realloc(t.din_niz, t.velicina * sizeof(Slog));
        }
        if(!fread(&(t.din_niz[t.iskoristeno++]),sizeof(Slog),1,serijska))
            kraj=0;

       if(kraj!=0) a++;
    }while(kraj);
    fclose(serijska);
    shellSort(t.din_niz,a);

    FILE *fseq;
    fseq=fopen(ime_seq,"wb");
    int brojac=0;
    int blokovi=0;
    Blok b;
    if(fseq!=NULL){
        for(i=0;i<a;i++){                           /*Blokiraj sekvencijalnu*/
            b.b[brojac]=t.din_niz[i];
            brojac++;
            b.prekoracioci=0;
            if(brojac==5){
                fwrite(&b,sizeof(Blok),1,fseq);
                blokovi++;
                brojac=0;
            }else if(i==a-1){
                int j;
                for(j=brojac;j<5;j++){              /*Ako blok ima manje od 5 slogova popuni ostatak kao praznim*/
                    b.b[j].id=99999999;
                    b.b[j].active=-1;
                }

                fwrite(&b,sizeof(Blok),1,fseq);
                blokovi++;
            }
        }
        fclose(fseq);
    }else{
        printf("\nNeuspesno otvaranje sekvencijalne datoteke <%s>",ime_seq);
    }
    free(t.din_niz);
    FILE *fproba;
    Blok s; int j;
    fproba=fopen(ime_seq,"rb");
    for(i=0;i<blokovi;i++){
        fread(&s,sizeof(Blok),1,fproba);
        for(j=0;j<5;j++){
            if(s.b[j].active>0){
                printf("\n\nBROJ\n%d  %s  %s  %s  %d  %s ",s.b[j].id,s.b[j].voditelj,s.b[j].datum,s.b[j].vreme,s.b[j].trajanje,s.b[j].vrsta);
            }else{break;}
        }
    }
    fclose(fproba);
}
void formiraj_indeks_seq2(){
    Blok block;
    IZ_Blok2 niz_listova[100];
    FILE *seq,*iseq;
    char ime_seq[30];
    char ime_is[30];
    int brojac_niz=0;

    printf("\nUnesite ime sekvencijalne datoteke za formiranje IS: ");
    scanf("%s",ime_seq);
    printf("\nUnesite zeljeno ime IS datoteke: ");
    scanf("%s",ime_is);
    seq = fopen(ime_seq,"rb");
    if(seq){
        printf("\nSekvencijalna datoteka <%s> otvorena ",ime_seq);
    }else{
        printf("\nGreska pri otvaranju datoteke <%s> ",ime_seq);
        return;
    }

    iseq=fopen(ime_is,"wb+");
    if(iseq){
    //pocetni podaci i njihovo postavljanje pre primarne zone
    Pre_primarne pp={0,0,0,0,0,0};
    fwrite(&pp,sizeof(Pre_primarne),1,iseq);
    pp.adresa_primarne=ftell(iseq);                                             //Pamtim gde pocinje primarna
    //Primarna zona
    int kraj=1;
    int mesto=0;
    int tekuci_list=0;
    do{
        //Odredjivanje na koje mesto u listu buduce zone indeksa ce biti upisan
        //minimalni kljuc iz nekog bloka; pomeranje tekucek lista

        //Kada smo dosli do kraja(EOF)
        if(!fread(&(block),sizeof(Blok),1,seq)){
            kraj=0;
            break;
        }
         if((((brojac_niz)%2)!=0)){
                mesto=1;
            }else{
                mesto=0;tekuci_list++;
            }
        block.prekoracioci=0;
        //povecaj brojac blokova,
        //Ubaci najmanji kljuc i adresu bloka u tekuci list
            niz_listova[tekuci_list-1].kljuc[mesto]=nadji_min(block);
            niz_listova[tekuci_list-1].adresa[mesto]=ftell(iseq);
            fwrite(&block,sizeof(Blok),1,iseq);
            brojac_niz++;
    }while(kraj!=0);
    //Zona indeksa
    int i;
    //Prvo polje u prvom listu je nula
    niz_listova[0].kljuc[0]=0;
    if((brojac_niz%2)!=0){
        niz_listova[tekuci_list-1].kljuc[1]=99999999;
        niz_listova[tekuci_list-1].adresa[1]=0;
    }
    Zona_cvor2 *cvorovi_list=malloc((tekuci_list)*sizeof(Zona_cvor2));
    //Kraj primarne pocetak indeksne, zapamti to zasad
    pp.adresa_indeksne=ftell(iseq);
    //Kreiramo niz listova tako da niko nema decu
    //Upisemo ih na pocetak zone indeksa
    for(i=tekuci_list-1;i>=0;i--){
        cvorovi_list[i].blokZi=niz_listova[i];
        cvorovi_list[i].levi=NULL;
        cvorovi_list[i].desni=NULL;
        fwrite(&cvorovi_list[i],sizeof(Zona_cvor2),1,iseq);
    }
    int m,k;        //m - indeks niza cvorova
    int C=0;
    //Formula za visinu stabla
    int h;
    if(brojac_niz==1){
        h=1;
    }else{
        h=ceil(log2((float)brojac_niz));
    }
    //Ukupan broj cvorova stabla
    for(i=1;i<=h;i++){
        C+=ceil(((float)brojac_niz)/pow(BLOCKING_FACTOR_IZ,h-i+1));
    }
    //Zapamtimo broj blokova u primarnoj i broj cvorova u zoni indeksa
    pp.indeksna_br_blokova=C;
    pp.primarna_br_blokova=brojac_niz;
    //Broj cvorova u nivou listova, napravi niz za sve ostale cvorove kojih ima (C-ch)
    int ch=(int)ceil(((float)brojac_niz)/2);
    Zona_cvor2 *cvor = (Zona_cvor2*)malloc(sizeof(Zona_cvor2)*(C-ch));
    int p=0;
    int cnt=0;
    Zona_cvor2 *root=cvorovi_list;
    //Po nivoima u stablu (nivo listova preskocen-vec postoji)
    for(i=h-1;i>0;i--){
        int ci=ceil(((float)brojac_niz)/pow(BLOCKING_FACTOR_IZ,h-i+1));
        m=0;
        //Racunam broj parova (kljuc,adresa) u krajnjem desnom cvoru tekuceg nivoa
        int broj_parova=ceil(((float)brojac_niz)/pow(BLOCKING_FACTOR_IZ,h-i))-BLOCKING_FACTOR_IZ*(ceil(((float)brojac_niz)/pow(BLOCKING_FACTOR_IZ,h-i+1))-1);
        //za svaki cvor u tekucem nivou
        for(k=0;k<ci;k++){
             printf("\nP= %d",m);
            //Za prvi nivo iznad listova-koristi se niz sa listovima
            if(i==h-1){
            if(cvorovi_list[m].blokZi.kljuc[0] > cvorovi_list[m].blokZi.kljuc[1]){
                cvor[p].blokZi.kljuc[0]=cvorovi_list[m].blokZi.kljuc[1];
                cvor[p].blokZi.adresa[0]=cvorovi_list[m].blokZi.adresa[1];
            }
            else{
                cvor[p].blokZi.kljuc[0]=cvorovi_list[m].blokZi.kljuc[0];
                cvor[p].blokZi.adresa[0]=cvorovi_list[m].blokZi.adresa[0];
            }
            cvor[p].levi=&cvorovi_list[m];
            m++;
            //Ako krajnji desni treba da ima samo levo polje
            if((k==ci-1) && (broj_parova==1)){
                cvor[p].blokZi.kljuc[1]=99999999;
                cvor[p].blokZi.adresa[1]=NULL;
                cvor[p].desni=NULL;
                p++;
                continue;
            }
            if(cvorovi_list[m].blokZi.kljuc[0]>cvorovi_list[m].blokZi.kljuc[1]){
                cvor[p].blokZi.kljuc[1]=cvorovi_list[m].blokZi.kljuc[1];
                cvor[p].blokZi.adresa[1]=cvorovi_list[m].blokZi.adresa[1];
            }else{
                cvor[p].blokZi.kljuc[1]=cvorovi_list[m].blokZi.kljuc[0];
                cvor[p].blokZi.adresa[1]=cvorovi_list[m].blokZi.adresa[0];
            }

            cvor[p].desni=&cvorovi_list[m];
            m++;
            p++;
            //Za vise nivoe se koristi drugi niz sa novonapravljenim cvorovima
            }else{
            if(cvor[m].blokZi.kljuc[0] > cvor[m].blokZi.kljuc[1]){
                cvor[p].blokZi.kljuc[0]=cvor[m].blokZi.kljuc[1];
                cvor[p].blokZi.adresa[0]=cvor[m].blokZi.adresa[1];
            }
            else{
                cvor[p].blokZi.kljuc[0]=cvor[m].blokZi.kljuc[0];
                cvor[p].blokZi.adresa[0]=cvor[m].blokZi.adresa[0];
            }
            cvor[p].levi=&cvor[m];
            m++;
            if((k==ci-1) && (broj_parova==1)){
                cvor[p].blokZi.kljuc[1]=99999999;
                cvor[p].blokZi.adresa[1]=NULL;
                cvor[p].desni=NULL;
                p++;
                continue;
            }
            if(cvor[m].blokZi.kljuc[0]>cvor[m].blokZi.kljuc[1]){
                cvor[p].blokZi.kljuc[1]=cvor[m].blokZi.kljuc[1];
                cvor[p].blokZi.adresa[1]=cvor[m].blokZi.adresa[1];
            }else{
                cvor[p].blokZi.kljuc[1]=cvor[m].blokZi.kljuc[0];
                cvor[p].blokZi.adresa[1]=cvor[m].blokZi.adresa[0];
            }
            cvor[p].desni=&cvor[m];
            m++;
            p++;
            }
        }
        int cntCi=0;
        for(cnt=p-1;cnt>=0;cnt--){
        if(cntCi==ci) break;
        fwrite(&cvor[cnt],sizeof(Zona_cvor2),1,iseq);
        cntCi++;
        }
    }
    if(h>1){ root=&cvor[p-1];}
        //Provera jeeej radi
    ispisi_stablo(root);
    //Zona prekoracenja
    pp.adresa_prekoracenja=ftell(iseq);
    pp.prvi_slobodni_prekoracenje=pp.adresa_prekoracenja;
    //Imam sve podatke upisi ih na pocetak
    rewind(iseq);
    fwrite(&pp,sizeof(Pre_primarne),1,iseq);
    fseek(iseq,pp.adresa_prekoracenja,SEEK_SET);

    Blok_Preko *zona_prekoracilaca= (Blok_Preko *)malloc(sizeof(Blok_Preko)*DUZINA_ZONE_PREKORACENJA);
    fwrite(&zona_prekoracilaca,sizeof(Blok_Preko),DUZINA_ZONE_PREKORACENJA,iseq);
    fclose(iseq);
    free(cvor);
}else{
    printf("Greska pri otvaranju IS datoteke: <%s>",ime_is);
}
     fclose(seq);
}

void ispisi_stablo(Zona_cvor2 *root){
    if (root!=NULL){
        ispisi_stablo(root->levi);
        printf("\nID levi = %d, ID desni = %d ",root->blokZi.kljuc[0],root->blokZi.kljuc[1]);
        ispisi_stablo(root->desni);
    }
}
void prikazi_sadrzaj_aktivne(){
    if(strcmp(aktivna,"")){
        FILE *fis;
        int i,adrs,j,adrs1;
        fis=fopen(aktivna,"rb");
        if(fis){
            Pre_primarne pp;
            fread(&pp,sizeof(Pre_primarne),1,fis);
            Blok blo[pp.primarna_br_blokova];
            for(i=0;i<pp.primarna_br_blokova;i++){
                adrs=ftell(fis);
                fread(&blo[i],sizeof(Blok),1,fis);
                printf("\nAdresa bloka: %d",adrs);
                for(j=0;j<5;j++){
                    Slog s=blo[i].b[j];
                    if(s.active!=-1)
                    printf("\n%d. ID emisije: %07d Voditelj: %s Datum: %s Vreme: %s Trajanje: %d Vrsta: %s",j+1,s.id,s.voditelj,s.datum,s.vreme,s.trajanje,s.vrsta);
                }
                adrs1=ftell(fis);
                if(blo[i].prekoracioci!=0){             /*Blok ima prekoracioce, prikazi ih*/
                    Blok_Preko preko;
                    fseek(fis,blo[i].prekoracioci,SEEK_SET);
                    fread(&preko,sizeof(Blok_Preko),1,fis);
                    printf("\nPrekoracioci:\n");
                    printf("\nID emisije: %07d Voditelj: %s Datum: %s Vreme: %s Trajanje: %d Vrsta: %s",preko.prekoracilac.id,preko.prekoracilac.voditelj,preko.prekoracilac.datum,
                           preko.prekoracilac.vreme,preko.prekoracilac.trajanje,preko.prekoracilac.vrsta);
                    while(preko.sledeci!=0){
                        fseek(fis,preko.sledeci,SEEK_SET);
                        fread(&preko,sizeof(Blok_Preko),1,fis);
                        printf("\nID emisije: %07d Voditelj: %s Datum: %s Vreme: %s Trajanje: %d Vrsta: %s",preko.prekoracilac.id,preko.prekoracilac.voditelj,preko.prekoracilac.datum,
                           preko.prekoracilac.vreme,preko.prekoracilac.trajanje,preko.prekoracilac.vrsta);
                    }
                }
                fseek(fis,adrs1,SEEK_SET);
                printf("\n");
            }
            fclose(fis);
            printf("\n\nStablo:\n");
            Zona_cvor2 *root=ucitaj_stablo();
            ispisi_stablo(root);
        }
    }else{
        printf("\n\nNeispravna aktivna datoteka\n");
    }
}
//Self explanatory
void shellSort(Slog numbers[], int array_size)
{
    int i, j, increment;
    Slog temp;
    increment = 3;
    while (increment > 0)
    {
       for (i=0;i < array_size;i++){
            j = i;
             temp =numbers[i];
             while((j>=increment) && (numbers[j-increment].id > temp.id)){
                numbers[j] = numbers[j - increment];
                j = j - increment;
             }
            numbers[j] = temp;
        }
        if (increment/2 != 0)
            increment = increment/2;
        else if (increment == 1)
            increment = 0;
        else increment = 1;
    }
}
int nadji_min(Blok bl){
    int mini=bl.b[0].id;
    int i;
    for(i=1;i<BLOCKING_FACTOR_PZ;i++){
        if((bl.b[i].id<mini)&&(bl.b[i].active!=-1)){
            mini=bl.b[i].id;
        }
    }
    return mini;
}
void clrscr()
{
    printf("\nPritisnite bilo koje dugme za nastavak\n");
    _getch();
    system("@cls||clear");
}
