#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <windows.h>

typedef struct object {
	float x, y; // координаты начала	
	float width, height; // ширина/высота
	float vertSpeed; // скорость падения (гравитация)
	BOOL IsFly; // предмет в полёте?
	char cType; // что за предмет
} Tobj;

#define mapHeight 29
#define mapWidth 115

char map[mapHeight][mapWidth + 1];
Tobj cube; 
Tobj *brick = NULL;
int bricklenght; // переменная для динамического выделения памяти блоков
int level = 1;

// заполнение карты пробелом 
void cleanmap() {
	for (int x = 0; x < mapWidth; x++) { 
		map[0][x] = ' ';
		map[0][mapWidth] = '\0';
	}
	for (int i = 1; i < mapHeight; i++) 
		sprintf(map[i], map[0]);
} 

// вывод карты 
void showmap() {
	map[mapHeight - 1][mapWidth - 1] = '\0';
	for (int i = 0; i < mapHeight; i++) 
		printf("%s\n", map[i]);
	
}

// начальная позиция объёекта
void PosObject(Tobj* obj, float xPos, float yPos) {
	(*obj).x = xPos;
	(*obj).y = yPos;
}

// назначение объекту шириины высоты, скорости падения и типа
void InitObject(Tobj* obj, float xPos, float yPos, float oWidth, float oHeight, char inType) {
	PosObject(obj, xPos, yPos);
	(*obj).width = oWidth;
	(*obj).height = oHeight;
	(*obj).vertSpeed = 0;
	(*obj).cType = inType;
}

// Объект не проходит через другой объект
BOOL SetCollision(Tobj o1, Tobj o2);

// Создание уровня
void CreateLevel(int lvl);

// гравитация
void VertSpeedMovement(Tobj* obj) {
	(*obj).IsFly = TRUE; // объект в полёте
	(*obj).vertSpeed += 0.05; // скорость падения 
	PosObject(obj, (*obj).x, (*obj).y + (*obj).vertSpeed); // новая позиция объекта на карте
	for (int x = 0; x < bricklenght; x++) // цикл прохождения по всем объекта типа "brick"
		if (SetCollision(*obj, brick[x])) { // проверка на столкновение, если да - скорость 0, объект не в полёте,
		(*obj).y -= (*obj).vertSpeed;
		(*obj).vertSpeed = 0;
		(*obj).IsFly = FALSE; 
		if (brick[x].cType == '+') { // проверка на столкновение с блоком типа "brick" но символом "+", если да - переход на некст лвл
			level++;
			if (level > 2) level = 1;
			CreateLevel(level);
			Sleep(1000);
		}
		break;
		}
}

// Чтобы объект не превысил границы карты
BOOL IsPosInMap(int x, int y) {
	return(((x >= 0) && (x < mapWidth)) && ((y >= 0) && (y < mapHeight)));
}

// размещение объектов на карте
void PutObject(Tobj obj){
	//начальные координаты
	int ix = (int)round(obj.x);
	int iy = (int)round(obj.y);

	//ширина высота 
	int iwidth = (int)round(obj.width);
	int iheight = (int)round(obj.height);
	
	//цикл заполнения карты
	for (int x = ix; x < (ix+iwidth); x++) {
		for (int y = iy; y < (iy + iheight); y++)
			if (IsPosInMap(x, y))
				map[y][x] = obj.cType;
	}
	
}

// движение карты (иллюзия движения персонажа)
void HorizonMapMove(float dx) {

	cube.x -= dx;
	for (int x = 0; x < bricklenght; x++)
		if (SetCollision(cube, brick[x])) {
			cube.x += dx;
			return;
		}
	cube.x += dx; 
	for (int x = 0; x < bricklenght;x++)
		brick[x].x += dx;
}

// Объект не проходит через другой объект
BOOL SetCollision(Tobj o1, Tobj o2) {
	return (((o1.x + o1.width) > o2.x) && (o1.x < (o2.x + o2.width) ) &&
			((o1.y + o1.height) > o2.y) && (o1.y < (o2.y + o2.height)));
}

// уровень
void CreateLevel(int lvl) {
	InitObject(&cube, 30, 10, 5, 3, '@'); // иниицализация объекта типа "персонаж"
	if (lvl == 1) { // 1 уровень
		bricklenght = 6; // кол-во блоков на уровне
		brick = realloc(brick, sizeof(*brick) * bricklenght); // выделеение под них память
		//         название   X   Y ширина высота символ 
		InitObject(brick + 0, 20, 20, 40, 5, '#');
		InitObject(brick + 1, 60, 15, 10, 10, '#');
		InitObject(brick + 2, 80, 20, 20, 5, '#');
		InitObject(brick + 3, 120, 15, 10, 10, '#');
		InitObject(brick + 4, 150, 20, 40, 5, '#');
		InitObject(brick + 5, 200, 23, 10, 4, '+');
	}
	if (lvl==2){ 
		bricklenght = 4;
		brick = realloc(brick, sizeof(*brick) * bricklenght);
		InitObject(brick + 0, 20, 20, 40, 5, '#');
		InitObject(brick + 1, 80, 20, 20, 5, '#');
		InitObject(brick + 2, 110, 20, 40, 5, '#');
		InitObject(brick + 3, 150, 12, 10, 4, '+');
	}
}

void setcur(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int main() {

	CreateLevel(level);
	system("color E"); // цвет консоли
	do {
		cleanmap();
		if ((cube.IsFly == FALSE)&&(GetKeyState(VK_SPACE) < 0)) cube.vertSpeed = -0.9;
		if (GetKeyState('A') < 0) HorizonMapMove(1);
		if (GetKeyState('D') < 0) HorizonMapMove(-1);
		if (cube.y> mapHeight) CreateLevel(level);
		VertSpeedMovement(&cube);
		for (int x = 0; x < bricklenght; x++)
			PutObject(brick[x]);
		PutObject(cube);
		setcur(0, 0);
		showmap();
		Sleep(2);
	} while (GetKeyState(VK_ESCAPE) >= 0);
}