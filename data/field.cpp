#include "field.h"

#include <fstream>
#include <math.h>


// =======================================================================
// Реализация методов Field2
// =======================================================================

FMField2::FMField2()
{
   X0 = 0;
   Y0 = 0;

   dX = 0;
   dY = 0;

   maxX = 0;
   maxY = 0;

   F = 0;
}

/**
   Почему-то деструктор вызывается тогда, когда это не нужно. Вероятно, это
   связано с созданием локальных объектов
*/
FMField2::~FMField2()
{
	// TODO
   /*if (F)
   {
      for (int i = 0; i < maxY; i++)
      {
         delete F[i];
      }

      delete F;
   }*/
}

/**
Нахождение значения в точке посредством билинейной интерполяции
*/
double FMField2::get(double X, double Y)
{
   if (F == 0) return 0;
   // - проверка на попадание в имеющуюся зону. Если искомая точка за пределами -
   // возвращаем ноль.

   if (X < X0) X = X0;
   if (Y < Y0) Y = Y0;
   if (X > (X0 + dX*(maxX-1))) X = X0 + dX*(maxX-1);
   if (Y > (Y0 + dY*(maxY-1))) Y = Y0 + dY*(maxY-1);


   // - поиск ближайших к искомой точке соседей
   //double x1, x2, y1, y2; // значения координат, соответсвующих соседям
   //double f11, f12, f21, f22; // значения в точках-соседях

   int i = floor((X - this->X0) / this->dX);
   int j = floor((Y - this->Y0) / this->dY);

   //if (i == 

   if (i > this->maxX-2)
   {
      i = this->maxX-2;
   }

   if (j > this->maxY-2)
   {
      j = this->maxY-2;
   }

   double x1 = (i) * this->dX + this->X0;
   double y1 = (j) * this->dY + this->Y0;
   double x2 = (i+1) * this->dX + this->X0;
   double y2 = (j+1) * this->dY + this->Y0;

   double f11 = F[j*this->maxY + i];      		// x1 y1
   double f12 = F[j*this->maxY + i + 1];    	// x2 y1
   double f21 = F[(j+1)*this->maxY + i];    	// x1 y2
   double f22 = F[(j+1)*this->maxY + i + 1];  	// x2 y2

   // - интерполяция
   double Kf = 1 / ((x2-x1)*(y2-y1));

   double xyF;
   xyF = f11*Kf * (x2-X)*(y2-Y)
       + f12*Kf * (X-x1)*(y2-Y)
       + f21*Kf * (x2-X)*(Y-y1)
       + f22*Kf * (X-x1)*(Y-y1);
   return xyF;
}

/**
Заполнение данных согласно внешним источникам
*/
void FMField2::init(double x0, double dx, int maxx,
                    double y0, double dy, int maxy,
                    double defval)
{
   this->X0 = x0;
   this->Y0 = y0;
   this->dX = dx;
   this->dY = dy;
   this->maxX = maxx;
   this->maxY = maxy;

   this->def = defval;
}

// ---------------------------------------------------
FMField1::FMField1()
{
   X0 = 0;

   dX = 0;

   maxX = 0;

   F = 0;
}

/**
   Почему-то деструктор вызывается тогда, когда это не нужно. Вероятно, это
   связано с созданием локальных объектов
*/
FMField1::~FMField1()
{
	// TODO
   /*if (F)
   {
      for (int i = 0; i < maxY; i++)
      {
         delete F[i];
      }

      delete F;
   }*/
}

/**
Нахождение значения в точке посредством билинейной интерполяции
*/
double FMField1::get(double X)
{
   if (F == 0) return 0;
   // - проверка на попадание в имеющуюся зону. Если искомая точка за пределами -
   // возвращаем ноль.

   if (X < X0) X = X0;
   if (X > (X0 + dX*(maxX-1))) X = X0 + dX*(maxX-1);

   // - поиск ближайших к искомой точке соседей
   //double x1, x2, y1, y2; // значения координат, соответсвующих соседям
   //double f11, f12, f21, f22; // значения в точках-соседях

   int i = floor((X - this->X0) / this->dX);

   //if (i ==

   if (i > this->maxX-2)
   {
      i = this->maxX-2;
   }

   double x1 = (i) * this->dX + this->X0;
   double x2 = (i+1) * this->dX + this->X0;

   double f11 = F[i];      		// x1 y1
   double f12 = F[i + 1];    	// x2 y1

   // - интерполяция
   double Kf = 1 / (x2-x1);

   double xyF;
   xyF = f11*Kf * (x2-X)
       + f12*Kf * (X-x1);
   return xyF;
}

/**
Заполнение данных согласно внешним источникам
*/
void FMField1::init(double x0, double dx, int maxx,
                    double defval)
{
   this->X0 = x0;
   this->dX = dx;
   this->maxX = maxx;

   this->def = defval;
}

/**
Выделение памяти под массив данных
*/
/*void FMField2::allocF()
{
   F = (double**) malloc(maxY * sizeof(double*));
   for (int i = 0; i < maxY; i++)
   {
      F[i] = (double*) malloc(maxX * sizeof(double));
   }
}*/

// =======================================================================
// Реализация методов Field3
// =======================================================================
/*FMField3::FMField3()
{
   X0 = 0;
   Y0 = 0;
   Z0 = 0;

   dX = 0;
   dY = 0;
   dZ = 0;

   maxX = 0;
   maxY = 0;
   maxZ = 0;

   F = 0;
}*/

/**
Нахождение значения в точке посредством билинейной интерполяции
*/
/*double FMField3::get(double X, double Y, double Z)
{
   if (F == 0) return 0;
   // - проверка на попадание в имеющуюся зону. Если искомая точка за пределами -
   // возвращаем ноль.

   // ... TODO
   int i = (X - this->X0) / this->dX;
   int j = (Y - this->Y0) / this->dY;
   int k = (Z - this->Z0) / this->dZ;

   // - поиск ближайших к искомой точке соседей
   //double x1, x2, y1, y2, z1, z2; // значения координат, соответсвующих соседям
   //double f111, f112, f121, f122, f211, f212, f221, f222; // значения в точках-соседях

   // ... TODO
   double x1 = i * this->dX + this->X0;
   double y1 = j * this->dY + this->Y0;
   double z1 = k * this->dZ + this->Z0;
   double x2 = (i+1) * this->dX + this->X0;
   double y2 = (j+1) * this->dY + this->Y0;
   double z2 = (k+1) * this->dZ + this->Z0;

   double f111 = F[k][j][i];        // x1 y1 z1
   double f112 = F[k][j][i+1];      // x2 y1 z1
   double f121 = F[k][j+1][i];      // x1 y2 z1
   double f122 = F[k][j+1][i+1];    // x2 y2 z1
   double f211 = F[k+1][j][i];      // x1 y1 z2
   double f212 = F[k+1][j][i+1];    // x2 y1 z2
   double f221 = F[k+1][j+1][i];    // x1 y2 z2
   double f222 = F[k+1][j+1][i+1];  // x2 y2 z2

   // - интерполяция
   double Kf = 1/((x2-x1)*(y2-y1)*(z2-z1)) ;

   double xyzF;
   xyzF = f111*Kf * (x2-X)*(y2-Y)*(z2-Z)
        + f112*Kf * (X-x1)*(y2-Y)*(z2-Z)
        + f121*Kf * (x2-X)*(Y-y1)*(z2-Z)
        + f122*Kf * (X-x1)*(Y-y1)*(z2-Z)
        + f211*Kf * (x2-X)*(y2-Y)*(Z-z1)
        + f212*Kf * (X-x1)*(y2-Y)*(Z-z1)
        + f221*Kf * (x2-X)*(Y-y1)*(Z-z1)
        + f222*Kf * (X-x1)*(Y-y1)*(Z-z1);
   return xyzF;
}*/

/**
Заполнение данных согласно внешним источникам
*/
/*void FMField3::init(double x0, double dx, int maxx,      // так и просится структура из трёх чисел
                    double y0, double dy, int maxy,
                    double z0, double dz, int maxz,
                    double defval)
{
   this->X0 = x0;
   this->Y0 = y0;
   this->Z0 = z0;

   this->dX = dx;
   this->dY = dy;
   this->dZ = dz;
   
   this->maxX = maxx;
   this->maxY = maxy;
   this->maxZ = maxz;

   this->def = defval;
}*/


// -----------------------------------------------------------------------

/**
Создание массива
*/
/*double** CreateMatrix(int maxX, int maxY)
{
   // создание массива
   double** m = new double*[maxY];
   for (int i = 0; i < maxY; i++)
   {
      m[i] = new double[maxX];
   }
   
   return m;
}*/

/**
 *  Создание Field из файла
 */
/*FMField2* ReadField2FromFile(char* FileName)
{
   std::ifstream f;
   
   f.open(FileName);
   if (f.fail())
   {
      return 0;
   }

   // чтение данных
   int nI, nJ;
   double I0, In, J0, Jn;
   double def;

   f >> nJ >> J0 >> Jn;
   f >> nI >> I0 >> In;
   f >> def;

   // создание массива nI x nJ
   // i - столбцы, j - строки
   double** m = CreateMatrix(nI, nJ);
   for (int j = 0; j < nJ; j++)
   {
      for (int i = 0; i < nI; i++)
      {
         f >> m[i][j];
      }
   }

   f.close();
   
   // инициализация
   FMField2* field = new FMField2;
   double dI = (In-I0) / (nI-1);
   double dJ = (Jn-J0) / (nJ-1);

   field->init(I0,dI,nI,J0,dJ,nJ,def);
   field->set(&m);

   return field;
}*/

