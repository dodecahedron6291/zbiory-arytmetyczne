//testy ocen2.c i ocen3.c sa od krzysia
#include <stdbool.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include "zbior_ary.h"

/*
 * Testy struktury zbior_ary.
 *
 * We wszystkich wywolaniach ciag_arytmetyczny roznica q = 5.
 *
 * Testujacy zaklada, ze zbior_ary jest typem obslugiwanym
 * przez funkcje zdefiniowane w zbior_ary.h.
 */

int main(void) {
    /* =========================================================
       1. PUSTY ZBIOR
       ========================================================= */

    zbior_ary pusty = roznica(
        ciag_arytmetyczny(0, 5, 20),
        ciag_arytmetyczny(0, 5, 20)
    );

    assert(moc(pusty) == 0);
    assert(ary(pusty) == 0);

    assert(nalezy(pusty, 0) == false);
    assert(nalezy(pusty, -1) == false);
    assert(nalezy(pusty, INT_MIN) == false);
    assert(nalezy(pusty, INT_MAX) == false);

    /* Operacje na pustym zbiorze. */

    zbior_ary A = ciag_arytmetyczny(0, 5, 10);
    zbior_ary wynik = suma(pusty, A);

    assert(moc(wynik) == 3);
    assert(ary(wynik) == 1);
    assert(nalezy(wynik, 0));
    assert(nalezy(wynik, 5));
    assert(nalezy(wynik, 10));

    wynik = suma(A, pusty);
    assert(moc(wynik) == 3);
    assert(ary(wynik) == 1);

    wynik = iloczyn(pusty, A);
    assert(moc(wynik) == 0);
    assert(ary(wynik) == 0);

    wynik = iloczyn(A, pusty);
    assert(moc(wynik) == 0);
    assert(ary(wynik) == 0);

    wynik = roznica(pusty, A);
    assert(moc(wynik) == 0);
    assert(ary(wynik) == 0);

    wynik = roznica(A, pusty);
    assert(moc(wynik) == 3);
    assert(ary(wynik) == 1);

    /* Suma pustego zbioru z pustym zbiorem. */

    wynik = suma(pusty, pusty);
    assert(moc(wynik) == 0);
    assert(ary(wynik) == 0);

    /* =========================================================
       2. SINGLETONY
       ========================================================= */

    zbior_ary s1 = singleton(42);
    zbior_ary s2 = singleton(42);
    zbior_ary s3 = singleton(-17);
    zbior_ary s4 = singleton(INT_MIN);
    zbior_ary s5 = singleton(INT_MAX);

    assert(moc(s1) == 1);
    assert(ary(s1) == 1);
    assert(nalezy(s1, 42));
    assert(!nalezy(s1, 41));
    assert(!nalezy(s1, 43));

    assert(moc(s3) == 1);
    assert(ary(s3) == 1);
    assert(nalezy(s3, -17));
    assert(!nalezy(s3, 17));

    assert(moc(s4) == 1);
    assert(ary(s4) == 1);
    assert(nalezy(s4, INT_MIN));

    assert(moc(s5) == 1);
    assert(ary(s5) == 1);
    assert(nalezy(s5, INT_MAX));

    /* Identyczne singletony. */

    wynik = suma(s1, s2);
    assert(moc(wynik) == 1);
    assert(ary(wynik) == 1);

    wynik = iloczyn(s1, s2);
    assert(moc(wynik) == 1);
    assert(ary(wynik) == 1);
    assert(nalezy(wynik, 42));

    wynik = roznica(s1, s2);
    assert(moc(wynik) == 0);
    assert(ary(wynik) == 0);

    /* Rozne singletony. */

    wynik = suma(s1, s3);
    assert(moc(wynik) == 2);
    assert(ary(wynik) == 2);

    wynik = iloczyn(s1, s3);
    assert(moc(wynik) == 0);
    assert(ary(wynik) == 0);

    wynik = roznica(s1, s3);
    assert(moc(wynik) == 1);
    assert(ary(wynik) == 1);
    assert(nalezy(wynik, 42));

    /* =========================================================
       3. CIAGI ARYTMETYCZNE
       ========================================================= */

    A = ciag_arytmetyczny(2, 5, 12);
    // A = {2, 7, 12}

    assert(moc(A) == 3);
    assert(ary(A) == 1);

    assert(nalezy(A, 2));
    assert(nalezy(A, 7));
    assert(nalezy(A, 12));

    assert(!nalezy(A, 1));
    assert(!nalezy(A, 3));
    assert(!nalezy(A, 6));
    assert(!nalezy(A, 8));
    assert(!nalezy(A, 11));
    assert(!nalezy(A, 13));

    /* Ciag jednoelementowy. */

    zbior_ary jeden = ciag_arytmetyczny(-3, 5, -3);

    assert(moc(jeden) == 1);
    assert(ary(jeden) == 1);
    assert(nalezy(jeden, -3));
    assert(!nalezy(jeden, 2));

    /* Ciag z liczb ujemnych. */

    zbior_ary ujemny = ciag_arytmetyczny(-13, 5, 2);
    // {-13, -8, -3, 2}

    assert(moc(ujemny) == 4);
    assert(ary(ujemny) == 1);

    assert(nalezy(ujemny, -13));
    assert(nalezy(ujemny, -8));
    assert(nalezy(ujemny, -3));
    assert(nalezy(ujemny, 2));

    assert(!nalezy(ujemny, -14));
    assert(!nalezy(ujemny, -12));
    assert(!nalezy(ujemny, -4));
    assert(!nalezy(ujemny, 3));

    /* =========================================================
       4. SUMA ZBIOROW
       ========================================================= */

    A = ciag_arytmetyczny(2, 5, 12);
    zbior_ary B = ciag_arytmetyczny(3, 5, 13);

    // A = {2, 7, 12}
    // B = {3, 8, 13}

    zbior_ary C = suma(A, B);
    // C = {2, 3, 7, 8, 12, 13}

    assert(moc(C) == 6);
    assert(ary(C) == 2);

    assert(nalezy(C, 2));
    assert(nalezy(C, 3));
    assert(nalezy(C, 7));
    assert(nalezy(C, 8));
    assert(nalezy(C, 12));
    assert(nalezy(C, 13));

    assert(!nalezy(C, 1));
    assert(!nalezy(C, 4));
    assert(!nalezy(C, 9));
    assert(!nalezy(C, 18));

    /* Suma zbioru z samym soba nie duplikuje elementow. */

    wynik = suma(C, C);
    assert(moc(wynik) == 6);
    assert(ary(wynik) == 2);

    /* Suma zbiorow rozlacznych. */

    wynik = suma(singleton(0), singleton(1));
    assert(moc(wynik) == 2);
    assert(ary(wynik) == 2);

    /* Suma zbiorow z czesciowym pokrywaniem. */

    A = ciag_arytmetyczny(0, 5, 10);
    B = ciag_arytmetyczny(5, 5, 15);
    // A = {0, 5, 10}
    // B = {5, 10, 15}

    wynik = suma(A, B);
    // {0, 5, 10, 15}

    assert(moc(wynik) == 4);
    assert(ary(wynik) == 1);

    /* Laczenie dwoch odcinkow tego samego ciagu. */

    A = ciag_arytmetyczny(0, 5, 10);
    B = ciag_arytmetyczny(15, 5, 25);

    wynik = suma(A, B);
    // {0, 5, 10, 15, 20, 25}

    assert(moc(wynik) == 6);
    assert(ary(wynik) == 1);

    /* Przerwa w ciagu powoduje powstanie dwoch fragmentow. */

    A = ciag_arytmetyczny(0, 5, 10);
    B = ciag_arytmetyczny(20, 5, 25);

    wynik = suma(A, B);
    // {0, 5, 10, 20, 25}

    assert(moc(wynik) == 5);
    assert(ary(wynik) == 2);

    /* Dodanie brakujacego elementu scala fragmenty. */

    wynik = suma(wynik, singleton(15));

    assert(moc(wynik) == 6);
    assert(ary(wynik) == 1);

    /* =========================================================
       5. ILOCZYN ZBIOROW
       ========================================================= */

    A = ciag_arytmetyczny(0, 5, 20);
    B = ciag_arytmetyczny(10, 5, 30);

    // A = {0, 5, 10, 15, 20}
    // B = {10, 15, 20, 25, 30}

    wynik = iloczyn(A, B);
    // {10, 15, 20}

    assert(moc(wynik) == 3);
    assert(ary(wynik) == 1);

    assert(nalezy(wynik, 10));
    assert(nalezy(wynik, 15));
    assert(nalezy(wynik, 20));

    assert(!nalezy(wynik, 0));
    assert(!nalezy(wynik, 5));
    assert(!nalezy(wynik, 25));
    assert(!nalezy(wynik, 30));

    /* Iloczyn zbiorow rozlacznych. */

    A = ciag_arytmetyczny(0, 5, 10);
    B = ciag_arytmetyczny(1, 5, 11);

    wynik = iloczyn(A, B);

    assert(moc(wynik) == 0);
    assert(ary(wynik) == 0);

    /* Jeden zbior zawiera sie w drugim. */

    A = ciag_arytmetyczny(0, 5, 30);
    B = ciag_arytmetyczny(10, 5, 20);

    wynik = iloczyn(A, B);

    assert(moc(wynik) == 3);
    assert(ary(wynik) == 1);
    assert(nalezy(wynik, 10));
    assert(nalezy(wynik, 15));
    assert(nalezy(wynik, 20));

    /* Iloczyn zbioru z samym soba. */

    wynik = iloczyn(A, A);

    assert(moc(wynik) == moc(A));
    assert(ary(wynik) == ary(A));

    /* Iloczyn singletonu ze zbiorem. */

    A = ciag_arytmetyczny(0, 5, 20);

    wynik = iloczyn(A, singleton(10));
    assert(moc(wynik) == 1);
    assert(ary(wynik) == 1);
    assert(nalezy(wynik, 10));

    wynik = iloczyn(A, singleton(11));
    assert(moc(wynik) == 0);
    assert(ary(wynik) == 0);

    /* =========================================================
       6. ROZNICA ZBIOROW
       ========================================================= */

    A = ciag_arytmetyczny(0, 5, 20);
    B = ciag_arytmetyczny(5, 5, 15);

    // A = {0, 5, 10, 15, 20}
    // B = {5, 10, 15}

    wynik = roznica(A, B);
    // {0, 20}

    assert(moc(wynik) == 2);
    assert(ary(wynik) == 2);

    assert(nalezy(wynik, 0));
    assert(nalezy(wynik, 20));

    assert(!nalezy(wynik, 5));
    assert(!nalezy(wynik, 10));
    assert(!nalezy(wynik, 15));

    /* Roznica zbioru z samym soba. */

    wynik = roznica(A, A);

    assert(moc(wynik) == 0);
    assert(ary(wynik) == 0);

    /* Odejmowanie pustego zbioru. */

    wynik = roznica(A, pusty);

    assert(moc(wynik) == moc(A));
    assert(ary(wynik) == ary(A));

    /* Odejmowanie zbioru wiekszego od mniejszego. */

    A = ciag_arytmetyczny(5, 5, 15);
    B = ciag_arytmetyczny(0, 5, 20);

    wynik = roznica(A, B);

    assert(moc(wynik) == 0);
    assert(ary(wynik) == 0);

    /* Usuwanie elementu ze srodka ciagu. */

    A = ciag_arytmetyczny(0, 5, 20);

    wynik = roznica(A, singleton(10));
    // {0, 5, 15, 20}

    assert(moc(wynik) == 4);
    assert(ary(wynik) == 2);

    assert(nalezy(wynik, 0));
    assert(nalezy(wynik, 5));
    assert(!nalezy(wynik, 10));
    assert(nalezy(wynik, 15));
    assert(nalezy(wynik, 20));

    /* Usuwanie poczatku ciagu. */

    wynik = roznica(A, ciag_arytmetyczny(0, 5, 10));
    // {15, 20}

    assert(moc(wynik) == 2);
    assert(ary(wynik) == 1);

    /* Usuwanie konca ciagu. */

    wynik = roznica(A, ciag_arytmetyczny(10, 5, 20));
    // {0, 5}

    assert(moc(wynik) == 2);
    assert(ary(wynik) == 1);

    /* =========================================================
       7. LICZBA FRAGMENTOW ARYTMETYCZNYCH
       ========================================================= */

    /*
     * q = 5.
     *
     * Zbior {1, 6, 11, 16} jest jednym ciagiem.
     */

    A = ciag_arytmetyczny(1, 5, 16);

    assert(moc(A) == 4);
    assert(ary(A) == 1);

    /*
     * Zbior {1, 2, 6, 7}:
     * {1, 6} oraz {2, 7}.
     */

    A = suma(
        ciag_arytmetyczny(1, 5, 6),
        ciag_arytmetyczny(2, 5, 7)
    );

    assert(moc(A) == 4);
    assert(ary(A) == 2);

    /*
     * Zbior {0, 10, 20}.
     * Kazdy element stanowi osobny fragment,
     * poniewaz brakuje elementow 5 i 15.
     */

    A = roznica(
        ciag_arytmetyczny(0, 5, 20),
        ciag_arytmetyczny(5, 5, 15)
    );

    assert(moc(A) == 2);
    assert(ary(A) == 2);

    /*
     * Zbior {0, 5, 15, 20}.
     * Dwa fragmenty: {0, 5} i {15, 20}.
     */

    A = roznica(
        ciag_arytmetyczny(0, 5, 20),
        singleton(10)
    );

    assert(moc(A) == 4);
    assert(ary(A) == 2);

    /*
     * Elementy o roznych resztach modulo q nie moga
     * nalezec do jednego ciagu o roznicy q.
     */

    A = suma(
        suma(singleton(0), singleton(1)),
        suma(singleton(2), singleton(3))
    );

    assert(moc(A) == 4);
    assert(ary(A) == 4);

    /* =========================================================
       8. UJEMNE LICZBY I GRANICE CIAGOW
       ========================================================= */

    A = ciag_arytmetyczny(-20, 5, 0);

    assert(moc(A) == 5);
    assert(ary(A) == 1);

    assert(nalezy(A, -20));
    assert(nalezy(A, -15));
    assert(nalezy(A, -10));
    assert(nalezy(A, -5));
    assert(nalezy(A, 0));

    assert(!nalezy(A, -21));
    assert(!nalezy(A, -19));
    assert(!nalezy(A, -1));
    assert(!nalezy(A, 1));

    /* Fragmenty po obu stronach zera. */

    A = suma(
        ciag_arytmetyczny(-10, 5, 0),
        ciag_arytmetyczny(5, 5, 15)
    );

    assert(moc(A) == 6);
    assert(ary(A) == 1);

    /* Singletony na granicach typu int. */

    A = suma(singleton(INT_MIN), singleton(INT_MAX));

    assert(moc(A) == 2);
    assert(ary(A) == 2);

    assert(nalezy(A, INT_MIN));
    assert(nalezy(A, INT_MAX));
    assert(!nalezy(A, 0));

    /* =========================================================
       9. TESTY LACZONE
       ========================================================= */

    A = ciag_arytmetyczny(2, 5, 22);
    // {2, 7, 12, 17, 22}

    B = suma(
        ciag_arytmetyczny(3, 5, 18),
        singleton(27)
    );
    // {3, 8, 13, 18, 27}

    C = suma(A, B);

    assert(moc(C) == 10);
    assert(ary(C) == 2);

    wynik = iloczyn(C, A);

    assert(moc(wynik) == 5);
    assert(ary(wynik) == 1);

    wynik = roznica(C, A);

    assert(moc(wynik) == 5);
    assert(ary(wynik) == 2);

    assert(nalezy(wynik, 3));
    assert(nalezy(wynik, 8));
    assert(nalezy(wynik, 13));
    assert(nalezy(wynik, 18));
    assert(nalezy(wynik, 27));

    assert(!nalezy(wynik, 2));
    assert(!nalezy(wynik, 7));
    assert(!nalezy(wynik, 12));
    assert(!nalezy(wynik, 17));
    assert(!nalezy(wynik, 22));

    /* Sprawdzenie tozsamosci: (A \ B) U (A ∩ B) = A. */

    zbior_ary czesc_wspolna = iloczyn(A, B);
    zbior_ary czesc_roznicy = roznica(A, B);

    wynik = suma(czesc_wspolna, czesc_roznicy);

    assert(moc(wynik) == moc(A));
    assert(ary(wynik) == ary(A));

    for (int x = 2; x <= 22; ++x) {
        assert(nalezy(wynik, x) == nalezy(A, x));
    }

    /* =========================================================
       10. DODATKOWE TESTY NIEZMIENNIKOW
       ========================================================= */

    A = ciag_arytmetyczny(0, 5, 50);
    B = ciag_arytmetyczny(10, 5, 40);

    /*
     * A zawiera 11 elementow, B zawiera 7 elementow,
     * a B jest podzbiorem A.
     */

    assert(moc(A) == 11);
    assert(moc(B) == 7);

    assert(moc(iloczyn(A, B)) == 7);
    assert(moc(roznica(A, B)) == 4);
    assert(moc(suma(A, B)) == 11);

    assert(ary(A) == 1);
    assert(ary(B) == 1);
    assert(ary(iloczyn(A, B)) == 1);

    /*
     * Suma zbiorow rozlacznych ma moc rowna sumie mocy.
     */

    A = ciag_arytmetyczny(0, 5, 10);
    B = ciag_arytmetyczny(1, 5, 11);

    assert(moc(iloczyn(A, B)) == 0);
    assert(moc(suma(A, B)) == moc(A) + moc(B));

    /*
     * Rownosc zbiorow sprawdzana przez wzajemne zawieranie.
     */

    A = suma(
        ciag_arytmetyczny(0, 5, 10),
        ciag_arytmetyczny(1, 5, 11)
    );

    B = suma(
        ciag_arytmetyczny(1, 5, 11),
        ciag_arytmetyczny(0, 5, 10)
    );

    assert(moc(A) == moc(B));
    assert(ary(A) == ary(B));

    for (int x = 0; x <= 11; ++x) {
        assert(nalezy(A, x) == nalezy(B, x));
    }

    printf("Wszystkie testy zakonczone powodzeniem.\n");

    return 0;
}
