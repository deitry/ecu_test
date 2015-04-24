#ifndef FM_FIELD_H
#define FM_FIELD_H

/**
������������ ���� ������, ������� ����� ��������� ��������� ������ ����� -
������� �� ���� � ����� ����������, �������� �� ����������� �����.
���� ��� �������� ������� 2-������ � 3-������ ����, �����, � ������ ����, �������.

Field3 ����� ����������� ��� �������� Field2 - ??
*/

class FMField2
{
protected:
   double X0, Y0; // ��������� �������� ��� X, Y
   double dX, dY; // ��� ����� �� X, Y
   int maxX, maxY;// ������� ��������

   double* F;

   double def;

public:
   FMField2();
   ~FMField2();
   double get(double X, double Y);

   void init(double X0, double dX, int maxX,
             double Y0, double dY, int MaxY,
             double defval);

   //void allocF();

   void set(double* F1) {this->F = F1;} // ??
};

class FMField1
{
protected:
   double X0; // ��������� �������� ��� X, Y
   double dX; // ��� ����� �� X, Y
   int maxX;// ������� ��������

   double* F;

   double def;

public:
   FMField1();
   ~FMField1();
   double get(double X);

   void init(double X0, double dX, int maxX,
             double defval);

   //void allocF();

   void set(double* F1) {this->F = F1;} // ??
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ��� �������� - ������� �� ��� ���������� �� ������ Field2, ��� ������������
/*class FMField3
{
protected:
   double X0, Y0, Z0; // ��������� �������� ��� X, Y
   double dX, dY, dZ; // ��� ����� �� X, Y
   int maxX, maxY, maxZ;// ������� ��������

   double def;  // �������� �� ��������� - ������ �� ������������

   double*** F;   // �������� ������� �� Field2 - ������ ������ ��������

public:
   FMField3();
   ~FMField3() {delete F;}
   double get(double X, double Y, double Z);

   void init(double X0, double dX, int maxX,
             double Y0, double dY, int maxY,
             double Z0, double dZ, int maxZ,
             double defval);


   void setF(double*** F1) {this->F = F1;}
};*/

/*double** CreateMatrix(int maxX, int maxY);
FMField2* ReadField2FromFile(char* FileName);
*/

#endif
