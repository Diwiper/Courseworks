#include "Measurement.h"
#include "sorting.h"
#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>


clock_t Res[measurements_number];// Масив значень часу роботи алгоритма

void sort_type_Select2(int type)//заповнення масиву значень часу для сортування вибором №2
{
    for(int i=0; i<measurements_number; i++)
    {
        Creation(P, M, N);
        switch(type)
        {
        case 1:
            Fill_Arr3D_Sort(P, M, N);
            break;
        case 2:
            Fill_Arr3D_Rand(P, M, N);
            break;
        case 3:
            Fill_Arr3D_Back(P, M, N);
            break;
        default:
            break;
        }
        Res[i] = Select2(Arr3D, P, M, N);
        Free(P,M);
    }
}

void sort_type_Insertion(int type)//заповнення масиву значень часу для сортування "обмін-вставка"
{
    for(int i=0; i<measurements_number; i++)
    {
        Creation(P, M, N);
        switch(type)
        {
        case 1:
            Fill_Arr3D_Sort(P, M, N);
            break;
        case 2:
            Fill_Arr3D_Rand(P, M, N);
            break;
        case 3:
            Fill_Arr3D_Back(P, M, N);
            break;
        default:
            break;
        }

        Res[i] = InsertExchange(Arr3D, P, M, N);
        Free(P,M);
    }
}

void sort_type_Select7(int type)//заповнення масиву значень часу для сортування вибором №7
{
    for(int i=0; i<measurements_number; i++)
    {
        Creation(P, M, N);
        switch(type)
        {
        case 1:
            Fill_Arr3D_Sort(P, M, N);
            break;
        case 2:
            Fill_Arr3D_Rand(P, M, N);
            break;
        case 3:
            Fill_Arr3D_Back(P, M, N);
            break;
        default:
            break;
        }
        Res[i] = Select7(Arr3D, P, M, N);
        Free(P,M);
    }
}

void sort_type_Select2_Vect(int type)//заповнення вектора значень часу для сортування вибором №2
{
    for(int i=0; i<measurements_number; i++)
    {
        Creation_Vect(M*N);
        switch(type)
        {
         case 1:
            Fill_Vect_Sort(M*N);
            break;
        case 2:
            Fill_Vect_Rand(M*N);
            break;
        case 3:
            Fill_Vect_Back(M*N);
            break;
        default:
            break;
        }
        Res[i] = Select2_Vect(Vector,M*N);
        free(Vector);
    }
}
void sort_type_Insertion_Vect(int type)//заповнення вектора значень часу для сортування "обмін-вставка"
{
    for(int i=0; i<measurements_number; i++)
    {
       Creation_Vect(M*N);
        switch(type)
        {
         case 1:
            Fill_Vect_Sort(M*N);
            break;
        case 2:
            Fill_Vect_Rand(M*N);
            break;
        case 3:
            Fill_Vect_Back(M*N);
            break;
        default:
            break;
        }
        Res[i] = InsertExchange_Vect(Vector,M*N);
        free(Vector);
    }
}
void sort_type_Select7_Vect(int type)//заповнення вектора значень часу для сортування вибором №7
{
    for(int i=0; i<measurements_number; i++)
    {
        Creation_Vect(M*N);
        switch(type)
        {
        case 1:
            Fill_Vect_Sort(M*N);
            break;
        case 2:
            Fill_Vect_Rand(M*N);
            break;
        case 3:
            Fill_Vect_Back(M*N);
            break;
        default:
            break;
        }
        Res[i] =  Select7_Vect(Vector,M*N);
        free(Vector);
    }
}
float MeasurementProcessing()// Функція обробки і усереднення значень вимірів
{

    long int Sum;
    float AverageValue;

    clock_t buf;
    int L = rejected_number, R = measurements_number - 1;
    int k = rejected_number;
    for (int j=0; j < min_max_number; j++)
    {
        for (int i = L; i < R; i++)
        {
            if (Res[i] > Res[i + 1])
            {
                buf = Res[i];
                Res[i] = Res[i + 1];
                Res[i + 1] = buf;
                k = i;
            }
        }

        R = k;
        for (int i = R - 1; i >= L; i--)
        {
            if (Res[i] > Res[i + 1])
            {
                buf = Res[i];
                Res[i] = Res[i + 1];
                Res[i + 1] = buf;
                k = i;
            }
        }
        L = k + 1;
    }

    Sum=0;
    for (int i = rejected_number + min_max_number; i < measurements_number - min_max_number; i++)
        Sum = Sum + Res[i];

    AverageValue = (float)Sum / (float)(measurements_number - 2*min_max_number - rejected_number);

    return AverageValue;
}
