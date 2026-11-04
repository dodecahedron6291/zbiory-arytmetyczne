#include "zbior_ary.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "zbior_ary.h"

// komentarz dotyczacy danej linijki jest nad nia lub na prawo od niej

static inline unsigned min(long long a, long long b) { return (unsigned)(a < b ? a : b); }
// zeby wynik modulo trafial w {0,1,...,q-1}
static inline unsigned mod(int a, int b) {
  assert(b > 0);
  int x = a % b;
  if (x < 0) x += b;
  return (unsigned)x; //i tak jest dodatni
}
static int q = 0;

// mallocuje podana wielkosc 
// i sprawdza czy dany wskaznik zostal po mallocu poprawnie zaalokowany, jesli nie
// to wywala program
static int* alokowanie(size_t size) {
  int *tmp = malloc(size);
  if (tmp == NULL) {
    fprintf(stderr, "blad w alokacji pamieci\n");
    exit(1);
  }
  return tmp;
}

// zwalnianie pamieci zaalokowanej przez wskazniki wewnatrz zbioru
static void free_ary(zbior_ary* Aptr) {
  if (Aptr == NULL) return;
  free(Aptr->poczatek);
  free(Aptr->koniec);
  Aptr->poczatek = NULL;
  Aptr->koniec = NULL;
  Aptr->ary = 0;
}

// generuje nam ciag pusty z ary == 0
static zbior_ary ciag_pusty(void) {
  zbior_ary z = {.ary = 0, .poczatek = NULL, .koniec = NULL};
  return z;
}

// kopiuje nam wejsciowy zbior alokujac nowa pamiec
static zbior_ary kopia_ary(zbior_ary A) {
  if (A.ary == 0) return ciag_pusty();
  int *poczatek = alokowanie(A.ary * sizeof(int));
  int *koniec = alokowanie(A.ary * sizeof(int)); 
  memcpy(poczatek, A.poczatek, A.ary * sizeof(int));
  memcpy(koniec, A.koniec, A.ary * sizeof(int));
  zbior_ary z = {.ary = A.ary, .poczatek = poczatek, .koniec = koniec};
  return z;
}

// Funkcja daje w wyniku zbior reprezentujacy ciag arytmetyczny o elemencie
// poczatkowym a, końcowym b i roznicy q>0, tj. {a,a+q,...,b}. Mozesz zalozyc,
// ze we wszystkich zapytaniach w danym tescie wartosc q bedzie taka sama.
// Mozesz zalozyc, ze ta funkcja zostanie wywolana jako pierwsza.
zbior_ary ciag_arytmetyczny(int a, int q_, int b) {
  assert(q_ > 0);
  if (q == 0) q = q_;
  assert(q_ == q);
  int *poczatek = alokowanie(sizeof (int));
  int *koniec = alokowanie(sizeof (int));
  poczatek[0] = a;
  koniec[0] = b;
  zbior_ary z = {.ary = 1, .poczatek = poczatek, .koniec = koniec};
  return z;
}

// Daje w wyniku zbior zlozony z samego elementu a, tj. {a}.
zbior_ary singleton(int a) { 
  assert(q != 0);
  return ciag_arytmetyczny(a, q, a); 
}

// kopiuje zawartosc danego zbioru do danych wskaznikow (modyfikuje), przydaje sie w scal_suma i scal_roznica
static void scal_kopia(unsigned *ary_wynik, int *poczatek_tab, int *koniec_tab, zbior_ary A) {
  memcpy(poczatek_tab, A.poczatek, A.ary * sizeof(int));
  memcpy(koniec_tab, A.koniec, A.ary * sizeof(int));
  (*ary_wynik) += A.ary;
  return;
}

// suma i roznica maja ten sam schemat i roznia sie tylko uzyta funkcja scalania
static zbior_ary wzor_sumy_i_roznicy(void (*scal_funkcja)(
          unsigned*, int*, int*, zbior_ary, zbior_ary),
          zbior_ary A, zbior_ary B) {
  unsigned rozmiar = min((long long)A.ary + B.ary, UINT_MAX);
  if (rozmiar == 0) return ciag_pusty();
  int *poczatek_bazowy = alokowanie((rozmiar) * sizeof(int));
  int *koniec_bazowy = alokowanie((rozmiar) * sizeof(int));
  int *poczatek = poczatek_bazowy, *koniec = koniec_bazowy;
  unsigned ary = 0;
  unsigned i_A= 0, i_B = 0;
  while (i_A < A.ary ||  i_B < B.ary) {
    unsigned i_A_stare = i_A, i_B_stare = i_B;
    unsigned ary_stary = ary;
    unsigned reszta;
    if (i_A < A.ary && i_B < B.ary)
      reszta = min(mod(A.poczatek[i_A], q), mod(B.poczatek[i_B], q));
    else if (i_A < A.ary)
      reszta = mod(A.poczatek[i_A], q);
    else 
      reszta = mod(B.poczatek[i_B], q);
    while (i_A < A.ary && mod(A.poczatek[i_A], q) == reszta) i_A++;
    while (i_B < B.ary && mod(B.poczatek[i_B], q) == reszta) i_B++;
    // wynikowe zbiory beda wyseparowane (stala reszta mod q i ta sama w obu!!)
    zbior_ary A_sep = { .ary = i_A - i_A_stare, .poczatek = A.poczatek + i_A_stare, .koniec = A.koniec + i_A_stare};
    zbior_ary B_sep = { .ary = i_B - i_B_stare, .poczatek = B.poczatek + i_B_stare, .koniec = B.koniec + i_B_stare};
    scal_funkcja(&ary, poczatek, koniec, A_sep, B_sep);
    poczatek += ary - ary_stary;
    koniec += ary - ary_stary;
  }
  if (ary == 0) {
    free(poczatek_bazowy);
    free(koniec_bazowy);
    return ciag_pusty();
  }  
  zbior_ary Z = kopia_ary((zbior_ary){ .ary = ary, .poczatek = poczatek_bazowy, .koniec = koniec_bazowy});
  free(poczatek_bazowy);
  free(koniec_bazowy);
  return Z;
}

// zbior wyseparowany to taki, ktory ma dla kazdego podciagu te sama reszte modulo q
// dla dwoch wyseparowanych z ta sama reszta zbiorow tworzy sume i doklada ja do danych wskaznikow (modyfikuje)
static void scal_suma(unsigned *ary_wynik, int *poczatek_tab, int *koniec_tab,
               zbior_ary A, zbior_ary B) {
  unsigned ary = 0;
  if (A.ary == 0 && B.ary == 0) return;
  if (A.ary == 0) {
    scal_kopia(ary_wynik, poczatek_tab, koniec_tab, B);
    return;
  }
  if (B.ary == 0) {
    scal_kopia(ary_wynik, poczatek_tab, koniec_tab, A);
    return;
  }
  unsigned i_A = 0, i_B = 0;
  // ustawiamy pierwszy analizowany przedzial na ten z mniejszym poczatkiem
  int poczatek, koniec;
  if (A.poczatek[0] <= B.poczatek[0]) {
    poczatek = A.poczatek[i_A];
    koniec = A.koniec[i_A];
    i_A++;
  } else {
    poczatek = B.poczatek[i_B];
    koniec = B.koniec[i_B];
    i_B++;
  }
  while (i_A < A.ary || i_B < B.ary) {
    int nastepny_poczatek, nastepny_koniec;
    //ustawiamy na nastepne krance ten z dostepnych, ktorego poczatek jest mniejszy
    if (i_B == B.ary || (i_A < A.ary && A.poczatek[i_A] <= B.poczatek[i_B])) {
      nastepny_poczatek = A.poczatek[i_A];
      nastepny_koniec = A.koniec[i_A];
      i_A++;
    } else {
      nastepny_poczatek = B.poczatek[i_B];
      nastepny_koniec = B.koniec[i_B];
      i_B++;
    }
    // badamy czy sie przecinaja i odpowiednio zwiekszamy przedzial, 
    // albo jezeli sie nie przecinaja to dokladamy poprzednie krance do wyniku 
    // i w nastepnym obrocie petli korzystamy juz z tych nowych;
    // uzywamy long longa zeby limity nie wywalily
    if (nastepny_poczatek <= (long long)koniec + q) {
      if (nastepny_koniec > koniec) koniec = nastepny_koniec;
    } else {
      poczatek_tab[ary] = poczatek;
      koniec_tab[ary] = koniec;
      ary++;
      poczatek = nastepny_poczatek;
      koniec = nastepny_koniec;
    }
  }
  // jeszcze trzeba dodac ostatni
  poczatek_tab[ary] = poczatek;
  koniec_tab[ary] = koniec;
  ary++;
  *ary_wynik += ary;
  return;
}

// dla dwoch wyseparowanych (o tej samej reszcie) zbiorow tworzy roznice i doklada ja do podanych wskaznikow
static void scal_roznica(unsigned *ary_wynik, int *poczatek_tab, int *koniec_tab,
                  zbior_ary A, zbior_ary B) {
  if (A.ary == 0) return;
  if (B.ary == 0) {
    scal_kopia(ary_wynik, poczatek_tab, koniec_tab, A);
    return;
  }
  unsigned i_A = 0, i_B = 0;
  unsigned ary = 0;
  // poczatek i koniec beda krancami przedzialu z A ktory aktualnie bierzemy na warsztat,
  // jak juz bedziemy mieli pewnosc ze nie bedzie modyfikowany to dokladamy go do wynikowych tablic
  // jak ponizej mamy +- q to nie wyjdzie poza limity inta bo odpowiednie nierownosci sa ostre
  // ponizsze warunki sa wyczerpujace, wiec petla sie nie zapetli
  int poczatek = A.poczatek[0], koniec = A.koniec[0];
  while (i_A < A.ary && i_B < B.ary) {
    // jezeli przedzial z B zawiera caly przedzial z A, to caly go odejmie, 
    // wiec bierzemy nastepny z A
    if (B.poczatek[i_B] <= poczatek && B.koniec[i_B] >= koniec) {
      i_A++;
      if (i_A < A.ary) {
        poczatek = A.poczatek[i_A];
        koniec = A.koniec[i_A];
      }
    } // jezeli przedzial z B jest scisle wewnatrz przedzialu z A, 
      // to mozemy dolozyc do tablicy wyniku ten o mniejszym poczatku, 
      // bo nie moze on juz byc modyfikowany przez nic, co bedzie w B
    else if (B.poczatek[i_B] > poczatek && B.koniec[i_B] < koniec) {
      poczatek_tab[ary] = poczatek;
      koniec_tab[ary] = B.poczatek[i_B] - q;
      poczatek = B.koniec[i_B] + q;
      ary++;
      i_B++;
    } // jesli sa rozlaczne i ten z B ma mniejszy poczatek, to 
      // ten juz nic nie zdziala i musimy rozpatrzec kolejny podciag B
    else if (B.koniec[i_B] < poczatek) i_B++;
    // jezeli sa rozlaczne i ten z A ma mniejszy poczatek, 
    // to juz nie moze byc nigdy odjety, wiec dodajemy krance do wyniku
    else if (B.poczatek[i_B] > koniec) {
      poczatek_tab[ary] = poczatek;
      koniec_tab[ary] = koniec;
      ary++;
      i_A++;
      if (i_A < A.ary) {
        poczatek = A.poczatek[i_A];
        koniec = A.koniec[i_A];
      }
    } // jezeli przecinaja sie wlasciwie i przedzial z A ma mniejszy poczatek, to 
      // dodajemy to nie odjete do wyniku i bierzemy na warsztat koleny przedzial z A
    else if (B.poczatek[i_B] > poczatek && B.poczatek[i_B] <= koniec && B.koniec[i_B] >= koniec) {
      poczatek_tab[ary] = poczatek;
      koniec_tab[ary] = B.poczatek[i_B] - q;
      ary++;
      i_A++;
      if (i_A < A.ary) {
        poczatek = A.poczatek[i_A];
        koniec = A.koniec[i_A];
      }
    } // jezeli przecinaja sie wlasciwie i przedzial z A ma wiekszy poczatek, to 
    // odejmujemy czesc wspolna, czyli przesuwamy poczatek przedzialu z A, i bierzemy kolejny przedzial z B
    else if (B.poczatek[i_B] <= poczatek && B.koniec[i_B] >= poczatek && B.koniec[i_B] < koniec) {
      poczatek = B.koniec[i_B] + q;
      i_B++;
    }
  }
  // jesli B sie skonczylo to dokladamy to co zostalo z A
  while (i_A < A.ary) {
    poczatek_tab[ary] = poczatek;
    koniec_tab[ary] = koniec;
    i_A++, ary++;
    if (i_A < A.ary) {
      poczatek = A.poczatek[i_A];
      koniec = A.koniec[i_A];
    }
  }
  *ary_wynik += ary;
  return;
}

zbior_ary suma(zbior_ary A, zbior_ary B) {
  return wzor_sumy_i_roznicy(scal_suma, A, B);
}

// Daje w wyniku zbior A \ B.
zbior_ary roznica(zbior_ary A, zbior_ary B) {
  return wzor_sumy_i_roznicy(scal_roznica, A, B);
}

// Daje w wyniku zbior reprezentujacy czesc wspolna zbiorow A i B.
zbior_ary iloczyn(zbior_ary A, zbior_ary B) {
  // korzystamy z wzorow teoriomnogosciowych: A n B = A \ (A \ B)
  zbior_ary C = roznica(A, B);
  zbior_ary D = roznica(A, C);
  // musimy tak zeby wskazniki wewnatrz C nie wyciekly
  free_ary(&C);  
  return D;
}

// Daje w wyniku true wtw. gdy liczba b nalezy do zbioru A.
bool nalezy(zbior_ary A, int b) {
  if (A.ary == 0) return false;
  unsigned reszta = mod(b, q);
  unsigned lewy = 0, prawy = A.ary - 1;
  while (lewy < prawy) {
    unsigned srodek = (lewy + prawy) / 2;
    if ( mod(A.poczatek[srodek], q) > reszta ||
        (reszta == mod(A.poczatek[srodek], q) &&
        b <= A.koniec[srodek])) {
      prawy = srodek;
    } else {
      lewy = srodek + 1;
    }
  }
  if (mod(A.poczatek[lewy],q) == reszta && b >= A.poczatek[lewy] && b <= A.koniec[lewy]) return true;
  return false;
}

// Wynikiem funkcji jest liczba elementow w zbiorze A.
unsigned moc(zbior_ary A) {
  unsigned moc = 0;
  // liczymy liczbe elementow kazdego podciagu i potem sumujemy
  for (unsigned i = 0; i < A.ary; i++) {
    assert(A.poczatek[i] <= A.koniec[i]);
    unsigned rozmiar = (unsigned)((long long)A.koniec[i] - A.poczatek[i]);
    moc += rozmiar / (unsigned)q + 1;
  }
  return moc;
}

// Wynikiem funkcji jest Ary_q(A), czyli minimalna liczba parami rozlacznych
// ciagow arytmetycznych o roznicy q, ktorych suma jest zbior A.
unsigned ary(zbior_ary A) { return A.ary; }