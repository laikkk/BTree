/* 
 * File:   main.c
 * Author: kamil
 *
 * Created on 6 kwiecień 2014, 12:59
 */

// struktura wezla B-drzewa i przyklad zapisu i odczytu na plik
// budowanie B-drzewa o zadanej wysokosci i drukowanie w dwoch
// wersjch tekstowo i graficznie
//                                     pmp@inf.ug.edu.pl 2007, 2013

#include <stdio.h>
#include <stdlib.h>
#define T 3   // stopien B-drzewa

typedef struct {
    short n; //ilosc kluczy (-1 oznacza wezel usuniety)
    short leaf; // czy lisc
    int k[2 * T]; // klucze
    int c[2 * T + 1]; // wskazniki do synow (pozycje w pliku: 0,1,2 ...)
    // int info[2 * T - 1]; // wskazniki do informacji skojarzonej z kluczem
    // (pozycje w innym pliku); tutaj nie beda uzyte
    int pozycjaWDrzewie;
} Wezel;
int ROOT = 0;
int rozmiarw = sizeof (Wezel); // rozmiar wezla w bajtach
FILE *drzewo; // plik drzewa (zawierajacy wezly)
int POZYCJAWPLIKU = 0; // numer ostatnio zapisanego wiersza w pliku;

void zapisz(int i, Wezel *w) {
    // zapisuje *w jako i-ty zapis w pliku drzewa
    fseek(drzewo, (long) i*rozmiarw, SEEK_SET);
    fwrite(w, rozmiarw, 1, drzewo);
    //  printf("z%d ",i);
}

void odczytaj(int i, Wezel *w) {
    // odczytuje i-ty zapis w pliku drzewa i wpisuje do *w
    fseek(drzewo, (long) i*rozmiarw, SEEK_SET);
    fread(w, rozmiarw, 1, drzewo);
    //  printf("o%d ",i);
}

// <editor-fold defaultstate="collapsed" desc="funkcje ">

void usun(int i) {
    // usuwa i-ty zapis w pliku drzewa
    // w tej wersji nie wykorzystujemy usunietych pozycji,
    // tylko zaznaczamy jako usuniete
    Wezel w;
    odczytaj(i, &w);
    w.n = -1;
    zapisz(i, &w);
}

int budujB(int g, int n) {
    // buduje B-drzewo o wysokosci g, w kazdym wezle jest n kluczy
    // wynikiem jest pozycja korzenia w pliku - jest to ostatni 
    // zapis w pliku, co jest wazne dla dalszych zapisow do pliku
    // ktore trzeba robic zaczynajac od kolejnej pozycji
    static int klucz = 0; // kolejny klucz
    static int pozycja = 0; // wolna pozycja w pliku
    Wezel w;
    w.n = n;
    int i;
    if (g == 0) { // lisc
        for (i = 0; i < n; i++) {
            w.c[i] = -1; // w lisc
            w.k[i] = klucz++;
        }
        w.c[n] = -1;
        w.leaf = 1;
    } else { // wezel wewnetrzny
        for (i = 0; i < n; i++) {
            w.c[i] = budujB(g - 1, n);
            w.k[i] = klucz++;
        }
        w.c[n] = budujB(g - 1, n);
        ;
        w.leaf = 0;
    }
    zapisz(pozycja++, &w);
    return pozycja - 1;
}

drukujB(int r, int p) {
    // drukuje B-drzewo o korzeniu r (pozycja w pliku)
    // wydruk przesuniety o p w prawo
    Wezel w;
    int i, j;
    odczytaj(r, &w);
    if (w.leaf) {
        for (i = 0; i < p; i++) printf(" ");
        for (i = 0; i < w.n; i++) printf("%d ", w.k[i]);
        printf("\n");
    } else {
        drukujB(w.c[w.n], p + 4);
        for (i = w.n - 1; i >= 0; i--) {
            for (j = 0; j < p; j++) printf(" ");
            printf("%d\n", w.k[i]);
            drukujB(w.c[i], p + 4);
        }
    }
}

void drukujBDot(int r) {
    // przygotowuje wydruk graficzny B-drzewa o korzeniu r (pozycja w pliku)
    // tworzy plik bdrzewo.gv z danymi dla programu dot
    // instrukcja    dot -Tpdf -o bdrzewo.pdf bdrzewo.gv 
    // utworzy plik bdrzewo.pdf z graficzna reprezentacja drzewa
    FILE *plikwy;
    plikwy = fopen("bdrzewo.gv", "w");
    fprintf(plikwy, "graph bdrzewo{\n");
    fprintf(plikwy, "size = \"2,20\"");
    rekDrukujBDot(r, 0, plikwy);
    fprintf(plikwy, "}\n");
    fclose(plikwy);
    printf("utworzony plik bdrzewo.gv");
}

int rekDrukujBDot(int r, int z, FILE *plikwy) {
    // pomocnicza funkcja dla drukujBDot()
    // drukuje B-drzewo o korzeniu r (pozycja w pliku)
    // z  - numer wezla
    // zwraca najwiekszy numer wezla w poddrzewie, ktorego jest korzeniem
    //d [shape=box, width=5.0,length=3, height=0.3]
    Wezel w;
    int nz, i;
    odczytaj(r, &w);
    fprintf(plikwy, "%d [shape=box, label=\"", z);
    for (i = 0; i < w.n; i++) fprintf(plikwy, "%d ", w.k[i]);
    fprintf(plikwy, "\"]\n");
    nz = z;
    if (!w.leaf) {
        for (i = 0; i <= w.n; i++) {
            fprintf(plikwy, "%d -- %d ;\n", z, nz + 1);
            nz = rekDrukujBDot(w.c[i], nz + 1, plikwy);
        }
    }
    return nz;
}// </editor-fold>

//int mojOdczyt(){}
//int mojZapis(int k){
// fseek(drzewo, (long) i*rozmiarw, SEEK_SET);
//    fwrite(w, rozmiarw, 1, drzewo);
//}

void zerujKluczeWezla(Wezel *w) {
    int i;
    for (i = 0; i <= 2 * T - 1; i++) {
        w->k[i] = 0;
        w->c[i] = 0;
    }
    w->c[i] = 0;
}

void drukuj() {
    int dl, i = 0,j;
    Wezel w;
    for (i = 0; i < POZYCJAWPLIKU; i++) {
        odczytaj(i, &w);
        printf("%d. WEZEL n = %d leaf = %d\n", i, w.n, w.leaf);
        printf("               klucze = ");
        for(j=1;j<= w.n;j++)
        printf("[%d] %d ",j, w.k[j]);
        printf("\n");
        printf("wskazniki do synow = ");
        for(j=1;j<= w.n+1;j++)
        printf("[%d] %d ",j, w.c[j]);
        printf("\n\n");
    }
//      int dl, i = 0;
//    Wezel w;
//    for (i = 0; i < POZYCJAWPLIKU; i++) {
//        odczytaj(i, &w);
//        printf("%d. WEZEL n = %d leaf = %d\n", i, w.n, w.leaf);
//        printf("               klucze= [1] %d [2] %d [3] %d [4] %d [5] %d\n", w.k[1], w.k[2], w.k[3], w.k[4], w.k[5]);
//        printf("wskazniki do synow= [1] %d [2] %d [3] %d [4] %d [5] %d [6] %d\n", w.c[1], w.c[2], w.c[3], w.c[4], w.c[5], w.c[6]);
//        printf("\n\n");
//    }
}

void drukujZawartoscWezla(Wezel *w) {
    printf("WEZEL n = %d leaf = %d\n", w->n, w->leaf);
    printf("               klucze= [1] %d [2] %d [3] %d [4] %d [5] %d\n", w->k[1], w->k[2], w->k[3], w->k[4], w->k[5]);
    printf("wskazniki do synow= [1] %d [2] %d [3] %d [4] %d [5] %d [6] %d\n", w->c[1], w->c[2], w->c[3], w->c[4], w->c[5], w->c[6]);
    printf("-------------------------------\n");

}

void BTreeSplitChild(
        Wezel *x, //NIE pełny węzeł wewnętrzny
        int i, //index
        Wezel *y //Pęłny węzeł y (syn x)
        ) {
    //static int pozycja = 0; // wolna pozycja w pliku
    Wezel z;
    zerujKluczeWezla(&z);
    z.leaf = y->leaf;
    z.n = T - 1; // ?-1
    int j;
    for (j = 1; j <= T - 1; j++) { //?!?!adaptacja T-1 skrajnie prawych kluczy z wezla y do wezla Z
        z.k[j] = y->k[j + T ];
    }
    //printf("Wezel z klucze= [0] %d [1] %d [2] %d [3] %d [4] %d\n", z.k[0], z.k[1], z.k[2], z.k[3], z.k[4]);
    //drukujZawartoscWezla(&z);
    if (!y->leaf) { //?               //jezeli y był lisciem to przepisuj tez wskazniki do synow (pozycje w pliku 0,1,2..)
        for (j = 1; j <= T; j++) { //j=0; //DO TRZETESTOWANIA POZNIEJ JAK DRZEWO BD WYSOKIE
            z.c[j] = y->c[j + T];
        }
    }
    drukujZawartoscWezla(&z);
    y->n = T - 1; // - 1; //wezel y nie jest juz pełen
    for (j = x->n + 1; j >= i + 1; j--) { //? przesuniecie wskaznikow do synow x'sa aby zrobic miejsce na nowy wskaznik do nowo przyjetego klucza z wezla y
        x->c[j + 1] = x->c[j];
    }
    //x.c[i + 1] = POZYCJAWPLIKU; //x.c[i+1]=z //mozliwy blad z zapisem pod ktory wiersz ma byc wpisany wezel z do pliku
    x->c[i + 1] = POZYCJAWPLIKU; //raczej powinnoo byc 1 zamiast 2
    z.pozycjaWDrzewie = POZYCJAWPLIKU; //zamiana
    zapisz(POZYCJAWPLIKU++, &z); //z wstawic na kolejna pozycje w pliku ktory bd synem x
    for (j = x->n; j >= i; j--) {
        x->k[j + 1] = x->k[j];
    }
    //x->k[i]= y->k[T];
    x->k[i] = y->k[T]; //dodanie srodkowego klucza z wezla Y do węzła X 
    x->n = x->n + 1;
    //disk-write(y)   
    //disk-write(x)
    zapisz(y->pozycjaWDrzewie, y);
    zapisz(x->pozycjaWDrzewie, x);
}
//!! TODO ustawiac pozycje na którym moja byc dodane wskaniki na nastepne wezly (wiersze)

void BTreeInsertNonFull(
        Wezel *x, //musi byc Wezlem bo do niego? bd wstawionny nowy klucz k
        int k // klucz który bd wstawiony
        ) {
    int i = x->n; //ilosc kluczy w wezle x
    if (x->leaf) {
        //int czyOdjelo=0;
        while (i >= 1 && k < x->k[i]) {//k[i]
            x->k[i + 1] = x->k[i]; //x->k[i+1]=x->k[i]
            i = i - 1;
            //czyOdjelo=1;
        }
        // if(i >= 1 && czyOdjelo==0)i = i - 1;
        // if( i >= 1 && k < x->k[i - 1]) x->k[i-1] = k;  else
        x->k[i + 1] = k; //x->k[i+1]=k
        x->n = x->n + 1;
        //DiskWrite(x); //Zapisz zmiany (czyli dodanie klucza k)
        zapisz(x->pozycjaWDrzewie, x);
    } else {

        //drukuj();
        while (i >= 1 && k < x->k[i]) { //x->k[i]
            i = i - 1;
        }

        i = i + 1;
        Wezel tmp;
        //        if(i==1)//Prawdopodobnie zle wybiera który wezel ma wczytac z pliku
        //        odczytaj(x->c[i], &tmp); // wczytujemy wezel z numeru x.c[i]    
        //        else 
        // printf("Wstawiam k = %d Wczytuje nastepny wezel i=%d\n",k,i);
        //drukujZawartoscWezla(x);

        odczytaj(x->c[i], &tmp);
        if (tmp.n == 2 * T - 1) {
            BTreeSplitChild(x, i, &tmp); // na i-tej pozycji wstaw klucz z wezla  TMP
            if (k > x->k[i])
                i = i + 1;
        }
        odczytaj(x->c[i], &tmp);
        BTreeInsertNonFull(&tmp, k);
    }
}

void BTreeInsert(Wezel TT, int k) {
    //r=root[T]?
    Wezel r;
    odczytaj(ROOT, &r);
    if (r.n == (2 * T - 1)) { //mozliwy blad r.n jest pelne dla T=3 gdy jest r.n=4
        Wezel s; //mozliwe allokacja pamieci; s ma zostac nowym korzeniem (jak dac mu roota?)
        zerujKluczeWezla(&s);
        s.pozycjaWDrzewie = POZYCJAWPLIKU;
        ROOT = POZYCJAWPLIKU;
        s.leaf = 0;
        s.n = 0;
        s.c[1] = r.pozycjaWDrzewie; //r;//podobna sytuacja jak wyzej, w s.c[1] ma byc numer wierszu w kótrym znajduje sie zapisana tam wartosc r
        zapisz(POZYCJAWPLIKU++, &s);
        BTreeSplitChild(&s, 1, &r); //test z 0
        //wczytac ponownie S?
        //odczytaj(s.pozycjaWDrzewie,&s);
        BTreeInsertNonFull(&s, k); //te s nie wie o zmianach w f BTreeSplitChild
    } else BTreeInsertNonFull(&r, k); //do wezla r dodajemy klucz k
}

Wezel BTreeCreate() {
    Wezel x;
    x.leaf = 1;
    x.n = 0;
    x.pozycjaWDrzewie = POZYCJAWPLIKU;
    zerujKluczeWezla(&x);
    //DiskWrite(X)
    //root[T]=x
    ROOT = POZYCJAWPLIKU;
    zapisz(POZYCJAWPLIKU++, &x);
    return x;
}

void drukujZawartoscPliku() {
    int i = 0;
    Wezel w;
    //for(i=0;i<POZYCJAWPLIKU;i++){
    for (i = 0; i < 3; i++) {
        odczytaj(i, &w);
        printf("%d. WEZEL n = %d leaf = %d\n", i, w.n, w.leaf);
        printf("            klucze= [0] %d [1] %d [2] %d [3] %d [4] %d\n", w.k[0], w.k[1], w.k[2], w.k[3], w.k[4]);
        printf("wskazniki do synow= [0] %d [1] %d [2] %d [3] %d [4] %d [5] %d\n", w.c[0], w.c[1], w.c[2], w.c[3], w.c[4], w.c[5]);
        printf("*******************************\n");
    }
}

Wezel BTreeSearch(Wezel x, int k) {
    int i = 1;
    while (i <= x.n && k > x.k[i])
        i++;
    if (i <= x.n && k == x.k[i]) {
        printf("Znalazlem wezel posiadajacy klucz k = %d  i = %d\n", k, i);
        return x;
    }
    if (x.leaf) {
        printf("NIE Znalazlem wezela posiadajacego klucza k = %d ;(\n", k);
        return;
    } else {
        Wezel tmp;
        odczytaj(x.c[i], &tmp);
        return BTreeSearch(tmp, k);
    }


}

int main() {
    //    int i;
    //    double sp;
    //    drzewo = fopen("bdrzewo.txt", "w+");
    //    Wezel wezel;
    //    int root;
    //    root = budujB(2, 2);
    //    printf("\n");
    //    drukujB(root, 0);
    //    drukujBDot(root);
    //    fclose(drzewo);

    drzewo = fopen("bdrzewo.txt", "w+");
    Wezel TT = BTreeCreate();
    //    BTreeInsert(TT, 99);
    //    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    //    drukuj();
    //    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    //    BTreeInsert(TT, 1);
    //    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    //    drukuj();
    //    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    //    BTreeInsert(TT, 2);
    //    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    //    drukuj();
    //    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    //    BTreeInsert(TT, 3);
    //    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    //    drukuj();
    //    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    //    //drukujZawartoscPliku();
    //    BTreeInsert(TT, 4); // do tego miejsca jest ok
    //    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    //    drukuj();
    //    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    //    BTreeInsert(TT, 5);
    ////    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    ////    drukuj();
    ////    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    //    BTreeInsert(TT, 6);
    ////    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    ////    drukuj();
    ////    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    //    BTreeInsert(TT, 7);
    ////    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    ////    drukuj();
    ////    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    //    BTreeInsert(TT, 8);
    ////    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    ////    drukuj();
    ////    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    //    BTreeInsert(TT, 100);
    //    
    //    BTreeInsert(TT, 10);

    //drukujZawartoscPliku();
    //    int i;
    //    for(i=10;i>4;i--)
    //        BTreeInsert(TT, i);

    int i;
    for (i = 1; i < 27; i++)
        BTreeInsert(TT, i);

    //    BTreeInsert(TT, 10);
    //    BTreeInsert(TT, 9);
    //    BTreeInsert(TT, 8);
    //    BTreeInsert(TT, 7);
    //    BTreeInsert(TT, 6);
    //    BTreeInsert(TT, 5);
    //     BTreeInsert(TT, 4);
    //    BTreeInsert(TT, 3);
    //    BTreeInsert(TT, 2);


    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    drukuj();
    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    Wezel r;
    odczytaj(ROOT, &r);
    BTreeSearch(r, 1);
    BTreeSearch(r, -3);
    drukujBDot(ROOT);
    fclose(drzewo);
    printf("POZYCJAWPLIKU = %d ROOT = %d\n", POZYCJAWPLIKU, ROOT);
    return EXIT_SUCCESS;
}


