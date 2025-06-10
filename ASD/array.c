#include <stdio.h>
#include <stdlib.h>
#include "array.h"

int ***Arr3D;
int *Vector;

void Creation(int p, int m, int n)//Створення динамічного багатовимірного масива
{
    Arr3D = (int***) malloc(p*sizeof(int**));
    for (int k=0; k<p; k++)
    {
        Arr3D[k] = (int**) malloc(m*sizeof(int*));
        for (int i=0; i<m; i++)
            Arr3D[k][i] = (int*) malloc(n*sizeof(int));
    }
}

void Fill_Arr3D_Sort(int p, int m, int n)// Заповнення багатовимірного масиву для випадку "відсортовано"
{
    int number=0;
    for (int k=0; k<p; k++)
        for (int j=0; j<n; j++)
            for (int i=0; i<m; i++)
                Arr3D[k][i][j] = number++;
}

void Fill_Arr3D_Rand(int p, int m, int n)// Заповнення багатовимірного масиву для випадку "не впорядкований"
{
    for (int k=0; k<p; k++)
        for (int j=0; j<n; j++)
            for (int i=0; i<m; i++)
                Arr3D[k][i][j] = rand() % (p*m*n);
}

void Fill_Arr3D_Back(int p, int m, int n)// Заповнення багатовимірного масиву для випадку "обернено-відсортовано"
{
    int number = p*m*n;
    for (int k=0; k<p; k++)
        for (int j=0; j<n; j++)
            for (int i=0; i<m; i++)
                Arr3D[k][i][j]=number--;
}

void Out_Arr3D(int p, int m, int n) // Виведення багатовимірного масива
{

    for (int k=0; k<p; k++)
    {
        for (int i=0; i<m; i++)
        {
            for (int j=0; j<n; j++)
            {
                printf("%4d", Arr3D[k][i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

void Creation_Vect(int n)//Створення динамічного вектора
{
   Vector=(int*) malloc(n*sizeof(int));
}

void Fill_Vect_Sort(int n)// Заповнення вектора для випадку "відсортовано"
{
    int number = 0;
    for (int i=0; i<n; i++)
        Vector[i] = number++;
}

void Fill_Vect_Rand(int n)// Заповнення вектора для випадку "не впорядкований"
{
    for (int i=0; i<n; i++)
        Vector[i] = rand() % n;
}

void Fill_Vect_Back(int n)// Заповнення вектора для випадку "обернено-відсортовано"
{
    int number=n;
    for (int i=0; i<n; i++)
        Vector[i]=number--;
}

void Free(int p,  int m)//Очищення багатовимірного динамічного масива
{
    for (int k=0; k<p; k++)
    {
        for (int i=0; i<m; i++)
            free(Arr3D[k][i]);
        free(Arr3D[k]);
    }
    free(Arr3D);
}

