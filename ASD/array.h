#ifndef ARRAY_H_INCLUDED
#define ARRAY_H_INCLUDED
#define P 3 // Кількість перерізів
#define M 64// Кількість рядків
#define N 64// Кількість стовпчиків
#define P_T 3//  Кількість перерізів для тесту сортування
#define NM_Test 6 //Кількість рядків і стовпчиків для тесту сортування
extern int ***Arr3D;
extern int *Vector;

void Creation(int p, int m,  int n);//Створення динамічного багатовимірного масива
void Fill_Arr3D_Sort(int p, int m,  int n);// Заповнення багатовимірного масиву для випадку "відсортовано"
void Fill_Arr3D_Rand(int p, int m,  int n);// Заповнення багатовимірного масиву для випадку "не впорядкований"
void Fill_Arr3D_Back (int p, int m,  int n);// Заповнення багатовимірного масиву для випадку "обернено-відсортовано"
void Out_Arr3D(int p, int m,  int n); // Виведення багатовимірного масива
void Creation_Vect(int n);//Створення динамічного вектора
void Fill_Vect_Sort(int n);// Заповнення вектора для випадку "відсортовано"
void Fill_Vect_Rand(int n);// Заповнення вектора для випадку "не впорядкований"
void Fill_Vect_Back(int n);// Заповнення вектора для випадку "обернено-відсортовано"
void Free(int p, int m);//Очищення багатовимірного динамічного масива

#endif // ARRAY_H_INCLUDED
