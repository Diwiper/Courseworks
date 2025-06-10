#ifndef MEASUREMENT_H_INCLUDED
#define MEASUREMENT_H_INCLUDED

#include <time.h>

// Загальна кількість вимірів часу роботи алгоритма
#define measurements_number 28
// Кількість відкинутих початкових вимірів
#define rejected_number 2
#define min_max_number 3// Кількість відкинутих вимірів з мінімільними значеннями та з максимальними значеннями

extern clock_t Res[measurements_number];// Масив значень часу роботи алгоритма

void sort_type_Select2(int type);//заповнення масиву значень часу для сортування вибором №2
void sort_type_Insertion(int type);//заповнення масиву значень часу для сортування "обмін-вставка"
void sort_type_Select7(int type);//заповнення масиву значень часу для сортування вибором №7
void sort_type_Select2_Vect(int type);//заповнення вектора значень часу для сортування вибором №2
void sort_type_Insertion_Vect(int type);//заповнення вектора значень часу для сортування "обмін-вставка"
void sort_type_Select7_Vect(int type);//заповнення вектора значень часу для сортування вибором №7
float MeasurementProcessing();// Функція обробки і усереднення значень вимірів

#endif // MEASUREMENT_H_INCLUDED



