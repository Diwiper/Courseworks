#ifndef SORTING_H_INCLUDED
#define SORTING_H_INCLUDED
#include "Measurement.h"
clock_t Select2(int ***A, int p, int m, int n); //�������� ���������� �2 ������ ������� ������.
clock_t InsertExchange(int ***A, int p, int m, int n); //ó������� �������� "������� � ����".
clock_t Select7(int ***A, int p, int m, int n);//�������� ���������� �7 ������ ������� ������.

//////////////////////////���������� �������///////////////////////////

clock_t Select2_Vect(int *A, int n) ;//�������� ���������� �2 ������ ������� ������ ��� �������.
clock_t InsertExchange_Vect(int *A, int n); //ó������� �������� "������� � ����" ��� �������.
clock_t Select7_Vect(int *A, int n);//�������� ���������� �7 ������ ������� ������ ��� �������.

#endif // SORTING_H_INCLUDED
