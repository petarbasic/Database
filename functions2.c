#include "heder.h"
Zona_cvor2 * ucitaj_stablo(){
    FILE *iseq;
    int i;
    iseq = fopen(aktivna,"rb");
    if(iseq){
        printf("\nIS datoteka <%s> otvorena \n",aktivna);
    }else{
        printf("\nGreska pri otvaranju datoteke <%s> ",aktivna);
        return NULL;
    }
    //Nadji gde se nalaze cvorovi stabla
    Pre_primarne pp;
    fread(&pp,sizeof(Pre_primarne),1,iseq);
    fseek(iseq,pp.adresa_indeksne,SEEK_SET);
    Zona_cvor2 *cvor2 = (Zona_cvor2*)malloc(sizeof(Zona_cvor2)*pp.indeksna_br_blokova);
    for(i=0;i<pp.indeksna_br_blokova;i++){
        fread(&cvor2[i],sizeof(Zona_cvor2),1,iseq);
    }
    int broj_blokova=pp.indeksna_br_blokova;
    //Obrni cvorove
    Zona_cvor2 *cvor = (Zona_cvor2*)malloc(sizeof(Zona_cvor2)*pp.indeksna_br_blokova);
    for(i=0;i<pp.indeksna_br_blokova;i++){
        cvor[i]=cvor2[broj_blokova-i-1];
    }
    /*cvor niz[];
    k=1;
    for(i=0;i<size(niz);i++){
        if((k+i)==size(niz)) break;
        niz[i].levi=&niz[k+i];              //Moj alg za ucitavanje
        k++;
        if((k+i)==size(niz)) break;
        niz[i].desni=&niz[k+i];
    }*/
    //Formiraj veze izmedju cvorova stabla
    int k=1;
    for(i=0;i<broj_blokova;i++){
        if((k+i)==broj_blokova){
            break;
        }
        cvor[i].levi=&cvor[k+i];
        k++;
        if((k+i)==broj_blokova){
            break;
        }
        cvor[i].desni=&cvor[k+i];
        }
    //ispisi_stablo(&cvor[0]);
    fclose(iseq);
    return &cvor[0];
}

Trazenje_povratna trazenje_slucajnog(int id2,int upis){
    FILE *iseq;
    Blok blck;
    Trazenje_povratna povratna;
    int id,adrs,i;
    if(upis){                                /* Da li je korisnik pozvao fju ili upis/brisanje*/
        id=id2;
    }else{
        printf("\nUnesite id: ");
        scanf("%d",&id);
    }
    Zona_cvor2 *root=ucitaj_stablo();
    adrs=trazi_u_stablu(id,root);           /* Nadji adresu bloka u primarnoj zoni gde se slog mozda nalazi*/
    iseq=fopen(aktivna,"rb");
    if(iseq){
        fseek(iseq,adrs,SEEK_SET);
        fread(&blck,sizeof(Blok),1,iseq);   /* */

    int indikator=0;
    //Trazenje u bloku primarne zone
    for(i=0;i<5;i++){
        if((id==blck.b[i].id)){             /*Uspesno trazenje u primarnoj*/
            Slog s=blck.b[i];
            if((blck.b[i].active!=-1)){
                povratna.uspesno=1;
                printf("\n\nSlog nadjen!\nAdresa bloka: %d\nRedni broj sloga u bloku: %d \nID: %d Voditelj: %s, Trajanje: %d min Vrsta: %s\n\n",adrs,
                   i+1,s.id,s.voditelj,s.trajanje,s.vrsta);
            }else{
                povratna.uspesno=0;
                printf("\n\nSlog ranije logicki izbrisan!\n");
            }
            indikator=0;
            povratna.slog=i+1;
            povratna.blok=adrs;
            povratna.s=s;
            povratna.u_bloku=1;
            fclose(iseq);
            return povratna;
        }else{                              /*Nema u primarnoj a veci je od najveceg u primarnoj-zaustavi odmah*/
            if((id<blck.b[i].id) && (blck.b[i].active!=-1)){
                povratna.uspesno=0;
                povratna.slog=i+1;
                povratna.blok=adrs;
                povratna.u_bloku=1;
                printf("\n\nTrazenje sloga sa id: %d je neuspesno, zavrseno u primarnoj zoni\n",id);
                fclose(iseq);
                return povratna;
            }else{
                if(blck.b[i].id==99999999){
                    povratna.uspesno=0;
                    povratna.slog=i+1;
                    povratna.blok=adrs;
                    povratna.u_bloku=1;
                    printf("\n\nTrazenje sloga sa id: %d je neuspesno, zavrseno u primarnoj zoni OVDE\n",id);
                    fclose(iseq);
                    return povratna;
                }else{
                    indikator++;
                }
            }
        }
    }
    if(indikator){
        if(blck.prekoracioci==0){
            printf("\n\nTrazenje sloga sa id: %d je neuspesno.\n",id);
        }else{
            povratna = trazi_u_prekoraciocima(blck.prekoracioci,id,adrs,iseq);
            if(povratna.uspesno==0){
                printf("\n\nTrazenje sloga sa id: %d u prekoraciocima je neuspesno.\n",id);
            }
            fclose(iseq);
            return povratna;
        }
    }
    fclose(iseq);
    }else{
        printf("\n\nGreska pri otvaranju aktivne datoteke");
    }
    povratna.blok=adrs;
    povratna.uspesno=0;
    povratna.slog=5;
    povratna.u_bloku=1;
    return povratna;

}
//Self explanatory
int trazi_u_stablu(int id,Zona_cvor2 *cvor){
    if(cvor==NULL){
        return 0;
    }
    if(id >= cvor->blokZi.kljuc[1]){
        if(cvor->desni != NULL){
            return trazi_u_stablu(id,cvor->desni);
        }else{
            return cvor->blokZi.adresa[1];
        }
    }else{
        if(cvor->levi!=NULL){
            return trazi_u_stablu(id,cvor->levi);
        }else{
            return cvor->blokZi.adresa[0];
        }
    }

}
Trazenje_povratna trazi_u_prekoraciocima(int a,int b,int adrs,FILE *f){
    fseek(f,a,SEEK_SET);                            /*Pomeri tekuci pokazivac na trenutno pretrazivani prekoracilac*/
    Blok_Preko bl;
    Trazenje_povratna p;
    fread(&bl,sizeof(Blok_Preko),1,f);
    if(b==bl.prekoracilac.id){                      /*Trazenje uspesno*/
        p.blok=adrs;
        p.slog=a;
        p.s=bl.prekoracilac;
        p.uspesno=1;
        p.u_bloku=0;
        printf("\nSlog nadjen u prekoraciocima!\nAdresa bloka: %d\nRedni broj sloga u bloku: %d \n ID: %d Voditelj: %s, Trajanje: %d min Vrsta: %s",adrs,
                   5,b,bl.prekoracilac.voditelj,bl.prekoracilac.trajanje,bl.prekoracilac.vrsta);
        return p;
    }else{
        if((bl.sledeci!=0) && (bl.prekoracilac.id<b)){
            return trazi_u_prekoraciocima(bl.sledeci,b,adrs,f);       /*Neuspesno trazenje, pokusaj u sledecem prekoraciocu, ako ga ima*/
        }else{
            if(bl.prekoracilac.id<b){                                 /*Sigurno nema u prekoraciocima, namesti adrese zbog unosa i brisanja*/
                 p.slog=bl.sledeci;
            }else{
                 p.slog=a;
            }
            p.blok=adrs;
            p.uspesno=0;
            p.u_bloku=0;
            return p;
        }
    }

}
int upis_u_aktivnu(){
    Slog s;
    int i;
    int ispravnost=0;
    Blok blck;
    FILE *iseq;
    Pre_primarne pp;
    printf("\nUnesite emisiju u formatu (ID_emisije,Ime_i_Prezime_Voditelja,Datum,Vreme,Trajanje,Vrsta): ");
    while(ispravnost==0){
        scanf("%d %s %s %s %d %s",&(s.id),(s.voditelj),(s.datum),(s.vreme),&(s.trajanje),(s.vrsta));
        if((s.id<0) || (s.id>9999999) || (s.trajanje>210) || (s.trajanje<=0)|| (proveri_duzinu(s.voditelj,50))|| (proveri_duzinu(s.vrsta,30))){
            printf("\n\nNeispravan unos emisije, pokusajte ponovo:\n");
            ispravnost=0;
        }else{
        ispravnost++;
        }
    }
    Trazenje_povratna tp=trazenje_slucajnog(s.id,1);        /*Upisu prethodi neuspesno trazenje*/
    if(tp.uspesno){
        printf("\n\nPokusaj unosa postojece emisije!\n ");
        return 0;
    }else{
        iseq=fopen(aktivna,"rb+");
        if(iseq){
            fread(&pp,sizeof(Pre_primarne),1,iseq);
            fseek(iseq,tp.blok,SEEK_SET);
            fread(&blck,sizeof(Blok),1,iseq);
            if((tp.u_bloku==1)){                            /*Ako se trazenje zaustavilo u bloku primarne zone*/
                if((blck.b[tp.slog-1].id==s.id) && (blck.b[tp.slog-1].active=-1)){  /*Slucaj unosa sloga sa ID koji je ranije izbrisan*/
                    s.active=1;
                    blck.b[tp.slog-1]=s;
                    fseek(iseq,tp.blok,SEEK_SET);
                    fwrite(&blck,sizeof(Blok),1,iseq);
                    printf("\n\nSlog upisan u primarnu zonu umesto starog izbrisanog\n");
                    fclose(iseq);
                    return 1;
                }else{
                   /* if((tp.slog==5)&&(blck.b[tp.slog].id<s.id) ){
                    }else{*/                                  /*Slog treba upisati negde u bloku PZ-pomeranje svih nakon njega*/
                        for(i=tp.slog-1;i<5;i++){
                            Slog temp=blck.b[i];
                            blck.b[i]=s;
                            s=temp;
                            if(temp.id==99999999){          /*Ako slog nije pun nemoj poslednji gurati u prekoracioce*/
                                s.active=-1;
                                break;
                            }
                        }
                    //}
                    if(blck.prekoracioci==0){               /*Slog koji je izguran u prekoracioce je prvi prekoracioc*/
                        if(s.active!=-1){
                        blck.prekoracioci=pp.prvi_slobodni_prekoracenje;
                        Blok_Preko blp;
                        blp.prekoracilac=s;
                        blp.sledeci=0;
                        fseek(iseq,pp.prvi_slobodni_prekoracenje,SEEK_SET);
                        fwrite(&blp,sizeof(Blok_Preko),1,iseq);
                        pp.prvi_slobodni_prekoracenje=ftell(iseq);
                        fseek(iseq,0,SEEK_SET);
                        fwrite(&pp,sizeof(Pre_primarne),1,iseq);
                        printf("\n\nSlog upisan u primarnu zonu\n");
                        }

                    }else{                                  /*Izgurani slog postaje prvi prekoracilac, prevezivanje ostalih*/
                        if(s.active!=-1){
                        int prvi=blck.prekoracioci;
                        blck.prekoracioci=pp.prvi_slobodni_prekoracenje;
                        Blok_Preko novi;
                        fseek(iseq,blck.prekoracioci,SEEK_SET);
                        novi.prekoracilac=s;
                        novi.sledeci=prvi;
                        fwrite(&novi,sizeof(Blok_Preko),1,iseq);
                        pp.prvi_slobodni_prekoracenje=ftell(iseq);
                        fseek(iseq,0,SEEK_SET);
                        fwrite(&pp,sizeof(Pre_primarne),1,iseq);
                        printf("\n\nSlog upisan u zonu prekoracenja kao prvi\n");
                        }
                    }
                        fseek(iseq,tp.blok,SEEK_SET);
                        fwrite(&blck,sizeof(Blok),1,iseq);
                        fclose(iseq);
                        return 1;
                }
            }else if((tp.u_bloku==0)){                      /*Trazenje zavrseno u prekoraciocima*/
                Blok_Preko bp;
                fseek(iseq,blck.prekoracioci,SEEK_SET);
                fread(&bp,sizeof(Blok_Preko),1,iseq);
                if(bp.prekoracilac.id>s.id){                /*Novi slog treba da je prvi prekoracilac*/
                        int prvi=blck.prekoracioci;
                        blck.prekoracioci=pp.prvi_slobodni_prekoracenje;
                        Blok_Preko novi;
                        fseek(iseq,blck.prekoracioci,SEEK_SET);
                        novi.prekoracilac=s;
                        novi.sledeci=prvi;
                        fwrite(&novi,sizeof(Blok_Preko),1,iseq);
                        pp.prvi_slobodni_prekoracenje=ftell(iseq);
                        fseek(iseq,tp.blok,SEEK_SET);
                        fwrite(&blck,sizeof(Blok),1,iseq);
                        printf("\n\nSlog upisan u zonu prekoracenja kao prvi\n");
                }else{
                while((tp.slog!=bp.sledeci)&& (bp.sledeci!=0)){     /*Namesti pokazivac na prekoracilac koji treba da prethodi novom slogu*/
                    fseek(iseq,bp.sledeci,SEEK_SET);
                    fread(&bp,sizeof(Blok_Preko),1,iseq);
                }
                printf("\n\nID: %d\n",bp.prekoracilac.id);
                int priv_sledeci=bp.sledeci;
                bp.sledeci=pp.prvi_slobodni_prekoracenje;
                fseek(iseq,-sizeof(Blok_Preko),SEEK_CUR);           /*Prevezivanje okolna 2 prekoracioca na novi*/
                fwrite(&bp,sizeof(Blok_Preko),1,iseq);
                Blok_Preko blok_p;
                blok_p.prekoracilac=s;
                blok_p.sledeci=priv_sledeci;
                fseek(iseq,pp.prvi_slobodni_prekoracenje,SEEK_SET);
                fwrite(&blok_p,sizeof(Blok_Preko),1,iseq);
                pp.prvi_slobodni_prekoracenje=ftell(iseq);
                printf("\n\nNovi slog upisan u prekoracioce ali ne kao prvi");
                }
                fseek(iseq,0,SEEK_SET);
                fwrite(&pp,sizeof(Pre_primarne),1,iseq);
                fclose(iseq);
                return 1;
            }
        }else{
            printf("\n\nNeuspesno otvaranje aktivne datoteke. Unos nije izvrsen\n");
            return 0;
        }
    }
return 0;
}
void izbrisi(){
    FILE *iseq;
    Blok blck;
    Blok_Preko bl;
    Trazenje_povratna povratna;
    int id;
    printf("\nUnesite ID emisije koju zelite da obrisete: ");
    scanf("%d",&id);
    povratna=trazenje_slucajnog(id,1);
    if(povratna.uspesno==0){
        printf("\nTrazena emisija ne postoji - nije obrisana");
    }else{
        iseq=fopen(aktivna,"rb+");
        if(iseq){
                fseek(iseq,povratna.blok,SEEK_SET);
                fread(&blck,sizeof(Blok),1,iseq);
            if(povratna.u_bloku==1){                /*Slog nadjen u primarnoj zoni, promeni status*/
                blck.b[povratna.slog-1].active=-1;
                fseek(iseq,povratna.blok,SEEK_SET);
                fwrite(&blck,sizeof(Blok),1,iseq);
                fclose(iseq);
                return;                             /*Slog nadjen u prekoraciocima*/
            }else{
                if(blck.prekoracioci==povratna.slog){
                    fseek(iseq,blck.prekoracioci,SEEK_SET);
                    fread(&bl,sizeof(Blok_Preko),1,iseq);
                    if((bl.sledeci==0)&&(bl.prekoracilac.id==id)){
                        bl.prekoracilac.active=-1;
                        blck.prekoracioci=0;
                        printf("\n\nSlog obrisan kao prvi i jedini u prekoraciocima");
                    }else{
                        blck.prekoracioci=bl.sledeci;
                        printf("\n\nSlog obrisan kao prvi u prekoraciocima");
                    }
                    fseek(iseq,povratna.blok,SEEK_SET);
                    fwrite(&blck,sizeof(Blok),1,iseq);
                    fclose(iseq);
                    return;
                }else{
                fseek(iseq,blck.prekoracioci,SEEK_SET);
                int adresa;
                while(bl.sledeci!=povratna.slog){
                    adresa=ftell(iseq);
                    fread(&bl,sizeof(Blok_Preko),1,iseq);
                }
                Blok_Preko za_brisati;
                fseek(iseq,bl.sledeci,SEEK_SET);
                fread(&za_brisati,sizeof(Blok_Preko),1,iseq);
                bl.sledeci=za_brisati.sledeci;
                fseek(iseq,adresa,SEEK_SET);
                fwrite(&bl,sizeof(Blok_Preko),1,iseq);
                printf("\n\nBlok izbrisan u prekoraciocima\n");
                fclose(iseq);
                return;
                }
            }
        }
    }
}
