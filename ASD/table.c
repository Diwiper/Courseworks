#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include "array.h"
#include "Measurement.h"

int table(int type)// функція виводу часу роботи різних методів сортування
{

    switch(type)//вибір виведення(масив, вектор, чи одночасно)
    {
    case 1:
        printf("\t\tTable for array:\tP=%d, M=%d, N=%d\n",P,M,N);
        printf("\n\t\t\tSorted\t\tRandom\t\tReverse Sorted");

        printf("\nSelect2");
        sort_type_Select2(1);
        printf("\t\t\t%.1f",MeasurementProcessing());
        sort_type_Select2(2);
        printf("\t\t%.1f", MeasurementProcessing());
        sort_type_Select2(3);
        printf("\t\t%.1f\n", MeasurementProcessing());


        printf("\nInsertionExchange");
        sort_type_Insertion(1);
        printf("\t%.1f", MeasurementProcessing());
        sort_type_Insertion(2);
        printf("\t\t%.1f", MeasurementProcessing());
        sort_type_Insertion(3);
        printf("\t\t%.1f\n", MeasurementProcessing());

        printf("\nSelect7");
        sort_type_Select7(1);
        printf("\t\t\t%.1f", MeasurementProcessing());
        sort_type_Select7(2);
        printf("\t\t%.1f", MeasurementProcessing());
        sort_type_Select7(3);
        printf("\t\t%.1f\n", MeasurementProcessing());
        break;
    case 2:
        printf("\n\t\tTable for vector:\tN=%d\n",M*N);
        printf("\n\t\t\tSorted\t\tRandom\t\tReverse Sorted");
        printf("\nSelect2");
        sort_type_Select2_Vect(1);
        printf("\t\t\t%.1f",P*MeasurementProcessing());
        sort_type_Select2_Vect(2);
        printf("\t\t%.1f", P*MeasurementProcessing());
        sort_type_Select2_Vect(3);
        printf("\t\t%.1f\n", P*MeasurementProcessing());


        printf("\nInsertionExchange");
        sort_type_Insertion_Vect(1);
        printf("\t%.4f", P*MeasurementProcessing());
        sort_type_Insertion_Vect(2);
        printf("\t\t%.1f", P*MeasurementProcessing());
        sort_type_Insertion_Vect(3);
        printf("\t\t%.1f\n", P*MeasurementProcessing());

        printf("\nSelect7");
        sort_type_Select7_Vect(1);
        printf("\t\t\t%.1f", P*MeasurementProcessing());
        sort_type_Select7_Vect(2);
        printf("\t\t%.1f", P*MeasurementProcessing());
        sort_type_Select7_Vect(3);
        printf("\t\t%.1f\n",P*MeasurementProcessing());
        break;
    case 3:
        table(1);
        table(2);
        break;
    default:
        printf("Incorrect data, try again");
        sleep(1);
        return 0;
        break;
    }
    return 1;
}
