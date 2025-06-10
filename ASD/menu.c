#include <stdio.h>
#include <stdlib.h>
#include "array.h"
#include "menu.h"
#include "table.h"
#include "sorting.h"

void menu()//навігація по програмі
{
    int choice, type, flag=1, flag1=1;
    char str[1];
    while(flag==1)
    {
        system("cls");
        printf("Press number to choice");
        printf("\n1.Table\n2.Test Select2\n3.Test InsertExchange\n4.Test Select7\n");
        str[0]=getch();
        choice=atoi(str);
        switch(choice) //Вибір: тест сортування або таблиця результатів
        {
        case 1:
            system("cls");
            printf("Press number to choice\n");
            printf("1. 3D array\n2. Vector\n3. Both\n");
            str[0]=getch();
            type=atoi(str);
            system("cls");
            flag1=table(type);
            break;
        case 2:
            flag1=test_algoritm(1);// flag1 відповідає за коректне введення в меню і ні на що не впливає
            break;
        case 3:
            flag1=test_algoritm(2);
            break;
        case 4:
            flag1=test_algoritm(3);
            break;
        default:
             printf("Incorrect data, try again");
             sleep(1);
             system ("cls");
             flag1=0;
            break;
        }
        if(flag1==1)
        {
            flag1=1;
            printf("\nPress number to choice\n");
            printf("1.Return to menu\n2.Exit");
            str[0]=getch();
            choice=atoi(str);
            printf("%d", choice);
            if(choice!=1)
            {
                fflush(stdin);
                flag=0;
                system("cls");
            }
        }

    }
}

int test_algoritm(int type)//тестування роботи сортування
{
    system("cls");
    int choice;
    char str[1];
    printf("Press number to choice");
    printf("\n1.Sorted Array\n2.Random Sorted Array\n3.Reverse Sorted Array\n");
    str[0]=getch();
    choice=atoi(str);
    system("cls");
    Creation(P_T, NM_Test, NM_Test);
    switch(choice)//Вибір методу заповнення масиву
    {
    case 1:
        Fill_Arr3D_Sort(P_T,NM_Test,NM_Test);
        break;
    case 2:
        Fill_Arr3D_Rand(P_T,NM_Test,NM_Test);
        break;
    case 3:
        Fill_Arr3D_Back(P_T,NM_Test,NM_Test);
         break;
    default:
         printf("Incorrect data, try again");
             sleep(1);
        return 0;
        break;
    }
    printf("\tP=%d, M=%d, N=%d\n\n",P_T,NM_Test,NM_Test);
     printf("\tNot sorted\n");
  Out_Arr3D(P_T,NM_Test,NM_Test);
 switch(type)//Вибір методу сортування масиву
    {
    case 1:
        Select2(Arr3D, P_T,NM_Test,NM_Test);
        break;
    case 2:
        InsertExchange(Arr3D, P_T,NM_Test,NM_Test);
        break;
    case 3:
        Select7(Arr3D, P_T,NM_Test,NM_Test);
         break;
    default:
         printf("Incorrect data, try again");
             sleep(1);
        return 0;
        break;
    }
     printf("\n\t Sorted\n");
  Out_Arr3D(P_T, NM_Test, NM_Test);
  Free(P_T, NM_Test);
  return 1;

}




























