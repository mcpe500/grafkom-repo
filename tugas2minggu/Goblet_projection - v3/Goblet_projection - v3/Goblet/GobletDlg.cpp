
// GobletDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Goblet.h"
#include "GobletDlg.h"
#include "afxdialogex.h"
#include <list>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGobletDlg dialog



CGobletDlg::CGobletDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGobletDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGobletDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGobletDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_WM_CREATE()
//	ON_WM_ACTIVATE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON1, &CGobletDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CGobletDlg message handlers

BOOL CGobletDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGobletDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


struct VERTEX3D { double x, y, z; };
struct VERTEX2D { double x, y; };

VERTEX2D vertices2d[] = { // data outline goblet , koordinat x dan y
	{ -0.0, 0.04783},
	{ 0.22824, 0.04783},
	{ 0.57849, 0.15744},
	{ 0.90865, 0.3357},
	{ 0.96506, 0.62667},
	{ 0.89239, 0.94034},
	{ 0.8846, 0.97398},
	{ 0.92678, 1.0},
	{ 0.93404, 0.96867},
	{ 1.01108, 0.63612},
	{ 0.95198, 0.33129},
	{ 0.60607, 0.14453},
	{ 0.23912, 0.02969},
	{ 0.11688, -0.05938},
	{ 0.11704, -0.1736},
	{ 0.11732, -0.36388},
	{ 0.11761, -0.56139},
	{ 0.11781, -0.69735},
	{ 0.15124, -0.84351},
	{ 0.34747, -0.86595},
	{ 0.64182, -0.92559},
	{ 0.66638, -0.97007},
	{ 0.67456, -0.99729},
	{ 0.1, -0.99729 },
	{ -0.0, -0.99729}
};

int flag = 0;
struct TRIANGLE { int p1, p2, p3; };
struct TRANSFORM { double t[4][4]; /* semestinya ada inverse-nya juga */ };
TRANSFORM t;  // matrix gabungan transformasi objek
TRANSFORM pro;  // matrix gabungan transformasi kamera / matrix proyeksi
std::list <VERTEX3D> vertices3d;
std::list <TRIANGLE> triangles;
CWnd *cwnd;
CDC *cdc;
int jumvertices2d = 0;
char debug[1000];
int segmen = 12;

// parameter kamera
VERTEX3D campos, lookat, up; // posisi arah, pandang dan orientasi
VERTEX3D look; // vektor arah pandang kamera (lookat-campos)
double tw, th; // sudut bukaan horisontal (tw) dan vertikal (th)
double nearclip, farclip; // near dan far clip               


// menggambar penampang di layar
void gambar_penampang(void)
{
	cdc->MoveTo((int)(round(100 * vertices2d[0].x + 100)),
		(int)(round(-100 * vertices2d[0].y + 150)));
	for (int i = 1; i < jumvertices2d; i++)
		cdc->LineTo((int)(round(100 * vertices2d[i].x + 100)), 
					 (int)(round(-100 * vertices2d[i].y + 150)));
}

// menyusun vertex-vertex goblet dg merotasikan data outline
// membuat definisi segitiga2 (face) penyusun goblet dari vertex yg telah dibuat
void susun_vertices(void)
{
	// kalikan 100 biar agak besar ... habis data koordinat semua nol koma :(
	for (int i = 0; i < jumvertices2d; i++) {
		vertices2d[i].x *= 100;
		vertices2d[i].y *= 100;
	}

	for (int i = 0; i < jumvertices2d; i++) {
		double sudut = 0;
		while (sudut < 360.0) {
			VERTEX3D baru;
			baru.x = vertices2d[i].x * cos(sudut*3.14 / 180.0);
			baru.y = vertices2d[i].y;
			baru.z = vertices2d[i].x * sin(sudut*3.14 / 180.0);;
			vertices3d.push_back(baru);
			sudut += (360.0/segmen);
		}
	}

	// susun face
	TRIANGLE t;
	int s, y;
	for (y = 0; y < (jumvertices2d - 1); y++) {
		for (s = 0; s < (segmen - 1); s++) {
			t.p1 = s + y*segmen;
			t.p2 = t.p1 + 1;
			t.p3 = t.p2 + segmen;
			triangles.push_back(t);
			t.p1 = t.p3;
			t.p2 = t.p1 - 1;
			t.p3 = s + y*segmen;
			triangles.push_back(t);
		}
		s = segmen - 1;
		t.p1 = s + y*segmen;
		t.p2 = t.p1 - segmen + 1;
		t.p3 = segmen * y;
		triangles.push_back(t);
		t.p1 = t.p1 + 1;
		t.p2 = t.p1 + segmen - 1;
		t.p3 = t.p1 - 1;
		triangles.push_back(t);
	}
}

// menggambar vertex2 di layar
// tanpa kamera, kita harus menggeser posisi berdasar z 
// agar tidak tumpang tindih
// menggunakan list ... bukan array, jadi lambat
void gambar3Dvertex(void)
{
	//int jum = vertices3d.size;
	for (std::list <VERTEX3D>::iterator it = vertices3d.begin(); it != vertices3d.end(); ++it)
	{
		double z = it->z;
		double x = it->x * 80 +z * 15    + 150;
		double y = -it->y * 80+ z * 10   + 150;
		cdc->SetPixel(x,y, RGB(255,0,0));
	}
}

// menggambar semua segitiga di layar
// tanpa kamera, kita harus menggeser posisi berdasar z 
// agar tidak tumpang tindih
void gambarwireframe(void)
{
	for (std::list <TRIANGLE>::iterator it = triangles.begin(); it != triangles.end(); ++it)
	{
		int p1 = it->p1; int p2 = it->p2; int p3 = it->p3;

		std::list <VERTEX3D>::iterator itx = vertices3d.begin();
		std::advance(itx, p1); 
		double x1 = itx->x; double y1 = itx->y; double z1 = itx->z;

		itx = vertices3d.begin();
		std::advance(itx, p2);
		double x2 = itx->x; double y2 = itx->y; double z2 = itx->z;

		itx = vertices3d.begin();
		std::advance(itx, p3);
		double x3 = itx->x; double y3 = itx->y; double z3 = itx->z;

		x1 =  x1 * 80 + z1 * 15 + 150;
		y1 = -y1 * 80 + z1 * 10 + 150;

		x2 = x2 * 80 + z2 * 15 + 150;
		y2 = -y2 * 80 + z2 * 10 + 150;

		x3 = x3 * 80 + z3 * 15 + 150;
		y3 = -y3 * 80 + z3 * 10 + 150;

		cdc->MoveTo(x1, y1); 
		cdc->LineTo(x2, y2); cdc->LineTo(x3, y3); cdc->LineTo(x1, y1);
	}
}

// perkalian matrix
void multiply(TRANSFORM &t, TRANSFORM s)
{
	TRANSFORM tmp;
	for (int baris = 0; baris < 4; baris++)
	for (int kolom = 0; kolom < 4; kolom++)
		tmp.t[kolom][baris] = t.t[kolom][baris];

	for (int baris = 0; baris < 4; baris++)
	{
		for (int kolom = 0; kolom < 4; kolom++)
		{
			t.t[kolom][baris] = 0.0;
			for (int cnt = 0; cnt< 4; cnt++)
				t.t[kolom][baris] += tmp.t[cnt][baris] * s.t[kolom][cnt];
		}
	}
}

// menghitung cross product antara 2 vektor
void Cross(VERTEX3D v1, VERTEX3D v2, VERTEX3D &hasil)
{
	hasil.x = v1.y * v2.z - v1.z * v2.y;
	hasil.y = v1.z * v2.x - v1.x * v2.z;
	hasil.z = v1.x * v2.y - v1.y * v2.x;
}

// melakukan normalisasi vektor, membuat panjang vektor menjadi 1 unit (vektor satuan)
void Normalize(VERTEX3D &v)
{
	double panjang = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	v.x /= panjang; v.y /= panjang; v.z /= panjang;
}

// inisialisasi matiex transformasi dg matrix identitas
void identity(TRANSFORM &t)
{
	t.t[0][0] = 1.0; t.t[1][0] = 0.0; t.t[2][0] = 0.0; t.t[3][0] = 0.0;
	t.t[0][1] = 0.0; t.t[1][1] = 1.0; t.t[2][1] = 0.0; t.t[3][1] = 0.0;
	t.t[0][2] = 0.0; t.t[1][2] = 0.0; t.t[2][2] = 1.0; t.t[3][2] = 0.0;
	t.t[0][3] = 0.0; t.t[1][3] = 0.0; t.t[2][3] = 0.0; t.t[3][3] = 1.0;
}

// membuat matrix translasi berdasar pergeseran tx, ty dan tz
// mengalikan dg matrix gabungan 't'
void translate(TRANSFORM &t, double tx, double ty, double tz)
{
	TRANSFORM g;
	g.t[0][0] = 1.0; g.t[1][0] = 0.0; g.t[2][0] = 0.0; g.t[3][0] = tx;
	g.t[0][1] = 0.0; g.t[1][1] = 1.0; g.t[2][1] = 0.0; g.t[3][1] = ty;
	g.t[0][2] = 0.0; g.t[1][2] = 0.0; g.t[2][2] = 1.0; g.t[3][2] = tz;
	g.t[0][3] = 0.0; g.t[1][3] = 0.0; g.t[2][3] = 0.0; g.t[3][3] = 1.0;
	multiply(t, g);
}

// membuat matrix scaling berdasar perubahan ukuran sx, sy dan sz
// mengalikan dg matrix gabungan 't'
void scale(TRANSFORM &t, double sx, double sy, double sz)
{
	TRANSFORM s;
	s.t[0][0] = sx;  s.t[1][0] = 0.0; s.t[2][0] = 0.0; s.t[3][0] = 0.0;
	s.t[0][1] = 0.0; s.t[1][1] = sy;  s.t[2][1] = 0.0; s.t[3][1] = 0.0;
	s.t[0][2] = 0.0; s.t[1][2] = 0.0; s.t[2][2] = sz;  s.t[3][2] = 0.0;
	s.t[0][3] = 0.0; s.t[1][3] = 0.0; s.t[2][3] = 0.0; s.t[3][3] = 1.0;
	multiply(t, s);
}

// membuat matrix rotasi 'r' derajat berdasar sumbu x
// mengalikan dg matrix gabungan 't'
void rotatex(TRANSFORM &t, double r)
{
	TRANSFORM s;
	r = r * 3.14 / 180.0;
	s.t[0][0] = 1.0; s.t[1][0] = 0.0; s.t[2][0] = 0.0; s.t[3][0] = 0.0;
	s.t[0][1] = 0.0; s.t[1][1] = cos(r); s.t[2][1] =-sin(r); s.t[3][1] = 0.0;
	s.t[0][2] = 0.0; s.t[1][2] = sin(r); s.t[2][2] = cos(r); s.t[3][2] = 0.0;
	s.t[0][3] = 0.0; s.t[1][3] = 0.0; s.t[2][3] = 0.0; s.t[3][3] = 1.0;
	multiply(t, s);
}

// membuat matrix rotasi 'r' derajat berdasar sumbu y
// mengalikan dg matrix gabungan 't'
void rotatey(TRANSFORM &t, double r)
{
	TRANSFORM s;
	r = r * 3.14 / 180.0;
	s.t[0][0] = cos(r); s.t[1][0] = 0.0; s.t[2][0] = sin(r); s.t[3][0] = 0.0;
	s.t[0][1] = 0.0; s.t[1][1] = 1.0; s.t[2][1] = 0.0; s.t[3][1] = 0.0;
	s.t[0][2] = -sin(r); s.t[1][2] = 0.0; s.t[2][2] = cos(r); s.t[3][2] = 0.0;
	s.t[0][3] = 0.0; s.t[1][3] = 0.0; s.t[2][3] = 0.0; s.t[3][3] = 1.0;
	multiply(t, s);
}

// membuat matrix rotasi 'r' derajat berdasar sumbu z
// mengalikan dg matrix gabungan 't'
void rotatez(TRANSFORM &t, double r)
{
	TRANSFORM s;
	r = r * 3.14 / 180.0;
	s.t[0][0] = cos(r); s.t[1][0] = -sin(r); s.t[2][0] = 0.0; s.t[3][0] = 0.0;
	s.t[0][1] = sin(r); s.t[1][1] = cos(r); s.t[2][1] = 0.0; s.t[3][1] = 0.0;
	s.t[0][2] = 0.0; s.t[1][2] = 0.0; s.t[2][2] = 1.0; s.t[3][2] = 0.0;
	s.t[0][3] = 0.0; s.t[1][3] = 0.0; s.t[2][3] = 0.0; s.t[3][3] = 1.0;
	multiply(t, s);
}

// mengalikan koordinat asal vertex dengan matrix transformasi
void transform(VERTEX3D &v, TRANSFORM t)
{
	VERTEX3D tmp;
	tmp.x = v.x; tmp.y = v.y; tmp.z = v.z;

	v.x = t.t[0][0] * tmp.x + t.t[1][0] * tmp.y + t.t[2][0] * tmp.z + t.t[3][0];
	v.y = t.t[0][1] * tmp.x + t.t[1][1] * tmp.y + t.t[2][1] * tmp.z + t.t[3][1];
	v.z = t.t[0][2] * tmp.x + t.t[1][2] * tmp.y + t.t[2][2] * tmp.z + t.t[3][2];
}

// mengambil data vertex, melakukan transformasi dg matrix transformasi gabungan 't'
// menggambar di layar berdasar koordinat yg telah mengalami transformasi
void gambargoblet()
{
	for (std::list <TRIANGLE>::iterator it = triangles.begin(); it != triangles.end(); ++it)
	{
		int p1 = it->p1; int p2 = it->p2; int p3 = it->p3;
		VERTEX3D tmp1, tmp2, tmp3;

		std::list <VERTEX3D>::iterator itx = vertices3d.begin();
		std::advance(itx, p1);
		tmp1.x = itx->x; tmp1.y = itx->y; tmp1.z = itx->z;
		transform(tmp1, t);
		transform(tmp1, pro);

		itx = vertices3d.begin();
		std::advance(itx, p2);
		tmp2.x = itx->x; tmp2.y = itx->y; tmp2.z = itx->z;
		transform(tmp2, t);
		transform(tmp2, pro);

		itx = vertices3d.begin();
		std::advance(itx, p3);
		tmp3.x = itx->x; tmp3.y = itx->y; tmp3.z = itx->z;
		transform(tmp3, t);
		transform(tmp3, pro);

		int LAYAR = 400; // skala ke ukuran layar, semestinya bisa otomatis dicari dari ukuran canvas :P

		// coba perhatikan...
		// hasil proyeksi ternyata tidak menampilkan efek perspektif
		// hingga semua koordinat x & y harus dibagi dg z
		// kenapa ya?
		cdc->MoveTo(tmp1.x / tmp1.z *LAYAR + 250, 250 - tmp1.y / tmp1.z *LAYAR);
		cdc->LineTo(tmp2.x / tmp2.z *LAYAR + 250, 250 - tmp2.y / tmp2.z *LAYAR);
		cdc->LineTo(tmp3.x / tmp3.z *LAYAR + 250, 250 - tmp3.y / tmp3.z *LAYAR);
		cdc->LineTo(tmp1.x / tmp1.z *LAYAR + 250, 250 - tmp1.y / tmp1.z *LAYAR);

	}

}


void siapkan_matrix_proyeksi(void)
{
	TRANSFORM pos; // untuk memindah kamera ke 0,0,0
	pos.t[0][0] = 1.0; pos.t[1][0] = 0.0; pos.t[2][0] = 0.0; pos.t[3][0] = -campos.x;
	pos.t[0][1] = 0.0; pos.t[1][1] = 1.0; pos.t[2][1] = 0.0; pos.t[3][1] = -campos.y;
	pos.t[0][2] = 0.0; pos.t[1][2] = 0.0; pos.t[2][2] = 1.0; pos.t[3][2] = -campos.z;
	pos.t[0][3] = 0.0; pos.t[1][3] = 0.0; pos.t[2][3] = 0.0; pos.t[3][3] = 1.0;

	TRANSFORM m; // untuk memutar arah pandang kamera ke -z
	VERTEX3D u, v, n; // sumbu lokal kamera
	// karena u,v,n adalah ortonormal, dan n = -look, maka
	// u = up x look  ---> jangan sampai look & up sejajar, hindari menoleh ke 0,1,0
	// v = loo x u
	look.x = lookat.x - campos.x;
	look.y = lookat.y - campos.y;
	look.z = lookat.z - campos.z;
	n.x = -look.x; n.y = -look.y; n.z = -look.z; Normalize(n); // n = -lookat
	Cross(up, look, u); Normalize(u); // u = up X lookat
	Cross(look, u, v); Normalize(v); // v = lookat X u
	m.t[0][0] = u.x; m.t[1][0] = u.y; m.t[2][0] = u.z; m.t[3][0] = 0.0;
	m.t[0][1] = v.x; m.t[1][1] = v.y; m.t[2][1] = v.z; m.t[3][1] = 0.0;
	m.t[0][2] = n.x; m.t[1][2] = n.y; m.t[2][2] = n.z; m.t[3][2] = 0.0;
	m.t[0][3] = 0.0; m.t[1][3] = 0.0; m.t[2][3] = 0.0; m.t[3][3] = 1.0;

	TRANSFORM S_1; // matrix scaling sudut pandang
	double sw = 1.0/tan((3.14*tw/180.0)/2);
	double sh = 1.0 / tan((3.14*th / 180.0) / 2);
	S_1.t[0][0] = sw;  S_1.t[1][0] = 0.0; S_1.t[2][0] = 0.0; S_1.t[3][0] = 0.0;
	S_1.t[0][1] = 0.0; S_1.t[1][1] = sh;  S_1.t[2][1] = 0.0; S_1.t[3][1] = 0.0;
	S_1.t[0][2] = 0.0; S_1.t[1][2] = 0.0; S_1.t[2][2] = 1.0; S_1.t[3][2] = 0.0;
	S_1.t[0][3] = 0.0; S_1.t[1][3] = 0.0; S_1.t[2][3] = 0.0; S_1.t[3][3] = 1.0;

	TRANSFORM far_1; // matrix scaling 1/far
	far_1.t[0][0] = 1.0/farclip; far_1.t[1][0] = 0.0; far_1.t[2][0] = 0.0; far_1.t[3][0] = 0.0;
	far_1.t[0][1] = 0.0; far_1.t[1][1] = 1.0 / farclip; far_1.t[2][1] = 0.0; far_1.t[3][1] = 0.0;
	far_1.t[0][2] = 0.0; far_1.t[1][2] = 0.0; far_1.t[2][2] = 1.0 / farclip; far_1.t[3][2] = 0.0;
	far_1.t[0][3] = 0.0; far_1.t[1][3] = 0.0; far_1.t[2][3] = 0.0; far_1.t[3][3] = 1.0;

	TRANSFORM D; // matrix perspektif, mengubah bentuk frustum jadi kotak
	double k = nearclip / farclip;
	D.t[0][0] = 1.0; D.t[1][0] = 0.0; D.t[2][0] = 0.0; D.t[3][0] = 0.0;
	D.t[0][1] = 0.0; D.t[1][1] = 1.0; D.t[2][1] = 0.0; D.t[3][1] = 0.0;
	D.t[0][2] = 0.0; D.t[1][2] = 0.0; D.t[2][2] = 1.0 / (k - 1.0); D.t[3][2] = k / (k - 1.0);
	D.t[0][3] = 0.0; D.t[1][3] = 0.0; D.t[2][3] = -1; D.t[3][3] = 1.0;

	// gabungkan semua
	// ingat: perkalian matrix dari belakang
	identity(pro);
	multiply(pro, D); 
	multiply(pro, far_1);
	multiply(pro, S_1); multiply(pro, m); multiply(pro, pos);
}

void definisi_kamera(void)
{
	campos.x = 100.0; campos.y = 400.0; campos.z = -1000.0; // posisi kamera
	lookat.x = 100.0; lookat.y = 0.0; lookat.z = 0.0; // memandang ke koordinat mana
	up.x = 0.0; up.y = 1.0; up.z = 0.0; // up = 010 berarti kamera tegak
	tw = 60.0; th = 60.0;
	nearclip = 0.1; farclip = 10000.0;
}

void gambar3goblet()
{

	// susun transformasi goblet 1
	identity(t);
	gambargoblet();

	// susun transformasi goblet 2
	identity(t);
	translate(t, 140, 80, 200);
	scale(t, 0.70, 2.0, 0.70);
	gambargoblet();

	// susun transformasi goblet 3
	identity(t);
	translate(t, 280, 0, 0);
	rotatey(t, 150);
	rotatex(t, 90);
	gambargoblet();

}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGobletDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		cwnd = GetDlgItem(IDC_PIC);
		cdc = cwnd->GetDC();
		jumvertices2d = sizeof(vertices2d) / sizeof(double) / 2;

		//gambar_penampang();
		//gambar3Dvertex();
		//gambarwireframe();
		susun_vertices();
		definisi_kamera();
		siapkan_matrix_proyeksi();
		gambar3goblet();

	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGobletDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CGobletDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	// ambil angka dari edit box TANPA VALIDASI ... pastikan memasukkan angka yg benar :3
	if (!flag) {
		flag = 1;

		CWnd *c;
		c = GetDlgItem(IDC_POS1);
		c->SetWindowTextW(L"100.0");
		c = GetDlgItem(IDC_POS2);
		c->SetWindowTextW(L"400.0");
		c = GetDlgItem(IDC_POS3);
		c->SetWindowTextW(L"-1000.0");

		c = GetDlgItem(IDC_L1);
		c->SetWindowTextW(L"100.0");
		c = GetDlgItem(IDC_L2);
		c->SetWindowTextW(L"0.0");
		c = GetDlgItem(IDC_L3);
		c->SetWindowTextW(L"0.0");

		c = GetDlgItem(IDC_UP1);
		c->SetWindowTextW(L"0.0");
		c = GetDlgItem(IDC_UP2);
		c->SetWindowTextW(L"1.0");
		c = GetDlgItem(IDC_UP3);
		c->SetWindowTextW(L"0.0");

		c = GetDlgItem(IDC_TW);
		c->SetWindowTextW(L"60.0");
		c = GetDlgItem(IDC_TH);
		c->SetWindowTextW(L"60.0");

		c = GetDlgItem(IDC_NEAR);
		c->SetWindowTextW(L"1.0");
		c = GetDlgItem(IDC_FAR);
		c->SetWindowTextW(L"1000.0");
	}

}


void CGobletDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	CWnd *c;
	CDC *cdc;
	CString s; 
	char buf[256];
	RECT r;

	c = GetDlgItem(IDC_POS1); c->GetWindowText(s); campos.x = _wtof(s);
	c = GetDlgItem(IDC_POS2); c->GetWindowText(s); campos.y = _wtof(s);
	c = GetDlgItem(IDC_POS3); c->GetWindowText(s); campos.z = _wtof(s);

	c = GetDlgItem(IDC_L1); c->GetWindowText(s); lookat.x = _wtof(s);
	c = GetDlgItem(IDC_L2); c->GetWindowText(s); lookat.y = _wtof(s);
	c = GetDlgItem(IDC_L3); c->GetWindowText(s); lookat.z = _wtof(s);

	c = GetDlgItem(IDC_UP1); c->GetWindowText(s); up.x = _wtof(s);
	c = GetDlgItem(IDC_UP2); c->GetWindowText(s); up.y = _wtof(s);
	c = GetDlgItem(IDC_UP3); c->GetWindowText(s); up.z = _wtof(s);

	c = GetDlgItem(IDC_TW); c->GetWindowText(s); tw = _wtof(s);
	c = GetDlgItem(IDC_TH); c->GetWindowText(s); th = _wtof(s);

	c = GetDlgItem(IDC_NEAR); c->GetWindowText(s); nearclip = _wtof(s);
	c = GetDlgItem(IDC_FAR); c->GetWindowText(s); farclip = _wtof(s);

	// NO CLIP ... gambar bisa keluar kanvas
	// agar tetap dalam kanvas, buatlah offscreen, gambar di sana lalu bitblt-kan ke kanvas screen
	c = GetDlgItem(IDC_PIC);
	c->GetClientRect(&r);
	cdc = c->GetDC();
	cdc->FillSolidRect(&r, GetSysColor(COLOR_BTNFACE));

	siapkan_matrix_proyeksi();
	gambar3goblet();
}
