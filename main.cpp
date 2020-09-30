 #if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <bits/stdc++.h>
#include <iostream>
#define ll long long
#define MAX_LOADSTRING 100
using namespace std;
#include <vector>
///////////////////////// line ///////////////////////////
void ParametricDrawLine(HDC hdc, int xs, int ys, int xe, int ye, COLORREF color)
{
    int deltax = xe - xs;
    int deltay = ye - ys;
    int n = max(abs(deltax), abs(deltay));

    double dt = 1.0 / n;
    double dx = dt * (double)deltax;
    double dy = dt * (double)deltay;

    double x = xs;
    double y = ys;

    for (int i = 0; i < n; i++)
    {
        SetPixel(hdc, round(x), round(y), color);
        x += dx;
        y += dy;
    }
}
//////////////////////// curve ////////////////////
void multiplyMatrixVector(int mat[],int vec[],int res[],int n)
{
    for(int i = 0 ; i < n ; i++)
    {
        res[i] = 0;
        for (int j = 0 ; j < n ; j++)
        {
            res[i]+= mat[i*n+j]*vec[j];
        }
    }
}

void drawCubicHermite(HDC hdc,POINT p0,POINT s0,POINT p1,POINT s1,COLORREF rgb)
{
    int n = 4;
    int xArr[] = {p0.x,s0.x,p1.x,s1.x}, yArr[] = {p0.y,s0.y,p1.y,s1.y};
    int mat[] = {2,1,-2,1, -3,-2,3,-1, 0,1,0,0,1,0,0,0};
    int coArrX[n],coArrY[n];
    multiplyMatrixVector(mat,xArr,coArrX,n);;
    multiplyMatrixVector(mat,yArr,coArrY,n);
    double x, y;
    for(double  t = 0 ; t <=1 ; t+=0.0001)
    {
        x = coArrX[3] + coArrX[2]*t + coArrX[1]*t*t+coArrX[0]*t*t*t;
        y = coArrY[3] + coArrY[2]*t + coArrY[1]*t*t+coArrY[0]*t*t*t;

        SetPixel(hdc,round(x),round(y),rgb);
    }
}
void PointClipping(HDC hdc,int x,int y,int xleft, int xright, int ybottom, int ytop ,COLORREF color)
{
    if(x>=xleft &&  x<= xright &&  y>=ybottom  &&  y<= ytop){
        SetPixel(hdc,x,y, RGB(0,0,255));
    }
    else{
         SetPixel(hdc,x,y,RGB(255,0,0));
     }
}
void drawCubicBezier1(HDC hdc,POINT p0,POINT p1,POINT p2,POINT p3, int xleft,int ytop,int xright,int ybottom, COLORREF rgb)
{
    int n = 4;
    int xArr[] = {p0.x,p1.x,p2.x,p3.x},yArr[] = {p0.y,p1.y,p2.y,p3.y};
    int mat[] = { -1,3,-3,1,3,-6,3,0, -3,3,0,0,1,0,0,0};
    int coArrX[n],coArrY[n];
    multiplyMatrixVector(mat,xArr,coArrX,n);;
    multiplyMatrixVector(mat,yArr,coArrY,n);
    double x, y;
    for(double  t = 0 ; t <=1 ; t+=0.0001)
    {
        x = coArrX[3] + coArrX[2]*t + coArrX[1]*t*t+coArrX[0]*t*t*t;
        y = coArrY[3] + coArrY[2]*t + coArrY[1]*t*t+coArrY[0]*t*t*t;
        PointClipping(hdc,round(x),round(y),  xleft, ytop, xright, ybottom,  rgb);
       // SetPixel(hdc,round(x),round(y),  xleft, ytop, xright, ybottom,  rgb);
    }
}
void drawCubicBezier2(HDC hdc,POINT p0,POINT p1,POINT p2,POINT p3,COLORREF rgb)
{
    POINT s0,s1 ;
    s0.x = 3 * (p1.x-p0.x), s0.y = 3 * (p1.y - p0.y),s1.x = 3 * (p3.x-p2.x), s1.y = 3 * (p3.y-p2.y) ;
    drawCubicHermite(hdc,p0,s0,p3,s1,rgb);
}

///////////////////////// curve clipping //////////////////////////
int Round(double x)
{
	return (int)(x + 0.5);
}
struct Vertex
{
	double x, y;
	Vertex(int x1 = 0, int y1 = 0)
	{
		x = x1;
		y = y1;
	}
};
typedef vector<Vertex> VertexList;
typedef bool (*IsInFunc)(Vertex& v, int edge);
typedef Vertex(*IntersectFunc)(Vertex& v1, Vertex& v2, int edge);
VertexList ClipWithEdge(VertexList p, int edge, IsInFunc In, IntersectFunc Intersect)
{
	VertexList OutList;
	if (p.size() == 0)return OutList;
	Vertex v1 = p[p.size() - 1];
	bool v1_in = In(v1, edge);
	for (int i = 0;i < (int)p.size();i++)
	{
		Vertex v2 = p[i];
		bool v2_in = In(v2, edge);
		if (!v1_in && v2_in)
		{
			OutList.push_back(Intersect(v1, v2, edge));
			OutList.push_back(v2);
		}
		else if (v1_in && v2_in)  OutList.push_back(v2);
		else if (v1_in) OutList.push_back(Intersect(v1, v2, edge));
		v1 = v2;
		v1_in = v2_in;
	}
	return OutList;
}
bool InLeft(Vertex& v, int edge)
{
	return v.x >= edge;
}
bool InRight(Vertex& v, int edge)
{
	return v.x <= edge;
}
bool InTop(Vertex& v, int edge)
{
	return v.y <= edge;
}
bool InBottom(Vertex& v, int edge)
{
	return v.y >= edge;
}
Vertex VIntersect(Vertex& v1, Vertex& v2, int xedge)
{
	Vertex res;
	res.x = xedge;
	res.y = v1.y + (xedge - v1.x) * (v2.y - v1.y) / (v2.x - v1.x);
	return res;
}

Vertex HIntersect(Vertex& v1, Vertex& v2, int yedge)
{
	Vertex res;
	res.y = yedge;
	res.x = v1.x + (yedge - v1.y) * (v2.x - v1.x) / (v2.y - v1.y);
	return res;
}
void CurveClip(HDC hdc, POINT* p, int n, int xleft, int xright, int ybottom, int ytop )
{
	VertexList vlist;
	for (int i = 0;i < n;i++)vlist.push_back(Vertex(p[i].x, p[i].y));
    drawCubicBezier1(hdc,p[0],p[1],p[2],p[3],  xleft, xright, ybottom, ytop,RGB(0,0,255));
}
void Draw8Point(HDC hdc, int xc, int yc, int a, int b, int xleft,int ytop,int xright,int ybottom, COLORREF color )
{
    PointClipping( hdc,xc + a ,yc + b , xleft, ytop, xright, ybottom,  color);
    PointClipping( hdc,xc - a ,yc + b ,  xleft,  ytop,  xright,  ybottom,  color);
    PointClipping( hdc,xc + a, yc - b  ,  xleft,  ytop,  xright,  ybottom,  color);
    PointClipping( hdc,xc - a , yc - b,  xleft,  ytop,  xright,  ybottom,  color);
    PointClipping( hdc, xc + b, yc + a ,  xleft,  ytop,  xright,  ybottom,  color);
    PointClipping( hdc, xc - b, yc + a,  xleft,  ytop,  xright,  ybottom,  color);
    PointClipping( hdc,xc + b, yc - a ,  xleft,  ytop,  xright,  ybottom,  color);
    PointClipping( hdc, xc - b, yc - a ,  xleft,  ytop,  xright,  ybottom,  color);
}

void CircleDirectPolar(HDC hdc, int xc, int yc, int R,int xleft,int ytop,int xright,int ybottom, COLORREF color)
{
    int x = R, y = 0;
    double theta = 0, dtheta = 1.0/R;

    while(x >= y)
    {
        //PointClipping( hdc, x, y, xleft, ytop, xright, ybottom,  color);
         Draw8Point(hdc,xc,yc,x,y, xleft, ytop, xright, ybottom,color);
        theta += dtheta;
        x = round(R * cos(theta));
        y = round(R * sin(theta));
    }
}

long dist(long x1, long y1, long x2, long y2)
{
    return sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
}

///////////////////////// line clipping //////////////////////////
union OutCode
{
    unsigned All:4;
    struct
    {
        unsigned left:1,top:1,right:1,bottom:1;
    };
};

OutCode GetOutCode(double x, double y, int xleft,int ytop,int xright,int ybottom)
{
    OutCode out;
    out.All=0;
    if(x < xleft) out.left = 1;
    else if(x > xright) out.right = 1;
    if(y < ytop) out.top = 1;
    else if(y > ybottom) out.bottom = 1;
    return out;
}

void VIntersect(double xs,double ys,double xe,double ye,int x,double *xi,double *yi)
{
    *xi = x; // x stands for xEdge .. left or right edges
    *yi = ys+(x-xs)*(ye-ys)/(xe-xs);
}
void HIntersect(double xs,double ys,double xe,double ye,int y,double *xi,double *yi)
{
    *yi=y;
    *xi=xs+(y-ys)*(xe-xs)/(ye-ys);
}

void CohenSuth(HDC hdc,int xs,int ys,int xe,int ye,int xleft,int ytop,int xright,int ybottom)
{
    double x1 = xs, y1 = ys, x2 = xe, y2 = ye;
    OutCode out1 = GetOutCode(x1,y1, xleft,ytop,xright,ybottom);
    OutCode out2=GetOutCode(x2,y2,xleft,ytop,xright,ybottom);
    //0000
    //0000


    while((out1.All || out2.All) && !(out1.All & out2.All) ) // check condition
    {
        double xi, yi;
        if(out1.All != 0)
        {
            if(out1.left) VIntersect(x1,y1,x1,y2,xleft,&xi, &yi);
            else if(out1.top)HIntersect(x1,y1,x2,y2,ytop,&xi,&yi);
            else if(out1.right)VIntersect(x1,y1,x2,y2,xright,&xi,&yi);
            else HIntersect(x1,y1,x2,y2,ybottom,&xi,&yi);
            x1 = xi;
            y1 = yi;
            out1=GetOutCode(x1,y1,xleft,ytop,xright,ybottom);
        }
        else
        {
            if(out2.left)VIntersect(x1,y1,x2,y2,xleft,&xi,&yi);
            else if(out2.top)HIntersect(x1,y1,x2,y2,ytop,&xi,&yi);
            else if(out2.right)VIntersect(x1,y1,x2,y2,xright,&xi,&yi);
            else HIntersect(x1,y1,x2,y2,ybottom,&xi,&yi);
            x2 = xi;
            y2 = yi;
            out2 = GetOutCode(x2,y2,xleft,ytop,xright,ybottom);
        }
    }
    if(!out1.All && !out2.All)
    {
     /*   MoveToEx(hdc,round(x1),round(y1),NULL);
        LineTo(hdc,round(x2),round(y2));
    }*/
    ParametricDrawLine(hdc,round(xs) ,round(ys) , round(x1) , round(y1)  ,RGB(255,0,0));
    ParametricDrawLine(hdc,round(x1) , round(y1) , round(x2),round(y2),RGB(0,0,255));
    ParametricDrawLine(hdc,round(x2) ,round(y2) , round(xe) , round(ye) ,RGB(255,0,0));
}
}
	static int choice=0;

#define ID_newWindow 1
#define ID_ClippingLine 2
#define ID_ClippingCurve 3
#define ID_ClippingCircle 4
#define ID_Exit 5
LONG WINAPI MyWindowProc(HWND hWnd, UINT mcode, WPARAM wp, LPARAM lp)
{
	HDC hdc;
	static int count = 0;
	const int num_verteces=4;
	static POINT p[10];
	static POINT p2[num_verteces];
	hdc = GetDC(hWnd);
    static PAINTSTRUCT ps;
	switch (mcode)
	{
	    case WM_LBUTTONDOWN:
            p[count].x = LOWORD(lp);
            p[count].y = HIWORD(lp);
             count++;
            break;
        break;
		//ReleaseDC(hWnd, hdc);
		//break;
	    case WM_CREATE:
        {
            HMENU hMenubar = CreateMenu();
            HMENU hWindow = CreateMenu();
            HMENU hLine = CreateMenu();
            HMENU hCircle = CreateMenu();
            HMENU hClipping = CreateMenu();
            HMENU hExit = CreateMenu();

            AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hWindow, "Window");
            AppendMenu(hWindow ,MF_STRING ,ID_newWindow , "NewWindow");
            AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hClipping, "Clipping");
            AppendMenu(hClipping, MF_STRING, ID_ClippingLine, "Clipping line");
            AppendMenu(hClipping, MF_STRING, ID_ClippingCircle, "Clipping Circle ");
            AppendMenu(hClipping , MF_STRING, ID_ClippingCurve , "Clipping Curve");
            AppendMenu(hMenubar ,MF_POPUP , ID_Exit, "Exit");

            SetMenu(hWnd, hMenubar);
            break;
        }
        case WM_COMMAND:
        {
            if(LOWORD(wp ) == ID_Exit)
            {
                exit(0);
                break;
            }
            else{
            choice = LOWORD(wp);
            }
        }
		case WM_PAINT:
		     if(choice==ID_newWindow){
                if(count == 2) {
                 //count = 0;
                    hdc = GetDC(hWnd);
                    Rectangle(hdc, p[0].x, p[1].y, p[1].x, p[0].y);
                }
		     }
		    else if (choice==ID_ClippingLine){
                if ( count == 4)
                {
               // count = 0;
                //Rectangle(hdc, p[0].x, p[1].y, p[1].x, p[0].y);
                CohenSuth(hdc, p[2].x, p[2].y, p[3].x, p[3].y,  p[0].x, p[1].y, p[1].x, p[0].y);
                }
            }
            else if(choice==ID_ClippingCircle){
                if ( count == 6)
                {
                   // count = 0;
                    int R=dist(p[4].x, p[4].y, p[5].x, p[5].y);
                    CircleDirectPolar( hdc, p[4].x, p[4].y ,  R, p[0].x, p[1].x ,p[1].y , p[0].y  , RGB(0,0,255));
                 }
            }

             else if(ID_ClippingCurve){
                for(int i=0 ;i<4; i++){
                    p2[i].x=p[i+6].x;
                    p2[i].y=p[i+6].y;
                }
                if ( count == 10)
                {
                    CurveClip(hdc, p2 ,4,p[0].x, p[1].x ,p[1].y , p[0].y );
                    count=0;
                }
             }
        EndPaint(hWnd , &ps);
		ReleaseDC(hWnd, hdc);
		break;
 	  case WM_DESTROY:PostQuitMessage(0); break;
	default: return DefWindowProc(hWnd, mcode, wp, lp);
	}
	return 0;//}
}
int main()
{

   cout<<"first, create new window "<<endl
	<<"second choose line clipping"<<endl
	<<"third choose circle clipping"<<endl
	<<"third choose curve clipping"<<endl
    <<"choose Exit to end program"<<endl;

   return WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOWNORMAL);

}
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nSh)
{
	WNDCLASSEX wcx;
	wcx.cbClsExtra = 0;
	wcx.cbSize = sizeof(wcx);
	wcx.cbWndExtra = 0;
	wcx.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcx.hInstance = hInst;
	wcx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wcx.lpfnWndProc = MyWindowProc;
	wcx.lpszClassName = "First";
	wcx.lpszMenuName = NULL;
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wcx);
	HWND hWnd;
	hWnd = CreateWindowEx(0, "first", "My First Window", WS_OVERLAPPEDWINDOW,
		0, 0, 800, 600, NULL, NULL, hInst, 0);
	ShowWindow(hWnd, nSh);
	UpdateWindow(hWnd);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
