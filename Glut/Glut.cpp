#include <stdio.h>
#include "glut.h"
#include <fstream>
#include <Windows.h>

#pragma comment(lib,"glut32.lib")
#pragma comment(lib,"opengl32.lib")



// Game proc
//
//
///////////
///////////
///////////
///////////

class models
{
public:
	int m_sphere = 0, m_quad = 0, m_cone = 0;
	float* sphere_p = 0, * quad_p = 0, * cone_p = 0;
};


models* model;








//Графика
///////
////////
////////
//переменная для количества моделей
int N = 0, number = 0;
// угол поворота камеры
float angle = 0.0f, angley = 0.0f;
// координаты вектора направления движения камеры
float lx = 0.0f, lz = -1.0f, ly = 0.0f;
// XZ позиция камеры
float x = 0.0f, z = 5.5f, y = 1.65f;
//Ключи статуса камеры. Переменные инициализируются нулевыми значениями
//когда клавиши не нажаты
float deltaAngle = 0.0f;
float deltaAngley = 0.0f;
float deltaMove = 0;
int xOrigin = -1, yOrigin = -1;
//ширина и высота окна
int h = GetSystemMetrics(SM_CXSCREEN), w = GetSystemMetrics(SM_CYSCREEN);
//переменные для вычисления кадров в секунду
int frame;
long time1, time2, timebase, timebase1;
char s[50], g[50];
// переменные для хранения идентификаторв окна
int mainWindow, subWindow1;
//Граница между подокнами
int border = 6;

void setProjection(int w1, int h1)
{
	float ratio;
	// На ноль не делим
	ratio = 1.0f * w1 / h1;
	// обнуляем координаты матрицы перед модификацией
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//устанавливаем камеру
	glViewport(0, 0, w1, h1);

	// Установить объем отсечения
	gluPerspective(45, ratio, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void changeSize(int w1, int h1) {

	if (h1 == 0)
		h1 = 1;

	// сохраним эти значения
	w = w1;
	h = h1;

	// установить активным подокно 1
	glutSetWindow(subWindow1);
	// resize and reposition the sub window
	glutPositionWindow(border, border);
	glutReshapeWindow(w, h);
	setProjection(w, h);
}

void drawMan()
{
	// тело снеговика
	for (int i = 0; i < model[number].m_sphere; ++i)
	{
		glColor3f(model[number].sphere_p[9 * i], model[number].sphere_p[9 * i + 1], model[number].sphere_p[9 * i + 2]);
		glTranslatef(model[number].sphere_p[9 * i + 3], model[number].sphere_p[9 * i + 4], model[number].sphere_p[9 * i + 5]);
		glutSolidSphere(model[number].sphere_p[9 * i + 6], model[number].sphere_p[9 * i + 7], model[number].sphere_p[9 * i + 8]);
	}
	glPushMatrix();
	for (int i = 0; i < model[number].m_quad; ++i)
	{
		glColor3f(model[number].quad_p[9 * i], model[number].quad_p[9 * i + 1], model[number].quad_p[9 * i + 2]);
		glTranslatef(model[number].quad_p[9 * i + 3], model[number].quad_p[9 * i + 4], model[number].quad_p[9 * i + 5]);
		glutSolidSphere(model[number].quad_p[9 * i + 6], model[number].quad_p[9 * i + 7], model[number].quad_p[9 * i + 8]);
	}
	glPopMatrix();
	glPushMatrix();
	for (int i = 0; i < model[number].m_cone; ++i)
	{
		glColor3f(model[number].cone_p[14 * i], model[number].cone_p[14 * i + 1], model[number].cone_p[14 * i + 2]);
		glTranslatef(model[number].cone_p[14 * i + 3], model[number].cone_p[14 * i + 4], model[number].cone_p[14 * i + 5]);
		glRotatef(model[number].cone_p[14 * i + 6], model[number].cone_p[14 * i + 7], model[number].cone_p[14 * i + 8], model[number].cone_p[14 * i + 9]);
		glutSolidCone(model[number].cone_p[14 * i + 10], model[number].cone_p[14 * i + 11], model[number].cone_p[14 * i + 12], model[number].cone_p[14 * i + 13]);
	}
	glPopMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
}

void renderBitmapString(
	float x,
	float y,
	float z,
	void* font,
	char* string) {

	char* c;
	glRasterPos3f(x, y, z);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void restorePerspectiveProjection() {
	glMatrixMode(GL_PROJECTION);
	//восстановить предыдущую матрицу проекции
	glPopMatrix();
	//вернуться в режим модели
	glMatrixMode(GL_MODELVIEW);
}

void setOrthographicProjection() {
	//выбрать режим проекции
	glMatrixMode(GL_PROJECTION);
	//Сохраняем предыдущую матрицу, которая содерж
	//параметры перспективной проекции
	glPushMatrix();
	//обнуляем матрицу
	glLoadIdentity();
	//устанавливаем 2D ортогональную проекцию
	gluOrtho2D(0, w, h, 0);
	//выбираем режим обзора модели
	glMatrixMode(GL_MODELVIEW);
}

void computePos(float deltaMove) {
	x += deltaMove * lx * 0.1f;
	z += deltaMove * lz * 0.1f;
}

char board[10][10];
void map()
{
	int S = 1;
	srand(static_cast<unsigned int>(time(0)));
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			if (rand() % 5 + 1 == 4)
			{
				board[i][j] = 'E';
			}
			else if (rand() % 20 + 1 == 1 && S > 0 && i < 9 && j < 9)
			{
				board[i][j] = 'S';
				--S;
			}
			else
			{
				board[i][j] = ' ';
			}
		}
	}
}
int fig = 0;
int mobHP[9] = { 20, 50, 35, 65, 40, 10, 1, 500, 1000 };
int currentHP = 0;
int myAttack = 10;
int ij = 0, ji = 0;
void fight()
{
	++fig;
	if (fig % 2 == 1)
		currentHP = mobHP[rand() % 9];
}
// Общие элементы визуализации для всех подчиненных окон
void renderScene2() {

	// нарисуем "землю"

	glColor3f(0.2f, 0.6f, 0.2f);
	glBegin(GL_QUADS);
	glVertex3f(-100.0f, 0.0f, -100.0f);
	glVertex3f(-100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();



	// walls

	glColor3f(0.2f, 0.2f, 0.6f);

	glBegin(GL_QUADS);
	glVertex3f(-100.0f, 100.0f, -100.0f);
	glVertex3f(100.0f, 100.0f, -100.0f);
	glVertex3f(100.0f, 100.0f, 100.0f);
	glVertex3f(-100.0f, 100.0f, 100.0f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(-100.0f, 0.0f, -100.0f);
	glVertex3f(-100.0f, 100.0f, -100.0f);
	glVertex3f(-100.0f, 100.0f, 100.0f);
	glVertex3f(-100.0f, 0.0f, 100.0f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(-100.0f, 0.0f, 100.0f);
	glVertex3f(-100.0f, 100.0f, 100.0f);
	glVertex3f(100.0f, 100.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, 100.0f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 100.0f, 100.0f);
	glVertex3f(100.0f, 100.0f, -100.0f);
	glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(100.0f, 0.0f, -100.0f);
	glVertex3f(100.0f, 100.0f, -100.0f);
	glVertex3f(-100.0f, 100.0f, -100.0f);
	glVertex3f(-100.0f, 0.0f, -100.0f);
	glEnd();

	// Нарисуем 64 снеговика
	for (int i = -5; i < 5; i++)
		for (int j = -5; j < 5; j++) {
			glPushMatrix();
			glTranslatef(i * 5.0f, 0.0f, j * 5.0f);
			if (x > (i * 5.0f - 0.5f) && x < (i * 5.0f + 0.5f) && z >(j * 5.0f - 0.5f) && z < (j * 5.0f + 0.5f) && fig % 2 != 1 && board[i + 5][j + 5] == 'E')
			{
				fight();
				ij = i + 5;
				ji = j + 5;
			}
			if (board[i + 5][j + 5] == 'E')
			{
				drawMan();
			}
			glPopMatrix();
		}
}

//функция рендеринга основного окна
void renderScene() {
	glutSetWindow(mainWindow);
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}

// функция рендеринга для подокна 1
void renderScenesw1() {

	glutSetWindow(subWindow1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	frame++;
	time1 = glutGet(GLUT_ELAPSED_TIME);
	if (time1 - timebase > 1000) {

		sprintf_s(s, "\t FPS:%4.2f", frame * 1000.0 / (time1 - timebase));
		timebase = time1;
		frame = 0;
	}
	if (fig % 2 == 1)
	{
		sprintf_s(g, 50, "\t\nMob has %d HP\n", currentHP);
	}
	setOrthographicProjection();

	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(5, 30, 0, GLUT_BITMAP_HELVETICA_12, s);
	if (fig % 2 == 1)
	{
		renderBitmapString(100, 30, 0, GLUT_BITMAP_HELVETICA_12, g);
	}
	glPopMatrix();
	restorePerspectiveProjection();


	gluLookAt(x, y, z,
		x + lx, y + ly, z + lz,
		0.0f, 1.0f, 0.0f);

	renderScene2();
	glutSwapBuffers();
}
void timer(int)
{
	glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, timer, 0);
}

// Глобальная функция рендеринга
void renderSceneAll() {

	// проверить перемещения камеры от клавиатуры
	if (deltaMove) {
		computePos(deltaMove);
		glutSetWindow(mainWindow);
		glutPostRedisplay();
	}

	renderScene();
	renderScenesw1();
}

// -----------------------------------	//
//            клавиатура				//
// -----------------------------------	//

void processNormalKeys(unsigned char key, int xx, int yy) {

	if (key == 27) {
		glutDestroyWindow(mainWindow);
		exit(0);
	}
	if (key == 'r')
	{
		number = rand() % N;
		glutSetWindow(mainWindow);
		glutPostRedisplay();
	}
}

void pressKey(int key, int xx, int yy) {

	switch (key) {
	case GLUT_KEY_UP: deltaMove = 1.0f; break;
	case GLUT_KEY_DOWN: deltaMove = -1.0f; break;
	}
	glutSetWindow(mainWindow);
	glutPostRedisplay();

}

void releaseKey(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN: deltaMove = 0; break;
	}
}

// -----------------------------------	//
//            функции мыши				//
// -----------------------------------	//

void mouseMove(int x, int y) {

	// только когда левая кнопка опущена
	/*if (xOrigin >= 0 || yOrigin >= 0) {

		// обновить deltaAngle
		deltaAngle = (x - xOrigin) * 0.001f;
		deltaAngley = (y - yOrigin) * 0.001f;

		// обновить направление камеры
		lx = sin(angle + deltaAngle);
		ly = -sin(angley + deltaAngley);
		lz = -cos(angle + deltaAngle);

		glutSetWindow(mainWindow);
		glutPostRedisplay();
	}*/
}


void mouseMove1(int x, int y) {
	x += 6;
	y += 6;
	if (x != w / 2 && h != w / 2)
	{
		xOrigin = h / 2;
		yOrigin = w / 2;
		deltaAngle = (x - xOrigin) * 0.001f;
		deltaAngley = (y - yOrigin) * 0.001f;



		angle += deltaAngle;
		deltaAngle = 0.0f;
		angley += deltaAngley;
		deltaAngley = 0.0f;


		// обновить направление камеры
		lx = sin(angle);
		ly = -sin(angley);
		lz = -cos(angle);

		glutSetWindow(mainWindow);
		glutPostRedisplay();
		SetCursorPos(h / 2, w / 2);
	}
}


void attack()
{
	currentHP -= myAttack;
	if (currentHP <= 0)
	{
		board[ij][ji] = ' ';
		ij = 0;
		ji = 0;
		fight();
	}
}
void mouseButton(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			if (fig % 2 == 1)
			{
				time2 = glutGet(GLUT_ELAPSED_TIME);
				if (time2 - timebase1 > 1000)
				{
					attack();
					timebase1 = time2;
				}
			}
		}
	}
}

// ------------------------------------	//
//             main()					//
// -----------------------------------	//

void init() {

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// регистрация вызовов	
	glutTimerFunc(1000 / 60, timer, 0);
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove1);
	glutPassiveMotionFunc(mouseMove1);
}








#include <ctime>
int main(int argc, char** argv) {

	HWND window_header = GetConsoleWindow();
	//ShowWindow(window_header, SW_HIDE);

	srand(static_cast<int>(time(0)));


	map();
	std::ifstream fin;
	fin.open("models.txt");
	fin >> N;
	number = rand() % N;
	model = new models[N];
	for (int i = 0; i < N; ++i)
	{
		fin >> model[i].m_sphere >> model[i].m_quad >> model[i].m_cone;
		model[i].sphere_p = new float[(model[i].m_sphere) * 9];
		model[i].quad_p = new float[(model[i].m_quad) * 9];
		model[i].cone_p = new float[(model[i].m_cone) * 14];
		for (int j = 0; j < (model[i].m_sphere) * 9; ++j)
		{
			fin >> (model[i].sphere_p)[j];
		}
		for (int j = 0; j < (model[i].m_quad) * 9; ++j)
		{
			fin >> model[i].quad_p[j];
		}
		for (int j = 0; j < model[i].m_cone * 14; ++j)
		{
			fin >> model[i].cone_p[j];
		}

	}
	// инициализация GLUT и создание окна
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	mainWindow = glutCreateWindow("Урок 18");
	glutFullScreen();
	// регистрация вызовов для главного окна
	glutDisplayFunc(renderSceneAll);

	glutReshapeFunc(changeSize);

	init();

	// подокна
	subWindow1 = glutCreateSubWindow(mainWindow, border, border, w, h);
	glutDisplayFunc(renderScenesw1);
	init();
	SetCursorPos(h / 2, w / 2);
	ShowCursor(FALSE);


	// главный цикл
	glutMainLoop();

	return 1;
}
