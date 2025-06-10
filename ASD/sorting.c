#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include "array.h"
#include "sorting.h"


clock_t Select2(int ***A, int p, int m, int n) //Алгоритм сортування №2 методу прямого вибору.
{
    int imin, jmin, tmp, si, sj, flag;
    clock_t time_start, time_stop;
    time_start = clock();
    for(int k=0; k<p; k++)
    {
        si=0;
        sj=0;
        while(sj<=n-1||si<m-2)
        {
            imin=si;
            jmin=sj;
            flag=1;
            for(int j=sj; j<n; j++)
            {
                int i=si;
                if(flag==1)
                    while(i<m)
                    {
                        if (A[k][i][j]<A[k][imin][jmin])
                        {
                            imin=i;
                            jmin=j;
                        }
                        i++;
                    }
                if(sj<=n-1&&flag==0)
                    for(int i=0; i<m; i++)
                        if (A[k][i][j]<A[k][imin][jmin])
                        {
                            imin=i;
                            jmin=j;
                        }
                flag=0;
            }
            tmp=A[k][imin][jmin];
            A[k][imin][jmin]=A[k][si][sj];
            A[k][si][sj]=tmp;
            si++;
            if(si==m)
            {
                sj++;
                si=0;
            }
        }
    }
    time_stop = clock();
    return (time_stop - time_start);
}

clock_t InsertExchange(int ***A, int p, int m, int n) //Гібридний алгоритм "вставка – обмін".
{
    int j, tmp, i, q, r, flag;
    clock_t time_start, time_stop;
    time_start = clock();
    for(int k=0; k<p; k++)
    {
        i=1;
        for(int j=0; j<n; j++)
        {
            if(j>0)
                i=0;
            while(i<m)
            {
                q=j;
                r=i;
                i++;
                flag=1;
                tmp=A[k][r][q];
                if(r==0)
                {
                    q--;
                    flag=0;
                    r=m;
                }
                while (q>=0 && r>0 && tmp<A[k][r-1][q])
                {
                    if(flag==1)
                    {
                        A[k][r][q] = A[k][r-1][q];
                        A[k][r-1][q]=tmp;
                        r--;
                    }
                    else
                    {
                        A[k][0][q+1]= A[k][r-1][q] ;
                        A[k][r-1][q]=tmp;
                        flag=1;
                        r--;
                    }
                    if(r==0)
                    {
                        tmp=A[k][0][q];
                        q--;
                        r=m;
                        flag=0;
                    }
                }
            }
        }
    }
    time_stop = clock();
    return time_stop - time_start;
}

clock_t Select7(int ***A, int p, int m, int n)//Алгоритм сортування №7 методу прямого вибору.
{
    int Min, Max;
    int i, L_i,L_j,R_i, R_j, imin, imax, jmin, jmax, m1;
    clock_t time_start, time_stop;
    time_start = clock();
    for(int k=0; k<p; k++)
    {
        L_j=0;
        R_j=n-1;
        L_i=0;
        R_i=m-1;
        while (L_j<=R_j||(L_i<R_i&&L_j<R_j))
        {

            Min=A[k][L_i][L_j];
            imin=L_i;
            jmin=L_j;
            Max=A[k][L_i][L_j];
            imax=L_i;
            jmax=L_j;
            m1=m;
            for(int j=L_j; j<R_j+1; j++)
            {
                i=L_i;
                if(L_j!=j)
                    i=0;
                while(i<m1)
                {
                    if (A[k][i][j] < Min)
                    {
                        Min=A[k][i][j];
                        imin=i;
                        jmin=j;
                    }
                    else if (A[k][i][j] > Max)
                    {
                        Max=A[k][i][j];
                        imax=i;
                        jmax=j;
                    }
                    i++;
                    if(j==R_j)
                        m1=R_i+1;
                }
            }
            if (imin!=L_i||jmin!=L_j)
            {
                A[k][imin][jmin]=A[k][L_i][L_j];
                A[k][L_i][L_j]=Min;
            }
            if (imax!=R_i||jmax!=R_j)
            {
                if (imax==L_i&&jmax==L_j) A[k][imin][jmin]=A[k][R_i][R_j];
                else A[k][imax][jmax]=A[k][R_i][R_j];
                A[k][R_i][R_j]=Max;
            }
            L_i++;
            R_i--;
            if(L_i==m)
            {
                L_i=0;
                R_i=m-1;
                L_j++;
                R_j--;
            }
            if(Max<=Min)
                break;
        }
    }
    time_stop = clock();
    return time_stop - time_start;
}


//////////////////////////Сортування Векторів///////////////////////////


clock_t Select2_Vect(int *A, int n) //Алгоритм сортування №2 методу прямого вибору для вектора.
{
    int imin, tmp;
    clock_t time_start, time_stop;
    time_start = clock();
    for(int s=0; s<n-1; s++)
    {
        imin=s;
            for(int i=s+1; i<n; i++)
            if (A[i]<A[imin])imin=i;
        tmp=A[imin];
        A[imin]=A[s];
        A[s]=tmp;
    }
    time_stop = clock();
    return time_stop - time_start;
}
clock_t InsertExchange_Vect(int *A, int n) //Гібридний алгоритм "вставка – обмін".
{
    int j, tmp;
    clock_t time_start, time_stop;
    time_start = clock();
    for(int i=1; i<n; i++)
    {
        j=i;
        while (j>0 && A[j]<A[j-1])
        {
            tmp=A[j];
            A[j]=A[j-1];
            A[j-1]=tmp;
            j=j-1;
        }
    }
    time_stop = clock();
    return time_stop - time_start;
}

clock_t Select7_Vect(int *A, int n)//Алгоритм сортування №7 методу прямого вибору.
{
    int Min, Max;
    int L, R, imin, imax;
    clock_t time_start, time_stop;
    time_start = clock();
    L=0;
    R=n-1;
    while (L<R)
    {
        Min=A[L];
        imin=L;
        Max=A[L];
        imax=L;
        for(int i=L+1; i<R+1; i++)
        {
            if (A[i] < Min)
            {
                Min=A[i];
                imin=i;
            }
            else if (A[i] > Max)
            {
                Max=A[i];
                imax=i;
            }
        }
        if (imin!=L)
        {
            A[imin]=A[L];
            A[L]=Min;
        }
        if (imax!=R)
        {
            if (imax==L) A[imin]=A[R];
            else A[imax]=A[R];
            A[R]=Max;
        }
        L=L+1;
        R=R-1;
    }
    time_stop = clock();
    return time_stop - time_start;
}
