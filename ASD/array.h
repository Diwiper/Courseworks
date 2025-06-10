#ifndef ARRAY_H_INCLUDED
#define ARRAY_H_INCLUDED
#define P 3 // ʳ������ �������
#define M 64// ʳ������ �����
#define N 64// ʳ������ ���������
#define P_T 3//  ʳ������ ������� ��� ����� ����������
#define NM_Test 6 //ʳ������ ����� � ��������� ��� ����� ����������
extern int ***Arr3D;
extern int *Vector;

void Creation(int p, int m,  int n);//��������� ���������� �������������� ������
void Fill_Arr3D_Sort(int p, int m,  int n);// ���������� �������������� ������ ��� ������� "�����������"
void Fill_Arr3D_Rand(int p, int m,  int n);// ���������� �������������� ������ ��� ������� "�� �������������"
void Fill_Arr3D_Back (int p, int m,  int n);// ���������� �������������� ������ ��� ������� "��������-�����������"
void Out_Arr3D(int p, int m,  int n); // ��������� �������������� ������
void Creation_Vect(int n);//��������� ���������� �������
void Fill_Vect_Sort(int n);// ���������� ������� ��� ������� "�����������"
void Fill_Vect_Rand(int n);// ���������� ������� ��� ������� "�� �������������"
void Fill_Vect_Back(int n);// ���������� ������� ��� ������� "��������-�����������"
void Free(int p, int m);//�������� �������������� ���������� ������

#endif // ARRAY_H_INCLUDED
