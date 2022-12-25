// ASCI_ART.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <fstream>

enum Axis {
    X, Y, Z
};

#define WIDTH 60
#define HEIGHT 50
#define STEP 0.001
#define PI 3.14159265359
#define ORIGIN_X 10
#define ORIGIN_Y 10
double j = 1;
struct Matrix_2D {
} typedef Matrix;


struct Point_3D {
    double X;
    double Y;
    double Z;

    Point_3D(double X, double Y, double Z)
    {
        this->X = X;
        this->Y = Y;
        this->Z = Z;
    };

    Point_3D operator-(Point_3D point)
    {
        return { X - point.X, Y - point.Y, Z - point.Z };
    }
} typedef Point;

struct Triangle_3D {

    int p0;
    int p1;
    int p2;

    Triangle_3D(int x, int y, int z)
    {
        this->p0 = x;
        this->p1 = y;
        this->p2 = z;
    };

} typedef Triangle;

void rotate_point(Point *point, Axis axis, double angle)
{
    double xx = point->X;
    double yy = point->Y;
    double zz = point->Z;

    switch (axis)
    {
    case X:
        point->Y = yy * cos(angle) - zz * sin(angle);
        point->Z = yy * sin(angle) + zz * cos(angle);
        break;
    case Y:
        point->X = xx * cos(angle) + zz * sin(angle);
        point->Z = -xx * sin(angle) + zz * cos(angle);
        break;
    case Z:
        point->X = xx * cos(angle) - yy * sin(angle);
        point->Y = xx * sin(angle) + yy * cos(angle);
        break;
    default:
        break;
    }
}
bool draw_point(double x, double y, double z, char** inner, char character)
{
    // Rotation
    // 
    // ARROUND Z AXIS
    Point p {x,y,z};

    rotate_point(&p, Y, j);
    //// AROUND X AXIS
    rotate_point(&p, Z, j);
    rotate_point(&p, X, j);

    x = p.X;
    y = p.Y;
    z = p.Z;

    x /= (double)(z + 1);
    y /= (double)(z + 1);
    // Scale to matrix
    int xx = (int)round((x+1)/2.0 * HEIGHT);
    int yy = (int)round((y+1)/2.0 * WIDTH);

    if (xx >= 0 && xx < HEIGHT &&
        yy >= 0 && yy < WIDTH) {
        inner[xx][yy] = character;
        return true;
    }
    return false;
}


void draw_line_derivative_character(Point point1, Point point2, char** inner)
{
    double xK = 0, zK = 0, yK = 0;
    double t = 0;
    char character = ' ';
    Point vec = point2 - point1;

    Point xAxis = { 1, 0, 0 };

    double len = sqrtf(vec.X * vec.X + vec.Y * vec.Y + vec.Z * vec.Z);
    vec.X /= len;
    vec.Y /= len;
    vec.Z /= len;

    double alpha = acos(vec.X);


    if (alpha < PI / 2.0 )
    {
        character = '/';
    }
    else if (alpha > PI / 2.0 )
    {
        character = '\\';
    }
    else if (alpha < 0.005)
    {
        character = '\|';
    }
    else if (abs(alpha - PI / 2.0) < 0.005)
    {
        character = '=';
    }


    while (t < len)
    {
        xK = point1.X + vec.X * t;
        yK = point1.Y + vec.Y * t;
        zK = point1.Z + vec.Z * t;

        draw_point(xK, yK, zK, inner, character);
        t += STEP;
    }
}

void draw_line(Point point1, Point point2, char** inner, char character)
{
    double xK = 0, yK = 0, zK = 0;
    double t = 0;
    Point vec = point2 - point1;
    
    double len = sqrtf(vec.X * vec.X + vec.Y * vec.Y + vec.Z * vec.Z);
    vec.X /= len;
    vec.Y /= len;
    vec.Z /= len;
    while (t < len)
    {
        xK = point1.X + vec.X * t;
        yK = point1.Y + vec.Y * t;
        zK = point1.Z + vec.Z * t;

        draw_point(xK, yK, zK, inner, character);
        t += STEP;
    }
}


void draw_circle(char** inner, double r, double h, double v)
{
    double i = 0;
    double xK = 0;
    double yK = 0;
    int j = 0;

    // Draw circle
    while (i < 2 * PI)
    {
        i += STEP;
        xK = h + cos(i) * r;
        yK = v + sin(i) * r;
        draw_point(xK, yK, 0, inner, '+');
    }
}

void draw_pentagram(char** inner, double r, double h, double v, bool more_circles)
{
    double i = 0;
    double xK = 0;
    double yK = 0;
    int j = 0;

    std::vector<Point> pentagram_points;
    // Draw circle
    while (i < 2 * PI)
    {

        i += STEP;
        xK = h + cos(i) * r;
        yK = v + sin(i) * r;
       // draw_point(xK, yK, 0, inner, '+');

        // Collect pentagram points
        if (abs(i - j * 2 * PI / 5.0) < 0.005)
        {
            pentagram_points.push_back({ xK, yK, abs(xK - yK) / 2.0});
            j++;
        }
    }

     //Pentagram lines
    //for (int i = 0; i <3; i ++)
    //    for (int j = i + 2; j < i + 4; j += 1) {
    //        if (more_circles) draw_circle(inner, 0.1, pentagram_points[i].X, pentagram_points[i].Y);//
    //        draw_line_derivative_character(pentagram_points[i], pentagram_points[j], inner);
    //    }
            

    for (int i = 0; i < pentagram_points.size() - 1; i++)
    {
        //if (more_circles) draw_circle(inner, 0.1, pentagram_points[i].X, pentagram_points[i].Y);//
        for (int j = i + 1; j < pentagram_points.size(); j += 1)
        {
            draw_line_derivative_character(pentagram_points[i], pentagram_points[j], inner);
        }
    }
}

void print(char** inner)
{
    for (int i = 0; i < HEIGHT; i += 1)
    {
        for (int j = 0; j < WIDTH; j += 1)
        {
            std::cout << " " << inner[i][j];
        }
        std::cout << std::endl;
    }
}

// Clear the matrix
void clear(char** inner)
{
    for (int x = 0; x < HEIGHT; x++)
    {
        for (int y = 0; y < WIDTH; y++)
        {
            inner[x][y] = ' ';
        }
    }
}

int main()
{
    // Prepare matrix to display
    char** inner = { 0 };
    inner = new char*[HEIGHT];
    for (int i = 0; i < HEIGHT; i++)
    {
        inner[i] = new char[WIDTH];
        for (int j = 0; j < WIDTH; j++) inner[i][j] = 0x20;
    }

    //draw_pentagram(inner, 0.9f, 0.f, 0.f, true);
    //draw_line({0,0,0}, {0,1,0}, inner, 'Y');
    //draw_line({ 0,0,0 }, { 1,0,0 }, inner, 'X');
    //draw_line({ 0,0,0 }, { 0,0,1 }, inner, 'Z');
    //draw_line({ 0,0,0 }, { 2,2,1 }, inner, '1');
    //draw_line({ 0,0,0 }, { -4,2,1 }, inner, '2');


    //double stepp = PI / 32.0;
    //double two_pi = 2 * PI;
    //double aLen = 0.2; 
    //int maxIter = 10;
    //int i = maxIter;

    //std::vector<Point_3D> pts = {};
    //int range_x = 2 / 0.01;
    //while (true)
    //{
    //    double step = 0.05;
    //    double lowerBound = -2;
    //    double upperBound = 2;
    //    for (double x = lowerBound; x < upperBound; x += step) {
    //        for (double y = lowerBound; y < upperBound; y += step) {
    //            for (double z = lowerBound; z < upperBound; z += step) {
    //                //While complex number converges
    //                i = maxIter;
    //                double zxzx = x * x;
    //                double zyzy = y * y;
    //                double zzzz = z * z;

    //                double zx = x;
    //                double zy = y;
    //                double zz = z;
    //                while (zx + zy + zz< 4 && i > 0) {
    //                    //One iteration applies some functions to X and Y coordinates
    //                    double tmp = zxzx - zyzy - zzzz - 0.399;
    //                    zy = 2 * zx * zy *zz + 0.651;
    //                    zz = zx * zz - 0.852;
    //                    zx = tmp;

    //                    //Continue cycle until we are out of iterations
    //                    zxzx = zx * zx;
    //                    zyzy = zy * zy;
    //                    zzzz = zz * zz;
    //                    --i;
    //                }

    //                if (i <4) {
    //                    double ii = (i + 1) / (double)maxIter;
    //                    char c = ' ';

    //                    if (ii < 0.2) {
    //                        c = '.';
    //                    }
    //                    else if (ii < 0.4) {
    //                        c = ',';
    //                    }
    //                    else if (ii < 0.6) {
    //                        c = ';';
    //                    }
    //                    else {
    //                        c = 'x';
    //                    }
    //                    //pts.push_back({ x, y, z });
    //                    draw_point(x, y, ii, inner, c);
    //                }


    //            }
    //        }
    //    }
    //    print(inner);
    //    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //    clear(inner);
    //    printf("\033[%d;%dH", 0, 0); //system("cls");
    //    //if (j != 2*PI) system("cls");
    //    std::cout << "J:" << j << std::endl;
    //}

    //std::vector<Triangle> tris = {};
    //for (int i = 1; i < pts.size() - 1 - range_x; i++) {
        // These points make up first triangle
        // i = current point
        // i + 1 = next point
        // i + range(x) 
        //tris.push_back({ i, i + 1, i + (int)range_x });

        // Second triangle
        // i + 1
        // i + range(x)
        // i + range(x) + 1
       // tris.push_back({ i + 1, i + (int)range_x, i + (int)range_x + 1 });
   // }
    //std::ofstream myfile;
    //myfile.open("fractal_3D.obj");
    //myfile << std::fixed;
    //bool obj = false;
    //if (obj)
    //{
    //    for (int i = 0; i < pts.size(); i++) {
    //        
    //        myfile << "v " << pts[i].X << " " << pts[i].Y << " " << pts[i].Z << std::endl;
    //    }
    //    //for (int i = 0; i < tris.size(); i++) {
    //      //  myfile << "f " << tris[i].p0 << " " << tris[i].p1 << " " << tris[i].p2 << std::endl;
    //   // }
    //}
    //else
    //{
    //    myfile << pts.size() << std::endl;
    //    for (int i = 0; i < pts.size(); i++) {
    //        myfile << pts[i].X << " " << pts[i].Y << " " << pts[i].Z << std::endl;
    //    }
    //}

   // myfile.close();

    //print(inner);
    //// std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //clear(inner);
    //printf("\033[%d;%dH", 0, 0); system("cls");
    ////if (j != 2*PI) system("cls");
    //std::cout << "J:" << j << std::endl;
#pragma region Pentagram


    double stepp = PI / 100.0;
    double two_pi = 2 * PI;
    double aLen = 0.2; system("cls");
    while (true)
    {
        j += stepp;
        draw_pentagram(inner, 0.6f, 0.f, 0.f, true);
        draw_line({ 0,0,0 }, { 0,aLen,0 }, inner, 'Y');
        draw_line({ 0,0,0 }, { aLen,0,0 }, inner, 'X');
        draw_line({ 0,0,0 }, { 0,0,aLen }, inner, 'Z');



        print(inner);
       // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        clear(inner);
        printf("\033[%d;%dH", 0, 0);
        //if (j != 2*PI) system("cls");
        std::cout << "J:" << j << std::endl;
    }

#pragma endregion
}