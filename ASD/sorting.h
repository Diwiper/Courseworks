#ifndef SORTING_H_INCLUDED
#define SORTING_H_INCLUDED
#include "Measurement.h"
clock_t Select2(int ***A, int p, int m, int n); //Алгоритм сортування №2 методу прямого вибору.
clock_t InsertExchange(int ***A, int p, int m, int n); //Гібридний алгоритм "вставка – обмін".
clock_t Select7(int ***A, int p, int m, int n);//Алгоритм сортування №7 методу прямого вибору.

//////////////////////////Сортування Векторів///////////////////////////

clock_t Select2_Vect(int *A, int n) ;//Алгоритм сортування №2 методу прямого вибору для вектора.
clock_t InsertExchange_Vect(int *A, int n); //Гібридний алгоритм "вставка – обмін" для вектора.
clock_t Select7_Vect(int *A, int n);//Алгоритм сортування №7 методу прямого вибору для вектора.

#endif // SORTING_H_INCLUDED
