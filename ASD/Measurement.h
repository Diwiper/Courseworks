#ifndef MEASUREMENT_H_INCLUDED
#define MEASUREMENT_H_INCLUDED

#include <time.h>

// �������� ������� ����� ���� ������ ���������
#define measurements_number 28
// ʳ������ ��������� ���������� �����
#define rejected_number 2
#define min_max_number 3// ʳ������ ��������� ����� � ��������� ���������� �� � ������������� ����������

extern clock_t Res[measurements_number];// ����� ������� ���� ������ ���������

void sort_type_Select2(int type);//���������� ������ ������� ���� ��� ���������� ������� �2
void sort_type_Insertion(int type);//���������� ������ ������� ���� ��� ���������� "����-�������"
void sort_type_Select7(int type);//���������� ������ ������� ���� ��� ���������� ������� �7
void sort_type_Select2_Vect(int type);//���������� ������� ������� ���� ��� ���������� ������� �2
void sort_type_Insertion_Vect(int type);//���������� ������� ������� ���� ��� ���������� "����-�������"
void sort_type_Select7_Vect(int type);//���������� ������� ������� ���� ��� ���������� ������� �7
float MeasurementProcessing();// ������� ������� � ����������� ������� �����

#endif // MEASUREMENT_H_INCLUDED



