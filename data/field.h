#ifndef FM_FIELD_H
#define FM_FIELD_H

/**
«аголовочный файл модул€, который будет описывать структуру данных полей -
функций от двух и более переменных, заданных на равномерной сетке.
ѕока дл€ простоты выделим 2-мерные и 3-мерные пол€, потом, в случае чего, обобщим.

Field3 можно реализовать как подкласс Field2 - ??
*/

class FMField2
{
protected:
   double X0, Y0; // начальные значени€ дл€ X, Y
   double dX, dY; // шаг сетки по X, Y
   int maxX, maxY;// сколько значений

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
   double X0; // начальные значени€ дл€ X, Y
   double dX; // шаг сетки по X, Y
   int maxX;// сколько значений

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
// ƒл€ простоты - функци€ от трЄх переменных на основе Field2, без наследовани€
/*class FMField3
{
protected:
   double X0, Y0, Z0; // начальные значени€ дл€ X, Y
   double dX, dY, dZ; // шаг сетки по X, Y
   int maxX, maxY, maxZ;// сколько значений

   double def;  // значение по умолчанию - сейчас не используетс€

   double*** F;   // коренное отличие от Field2 - массив теперь трЄхмерен

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
