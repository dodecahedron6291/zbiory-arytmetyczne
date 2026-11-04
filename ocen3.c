

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include "zbior_ary.h"

int main(void) {
    const int q = INT_MAX;

    // Ciąg złożony z trzech elementów:
    // INT_MIN, -1, INT_MAX - 1.
    zbior_ary A = ciag_arytmetyczny(INT_MIN, q, INT_MAX - 1);
    assert(moc(A) == 3);
    assert(ary(A) == 1);
    assert(nalezy(A, INT_MIN));
    assert(nalezy(A, -1));
    assert(nalezy(A, INT_MAX - 1));
    assert(!nalezy(A, INT_MAX));

    // Dwa ciągi, które mają wspólny element -1.
    zbior_ary B = ciag_arytmetyczny(INT_MIN, q, -1);
    zbior_ary C = ciag_arytmetyczny(-1, q, INT_MAX - 1);

    assert(moc(B) == 2);
    assert(ary(B) == 1);
    assert(moc(C) == 2);
    assert(ary(C) == 1);

    // Suma identycznych zbiorów.
    zbior_ary D = suma(B, B);
    assert(moc(D) == 2);
    assert(ary(D) == 1);

    // Suma ciągów łączy się w jeden dłuższy ciąg.
    zbior_ary E = suma(B, C);
    assert(moc(E) == 3);
    assert(ary(E) == 1);
    assert(nalezy(E, INT_MIN));
    assert(nalezy(E, -1));
    assert(nalezy(E, INT_MAX - 1));

    // Część wspólna.
    zbior_ary F = iloczyn(B, C);
    assert(moc(F) == 1);
    assert(ary(F) == 1);
    assert(nalezy(F, -1));
    assert(!nalezy(F, INT_MIN));
    assert(!nalezy(F, INT_MAX - 1));

    // Różnica zbiorów.
    zbior_ary G = roznica(B, C);
    assert(moc(G) == 1);
    assert(ary(G) == 1);
    assert(nalezy(G, INT_MIN));
    assert(!nalezy(G, -1));

    zbior_ary H = roznica(C, B);
    assert(moc(H) == 1);
    assert(ary(H) == 1);
    assert(nalezy(H, INT_MAX - 1));
    assert(!nalezy(H, -1));

    // Zbiory identyczne: różnica i iloczyn.
    zbior_ary I = roznica(A, A);
    assert(moc(I) == 0);
    assert(ary(I) == 0);
    assert(!nalezy(I, INT_MIN));

    zbior_ary J = iloczyn(A, A);
    assert(moc(J) == 3);
    assert(ary(J) == 1);

    // Pusty zbiór jako argument wszystkich operacji.
    zbior_ary K = suma(I, I);
    assert(moc(K) == 0);
    assert(ary(K) == 0);

    zbior_ary L = iloczyn(I, A);
    assert(moc(L) == 0);
    assert(ary(L) == 0);

    zbior_ary M = roznica(I, A);
    assert(moc(M) == 0);
    assert(ary(M) == 0);

    zbior_ary N = roznica(A, I);
    assert(moc(N) == 3);
    assert(ary(N) == 1);

    // Singletony na skrajnych wartościach typu int.
    zbior_ary S1 = singleton(INT_MIN);
    zbior_ary S2 = singleton(INT_MAX);

    assert(moc(S1) == 1);
    assert(ary(S1) == 1);
    assert(nalezy(S1, INT_MIN));
    assert(!nalezy(S1, INT_MAX));

    assert(moc(S2) == 1);
    assert(ary(S2) == 1);
    assert(nalezy(S2, INT_MAX));
    assert(!nalezy(S2, INT_MIN));

    zbior_ary O = suma(S1, S2);
    assert(moc(O) == 2);
    assert(ary(O) == 2);

    zbior_ary P = iloczyn(S1, S2);
    assert(moc(P) == 0);
    assert(ary(P) == 0);

    zbior_ary R = roznica(S1, S2);
    assert(moc(R) == 1);
    assert(ary(R) == 1);
    assert(nalezy(R, INT_MIN));

    // Singleton już należący do zbioru.
    zbior_ary T = suma(A, singleton(-1));
    assert(moc(T) == 3);
    assert(ary(T) == 1);

    // Usunięcie końcowego elementu ciągu.
    zbior_ary U = roznica(A, singleton(INT_MAX - 1));
    assert(moc(U) == 2);
    assert(ary(U) == 1);
    assert(nalezy(U, INT_MIN));
    assert(nalezy(U, -1));
    assert(!nalezy(U, INT_MAX - 1));

    puts("test_max: OK");
    return 0;
}
