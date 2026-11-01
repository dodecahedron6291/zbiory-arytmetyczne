#include "zbior_ary.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

// komentarz dotyczacy danej linijki jest nad nia lub na prawo od niej

int min(int a, int b) { return a < b ? a : b; }
int max(int a, int b) { return a > b ? a : b; }
// zeby wynik modulo trafial w {0,1,...,q-1}
int mod(int a, int q) {
  int x = a % q;
  if (x < 0) x = (x + q) % q;
  return x;
}
static int q_globalne;

// sprawdza czy dany wskaznik zostal po mallocu poprawnie zaalokowany, jesli nie
// to wywala program
void check_alloc(void* ptr) {
  if (ptr == NULL) {
    fprintf(stderr, "blad w alokacji pamieci\n");
    exit(1);
  }
}

// zwalnianie pamieci zaalokowanej przez wskazniki wewnatrz zbioru
void free_ary(zbior_ary* Aptr) {
  if (Aptr == NULL) return;
  free(Aptr->poczatek);
  free(Aptr->koniec);
  Aptr->poczatek = NULL;
  Aptr->koniec = NULL;
  Aptr->ary = 0;
}

// // printowanie dla latwiejszego debugowania, zostawiam bo zdarza sie byc przydatne
// void print_ary(zbior_ary A) {
//   printf("ary: %d\n", A.ary);
//   for (unsigned i = 0; i < A.ary; i++) {
//     printf("%d %d\n", A.poczatek[i], A.koniec[i]);
//   }
// }

zbior_ary ciag_pusty(void) {
  zbior_ary z = {.ary = 0, .poczatek = NULL, .koniec = NULL};
  return z;
}

// kopiuje nam wejsciowy zbior alokujac nowa pamiec
zbior_ary kopia_ary(zbior_ary A) {
  if (A.ary == 0) return ciag_pusty();
  int* poczatek = malloc((size_t)A.ary * sizeof(int));
  int* koniec = malloc((size_t)A.ary * sizeof(int));
  check_alloc(poczatek);
  check_alloc(koniec);
  for (unsigned i = 0; i < A.ary; i++) {
    poczatek[i] = A.poczatek[i];
    koniec[i] = A.koniec[i];
  }
  zbior_ary z = {.ary = A.ary, .poczatek = poczatek, .koniec = koniec};
  return z;
}

// Funkcja daje w wyniku zbior reprezentujacy ciag arytmetyczny o elemencie
// poczatkowym a, końcowym b i roznicy q>0, tj. {a,a+q,...,b}. Mozesz zalozyc,
// ze we wszystkich zapytaniach w danym tescie wartosc q bedzie taka sama.
// Mozesz zalozyc, ze ta funkcja zostanie wywolana jako pierwsza.
zbior_ary ciag_arytmetyczny(int a, int q, int b) {
  if (q <= 0) {
    fprintf(stderr, "blad: roznica ciagu musi byc dodatnia");
    exit(1);
  }
  q_globalne = q;
  int* poczatek = malloc(sizeof (int));
  int* koniec = malloc(sizeof (int));
  check_alloc(poczatek);
  check_alloc(koniec);
  poczatek[0] = a;
  koniec[0] = b;
  zbior_ary z = {.ary = 1, .poczatek = poczatek, .koniec = koniec};
  return z;
}

// Daje w wyniku zbior zlozony z samego elementu a, tj. {a}.
zbior_ary singleton(int a) { return ciag_arytmetyczny(a, q_globalne, a); }

// tworzy nam podzbior A gdzie elementy maja konkretna reszte modulo q_globalne
// w zbiorze {0,1,...,q-1}
zbior_ary separuj(int r, zbior_ary A) {
  if (A.ary == 0) return ciag_pusty();
  // szukanie start bisekcja: najmniejszego elementu z modulo >= r
  unsigned lewy = 0, prawy = A.ary - 1;
  while (lewy < prawy) {
    unsigned srodek = (lewy + prawy) / 2;
    if (mod(A.poczatek[srodek], q_globalne) >= r)
      prawy = srodek;
    else
      lewy = srodek + 1;
  }
  unsigned start = lewy;
  // sprawdzenie czy ten podzbior nie zawiera startu (jest pusty)
  if (start == A.ary || mod(A.poczatek[start], q_globalne) != r)
    return ciag_pusty();
  // szukanie finish bisekcja: najmniejszego elementu z modulo > r 
  lewy = 0, prawy = A.ary - 1;
  unsigned finish = A.ary;
  while (lewy <= prawy) {
    unsigned srodek = (lewy + prawy) / 2;
    if (mod(A.poczatek[srodek], q_globalne) <= r)
      lewy = srodek + 1;
    else {
      finish = srodek;
      prawy = srodek - 1;
    }
  }
  assert(finish > start);
  unsigned ary = finish - start;
  assert(ary != 0);
  // alokowanie nowej wyseparowanej tablicy
  int* poczatek = malloc((size_t)ary * sizeof(int));
  int* koniec = malloc((size_t)ary * sizeof(int));
  check_alloc(poczatek);
  check_alloc(koniec);
  for (unsigned i = 0; i < ary; i++) {
    poczatek[i] = A.poczatek[start + i];
    koniec[i] = A.koniec[start + i];
  }
  zbior_ary z = {.ary = ary, .poczatek = poczatek, .koniec = koniec};
  return z;
}
// kopiuje zawartosc danego zbioru do wynik_rzeczy, przydaje sie w scal_suma i scal_roznica
void scal_kopia(unsigned *wynik_ary, int *wynik_poczatek, int *wynik_koniec, zbior_ary A) {
  for (unsigned ary = 0; ary < A.ary; ary++) {
    wynik_poczatek[ary] = A.poczatek[ary];
    wynik_koniec[ary] = A.koniec[ary];
    (*wynik_ary)++;
  }
  return;
}

// dla dwoch wyseparowanych zbiorow tworzy sume i doklada ja do wynik_rzeczy
void scal_suma(unsigned* wynik_ary, int* wynik_poczatek, int* wynik_koniec,
               zbior_ary A, zbior_ary B) {
  unsigned ary = 0;
  if (A.ary == 0 && B.ary == 0) return;
  if (A.ary == 0) {
    scal_kopia(wynik_ary, wynik_poczatek, wynik_koniec, B);
    return;
  }
  if (B.ary == 0) {
    scal_kopia(wynik_ary, wynik_poczatek, wynik_koniec, A);
    return;
  }
  unsigned i = 0, j = 0;
  // ustawiamy pierwszy analizowany przedzial na ten o najmniejszym poczatku korzystajac z niezmiennikow zbioru
  int poczatek, koniec;
  if (A.poczatek[0] <= B.poczatek[0]) {
    poczatek = A.poczatek[i];
    koniec = A.koniec[i];
    i++;
  } else {
    poczatek = B.poczatek[j];
    koniec = B.koniec[j];
    j++;
  }
  while (i < A.ary || j < B.ary) {
    int nastepny_poczatek, nastepny_koniec;
    //ustawiamy na nastepne krance ten z dostepnych, ktorego poczatek jest mniejszy
    if (j >= B.ary || (i < A.ary && A.poczatek[i] <= B.poczatek[j])) {
      nastepny_poczatek = A.poczatek[i];
      nastepny_koniec = A.koniec[i];
      i++;
    } else {
      nastepny_poczatek = B.poczatek[j];
      nastepny_koniec = B.koniec[j];
      j++;
    }
    // badamy czy sie przecinaja i odpowiednio zwiekszamy przedzial, 
    // albo dokladamy poprzednie krance do wyniku 
    // i w nastepnym obrocie petli korzystamy juz z tych nowych 
    // uzywamy long longa zeby limity nie wywalily
    if (nastepny_poczatek <= (long long)koniec + q_globalne) {
      if (nastepny_koniec > koniec) koniec = nastepny_koniec;
    } else {
      wynik_poczatek[ary] = poczatek;
      wynik_koniec[ary] = koniec;
      ary++;
      poczatek = nastepny_poczatek;
      koniec = nastepny_koniec;
    }
  }
  // jeszcze trzeba dodac ostatni
  wynik_poczatek[ary] = poczatek;
  wynik_koniec[ary] = koniec;
  ary++;
  *wynik_ary += ary;
  return;
}

// Daje w wyniku zbior reprezentujacy teoriomnogosciowa sume zbiorow A i B.
zbior_ary suma(zbior_ary A, zbior_ary B) {
  if (A.ary == 0 && B.ary == 0) return ciag_pusty();
  if (A.ary == 0) {
    return kopia_ary(B);
  }
  if (B.ary == 0) {
    return kopia_ary(A);
  }
  // badamy czy rozmiar wyjdzie poza limity unsigned inta
  unsigned rozmiar = A.ary + B.ary;
  if (UINT_MAX - A.ary < B.ary) rozmiar = UINT_MAX;
  int* poczatek_bazowy = malloc((size_t)rozmiar * sizeof(int));
  int* koniec_bazowy = malloc((size_t)rozmiar * sizeof(int));
  check_alloc(poczatek_bazowy);
  check_alloc(koniec_bazowy);
  int *poczatek = poczatek_bazowy, *koniec = koniec_bazowy;

  unsigned ary = 0;
  unsigned i = 0, j = 0;
  // chcemy przejsc po wszystkich resztach modulo q_globalne 
  // wystepujacych w zbiorach A i B,
  // po kazdej dokladnie raz i w kolejnosci rosnacej
  while (i < A.ary || j < B.ary) {
    int r;
    if (i < A.ary && j < B.ary)
      r = min(mod(A.poczatek[i], q_globalne), mod(B.poczatek[j], q_globalne));
    else if (i < A.ary)
      r = mod(A.poczatek[i], q_globalne);
    else
      r = mod(B.poczatek[j], q_globalne);
    while (i < A.ary && mod(A.poczatek[i], q_globalne) == r) i++;
    while (j < B.ary && mod(B.poczatek[j], q_globalne) == r) j++;
    // jak juz mamy reszte to separujemy wzgledem jej A i B, potem scalamy 
    unsigned stare_ary = ary;
    zbior_ary A_sep = separuj(r, A);
    zbior_ary B_sep = separuj(r, B);
    scal_suma(&ary, poczatek, koniec, A_sep, B_sep);
    poczatek += ary - stare_ary;
    koniec += ary - stare_ary;
    //zwalniamy pomocnicze zmienne
    free_ary(&A_sep);
    free_ary(&B_sep);
  }

  if (ary == 0) {
    free(poczatek_bazowy);
    free(koniec_bazowy);
    return ciag_pusty();
  }  
  // ary <= A.ary + B.ary, wiec realloc obetnie koncowke zaalokowanej wczesniej
  // pamieci, trzeba tak ifowac gdyby zdarzyly sie problemy z alokacja
  int* tmp = realloc(poczatek_bazowy, (size_t)ary * sizeof(int));
  if (tmp == NULL) {
    free(poczatek_bazowy);
    free(koniec_bazowy);
    exit(1);
  }
  poczatek_bazowy = tmp;
  tmp = realloc(koniec_bazowy, (size_t)ary * sizeof(int));
  if (tmp == NULL) {
    free(poczatek_bazowy);
    free(koniec_bazowy);
    exit(1);
  }
  koniec_bazowy = tmp;
  zbior_ary z = {
      .ary = ary, .poczatek = poczatek_bazowy, .koniec = koniec_bazowy};
  return z;
}

// Daje w wyniku zbior reprezentujacy czesc wspolna zbiorow A i B.
zbior_ary iloczyn(zbior_ary A, zbior_ary B) {
  if (A.ary == 0 || B.ary == 0) return ciag_pusty();
  // korzystamy z wzorow teoriomnogosciowych: A n B = A \ (A \ B)
  zbior_ary C = roznica(A, B);
  zbior_ary D = roznica(A, C);
  // musimy tak zeby wskazniki wewnatrz C nie wyciekly
  free_ary(&C);  
  return D;
}
// dla dwoch wyseparowanych zbiorow tworzy roznice i doklada ja do wynik_rzeczy
void scal_roznica(unsigned* wynik_ary, int* wynik_poczatek, int* wynik_koniec,
                  zbior_ary A, zbior_ary B) {
  if (A.ary == 0) return;
  if (B.ary == 0) {
    scal_kopia(wynik_ary, wynik_poczatek, wynik_koniec, A);
    return;
  }
  unsigned i = 0, j = 0;
  unsigned ary = 0;
  int poczatek = A.poczatek[0], koniec = A.koniec[0];
  // poczatek i koniec to analizowany podciag z A
  // bierzemy podciag z B, moze on byc wzgledem krancow z A:
  // - calkowicie na lewo, wtedy bierzemy kolejny podciag B
  // - troche na lewo troche wewnatrz, wtedy przesuwamy w prawo poczatek do B.koniec[j] + q
  // - na lewo i na prawo, wtedy bierzemy kolejny podciag z A
  // - calkowicie na prawo, wtedy dokladamy krance do wyniku i bierzemy nowe z A
  // - wewnatrz lub wewnatrz i troche w prawo, wtedy dokladamy do wyniku poczatek i koniec obciety do B.poczatek[j] - q,
  // potem ustawiamy krance na prawa czesc poprzednich krancow (jesli sa) i bierzemy kolejny podciag z B,
  // lub bierzemy kolejny podciag z A
  while (i < A.ary && j < B.ary) {
    if (B.poczatek[j] <= poczatek) {
      // calkowicie na lewo
      if (B.koniec[j] < poczatek)
        j++;
      // troche na lewo troche wewnatrz
      else if (B.koniec[j] >= poczatek && B.koniec[j] < koniec) {
        //nie wyjdzie poza limity, bo nierownosc jest ostra, czyli cos jeszcze jest na prawo
        poczatek = B.koniec[j] + q_globalne;
        j++;
      // na lewo i na prawo
      } else if (B.koniec[j] >= koniec) {
        i++;
        if (i < A.ary) poczatek = A.poczatek[i], koniec = A.koniec[i];
      }
    // calkowicie na prawo
    } else if (B.poczatek[j] > koniec) {
      wynik_poczatek[ary] = poczatek;
      wynik_koniec[ary] = koniec;
      i++, ary++;
      if (i < A.ary) {
        poczatek = A.poczatek[i];
        koniec = A.koniec[i];
      }
    // wewnatrz lub troche w prawo
    } else if (B.poczatek[j] > poczatek && B.poczatek[j] <= koniec) {
      wynik_poczatek[ary] = poczatek;
      wynik_koniec[ary] = B.poczatek[j] - q_globalne;
      ary++;
      // wewnatrz
      if (B.koniec[j] < koniec) {
        //nie wyjdzie poza limity, bo nierownosc jest ostra, czyli cos jeszcze jest na prawo
        poczatek = B.koniec[j] + q_globalne;
        j++;
      // wewnatrz i troche w prawo
      } else {
        i++;
        if (i < A.ary) {
          poczatek = A.poczatek[i];
          koniec = A.koniec[i];
        }
      }
    }
  }
  // jesli B sie skonczylo to dokladamy to co zostalo z A
  while (i < A.ary) {
    wynik_poczatek[ary] = poczatek;
    wynik_koniec[ary] = koniec;
    i++, ary++;
    if (i < A.ary) {
      poczatek = A.poczatek[i];
      koniec = A.koniec[i];
    }
  }
  *wynik_ary += ary;
  return;
}
// Daje w wyniku zbior A \ B.
zbior_ary roznica(zbior_ary A, zbior_ary B) {
  if (A.ary == 0) return ciag_pusty();
  if (B.ary == 0) return kopia_ary(A);
  // badamy czy rozmiar wyjdzie poza limity unsigned inta
  unsigned rozmiar = A.ary + B.ary;
  if (UINT_MAX - A.ary < B.ary) rozmiar = UINT_MAX;
  // maksymalna wielkosc roznicy to A.ary + B.ary, bo kazdy podciag z B moze
  // zwiekszyc ary wyniku o 1
  int* poczatek_bazowy = malloc((size_t)rozmiar * sizeof(int));
  int* koniec_bazowy = malloc((size_t)rozmiar * sizeof(int));
  check_alloc(poczatek_bazowy);
  check_alloc(koniec_bazowy);
  int *poczatek = poczatek_bazowy, *koniec = koniec_bazowy;
  unsigned ary = 0;
  // wystarczy ze przejdziemy po resztach modulo q_globalne wystepujacych w A
  // po kazdej dokladnie raz
  for (unsigned i = 0; i < A.ary;) {
    int r = mod(A.poczatek[i], q_globalne);
    while (i < A.ary && mod(A.poczatek[i], q_globalne) == r) i++;
    unsigned stare_ary = ary;
    zbior_ary A_sep = separuj(r, A);
    zbior_ary B_sep = separuj(r, B);
    scal_roznica(&ary, poczatek, koniec, A_sep, B_sep);
    poczatek += ary - stare_ary;
    koniec += ary - stare_ary;
    // dealokujemy wnetrze pomocniczych zbiorow
    free_ary(&A_sep);
    free_ary(&B_sep);
  }
  // ary <= A.ary + B.ary, wiec realloc obetnie koncowke zaalokowanej wczesniej pamieci
  if (ary == 0) {
    free(poczatek_bazowy);
    free(koniec_bazowy);
    return ciag_pusty();
  }
  // trzeba badac czy realloc sie udal
  int* tmp = realloc(poczatek_bazowy, (size_t)ary * sizeof(int));
  if (tmp == NULL) {
    free(poczatek_bazowy);
    free(koniec_bazowy);
    exit(1);
  }
  poczatek_bazowy = tmp;
  tmp = realloc(koniec_bazowy, (size_t)ary * sizeof(int));
  if (tmp == NULL) {
    free(poczatek_bazowy);
    free(koniec_bazowy);
    exit(1);
  }
  koniec_bazowy = tmp;
  zbior_ary z = {
      .ary = ary, .poczatek = poczatek_bazowy, .koniec = koniec_bazowy};
  return z;
}
// Daje w wyniku true wtw. gdy liczba b nalezy do zbioru A.
bool nalezy(zbior_ary A, int b) {
  if (A.ary == 0) return false;
  int r = mod(b, q_globalne);
  // chcemy znalezc poczatek i koniec indeksow takich, ze ich reszta modulo q to r
  // nazwiemy je start i finish bo kreatywnosc mi sie konczy
  // juz zeby limity inta za mocno nie przeszkadzaly to licze w long longach, duzej roznicy to nie robi

  // bisekcyjne szukanie najmniejszego elementu z modulo >= r
  long long lewy = 0, prawy = A.ary - 1;
  while (lewy < prawy) {
    long long srodek = (lewy + prawy) / 2;
    if (mod(A.poczatek[srodek], q_globalne) >= r)
      prawy = srodek;
    else
      lewy = srodek + 1;
  }
  long long start = lewy;

  // jesli start nie bedzie w A lub reszta start mod q nie bedzie rowna r,
  // to oznacza to ze w zbiorze nie ma zadnego elementu z reszta r
  if (start == A.ary || mod(A.poczatek[start], q_globalne) != r) return false;

  // bisekcyjne szukanie najmniejszego elementu z modulo > r
  lewy = 0, prawy = A.ary - 1;
  long long finish = A.ary;
  while (lewy <= prawy) {
    long long srodek = (lewy + prawy) / 2;
    if (mod(A.poczatek[srodek], q_globalne) <= r)
      lewy = srodek + 1;
    else {
      finish = srodek;
      prawy = srodek - 1;
    }
  }
  // teraz miedzy start i finish-1 bisekcyjnie szukamy najmniejszego poczatku ciagu >= b,
  // bo ciagi sa rozlaczne i posortowane rosnaco, wiec b moze byc tylko w jednym
  lewy = start, prawy = finish - 1;
  long long x = -1;  // wartosc nieosiagalna
  while (lewy <= prawy) {
    long long srodek = (lewy + prawy) / 2;
    if (A.poczatek[srodek] <= b) {
      x = srodek;
      lewy = srodek + 1;
    } else
      prawy = srodek - 1;
  }
  if (x == -1) return false;
  // b oraz x-ty ciag maja te sama reszte modulo q, 
  // wiec wystaczy sprawdzic czy b jest miedzy poczatkiem a koncem
  if (b >= A.poczatek[x] && b <= A.koniec[x]) return true;
  return false;
}

// Wynikiem funkcji jest liczba elementow w zbiorze A.
unsigned moc(zbior_ary A) {
  unsigned moc = 0;
  // liczymy liczbe elementow kazdego podciagu i potem sumujemy
  for (unsigned i = 0; i < A.ary; i++) {
    assert(A.poczatek[i] <= A.koniec[i]);
    unsigned rozmiar;
    // robimy takie triki zeby po drodze nie zdarzyl sie overflow,
    // z takimi warunkami zawsze miescimy sie w limitach inta
    if (A.poczatek[i] < 0 && A.koniec[i] >= 0)
      rozmiar = (unsigned)A.koniec[i] + (unsigned)(-(A.poczatek[i] + 1)) + 1;
    else
      rozmiar = (unsigned)(A.koniec[i] - A.poczatek[i]);
    moc += rozmiar / (unsigned)q_globalne + 1;
  }
  return moc;
}
// Wynikiem funkcji jest Ary_q(A), czyli minimalna liczba parami rozlacznych
// ciagow arytmetycznych o roznicy q, ktorych suma jest zbior A.
unsigned ary(zbior_ary A) { return A.ary; }