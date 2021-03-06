#include <iostream>
#include <windows.h>
#include <conio.h>
using namespace std;

int main()
{
	// enumeration (перечисление - это набор именованных целочисленных констант)
	// MazeObject - пользовательский (кастомизированный) тип данных
	enum MazeObject { HALL, WALL, COIN, ENEMY, BORDER, HEALTHBOX, GRAVE };
	enum Color { PINK = 13, DARKGREEN = 2, YELLOW = 14, RED = 12, BLUE = 9, WHITE = 15, DARKYELLOW = 6, DARKRED = 4, BLACK = 0 };
	enum KeyCode { ENTER = 13, ESCAPE = 27, SPACE = 32, LEFT = 75, RIGHT = 77, UP = 72, DOWN = 80 };

	system("color 07");

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	// настройки шрифта консоли
	CONSOLE_FONT_INFOEX font; // https://docs.microsoft.com/en-us/windows/console/console-font-infoex
	font.cbSize = sizeof(font);
	font.dwFontSize.Y = 30;
	font.FontFamily = FF_DONTCARE;
	font.FontWeight = FW_NORMAL;
	wcscpy_s(font.FaceName, 9, L"Consolas");
	SetCurrentConsoleFontEx(h, 0, &font);

	// скрытие мигающего курсора 
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = false; // спрятать курсор
	cursor.dwSize = 1; // 1...100
	SetConsoleCursorInfo(h, &cursor);

	system("title Maze");
	MoveWindow(GetConsoleWindow(), 20, 60, 1850, 900, true);
	// 20 - отступ слева от левой границы рабочего стола до левой границы окна консоли (в пикселях!)
	// 60 - отступ сверху от верхней границы рабочего стола до верхней границы окна консоли
	// 1850 - ширина окна консоли в пикселях
	// 900 - высота окна консоли
	// true - перерисовать окно после перемещения

	srand(time(0));

	const int WIDTH = 40; // ширина лабиринта
	const int HEIGHT = 10; // высота лабиринта

	int maze[HEIGHT][WIDTH] = {}; // maze - лабиринт по-английски
	int coins_all = 0;                //amount all coins
	// алгоритм заполнения массива
	for (int y = 0; y < HEIGHT; y++) // перебор строк
	{
		for (int x = 0; x < WIDTH; x++) // перебор столбцов
		{
			maze[y][x] = rand() % 6; // 4 типа объектов в игре

			if (maze[y][x] == MazeObject::ENEMY) // если в лабиринте сгенерился враг
			{
				int probability = rand() % 15; // 0...14, если выпало 0 - враг останется, останется только одна пятая часть врагов
				if (probability != 0) // убираем врага
				{
					maze[y][x] = MazeObject::HALL; // на место врага ставим коридор
				}
			}

			if (maze[y][x] == MazeObject::WALL) // если в лабиринте сгенерировалась стена
			{
				int probability = rand() % 2; // 0...1, если выпало 0 - стена останется, останется только половина стен
				if (probability != 0) // убираем стену
				{
					maze[y][x] = MazeObject::HALL; // на место стены ставим коридор
				}
			}

			if (maze[y][x] == MazeObject::HEALTHBOX) // если в лабиринте сгенерился heal
			{
				int probability = rand() % 2;
				if (probability != 0)
				{
					maze[y][x] = MazeObject::HALL; // на место heal ставим коридор
				}
			}
			if (x == 0 || y == 0 || x == WIDTH - 1 || y == HEIGHT - 1)
				maze[y][x] = MazeObject::BORDER; // белая рамка

			if (x == 0 && y == 2 || x == 1 && y == 2 || x == 2 && y == 2)
				maze[y][x] = MazeObject::HALL; // вход
			if (x == WIDTH - 1 && y == HEIGHT - 3 ||
				x == WIDTH - 2 && y == HEIGHT - 3 ||
				x == WIDTH - 3 && y == HEIGHT - 3)
				maze[y][x] = MazeObject::HALL; // выход
			
		}
	}

	// показ лабиринта
	for (int y = 0; y < HEIGHT; y++) // перебор строк
	{
		for (int x = 0; x < WIDTH; x++) // перебор столбцов
		{
			switch (maze[y][x])
			{
			case MazeObject::HALL: // hall - коридор
				SetConsoleTextAttribute(h, Color::BLACK);
				cout << " ";
				break;

			case MazeObject::WALL: // wall - стена
				SetConsoleTextAttribute(h, Color::DARKGREEN);
				cout << (char)178;
				break;

			case MazeObject::BORDER: // border - рамка
				SetConsoleTextAttribute(h, Color::WHITE);
				cout << (char)178;
				break;

			case MazeObject::COIN: // coin - монетка
				SetConsoleTextAttribute(h, Color::YELLOW);
				cout << ".";
				coins_all++;
				break;

			case MazeObject::ENEMY: // enemy - враг
				SetConsoleTextAttribute(h, Color::RED);
				cout << (char)1;
				break;
			case MazeObject::HEALTHBOX:
				SetConsoleTextAttribute(h, 15 * 16 + 12);
				cout << "+";
				break;
			}
		}
		cout << "\n";
	}

	/////////////////////////////////////////////////////////////////////

	// размещение главного героя (ГГ)
	COORD position; // координаты нашего персонажа
	position.X = 0;
	position.Y = 2;
	SetConsoleCursorPosition(h, position);
	SetConsoleTextAttribute(h, Color::BLUE);
	cout << (char)2;

	int coins = 0; // счётчик собранных монет
	int health = 100; // количество очков здоровья главного героя
	int steps = 0;
	int energy = 150;

	/////////////////////////////////////////////////////////////////////
	// информация по всем показателям
	COORD infobox;
	infobox.X = WIDTH + 1;
	infobox.Y = 1;
	SetConsoleCursorPosition(h, infobox);
	SetConsoleTextAttribute(h, Color::DARKYELLOW);
	cout << "COINS: ";
	SetConsoleTextAttribute(h, Color::YELLOW);
	cout << coins << "\n"; // 0

	infobox.Y = 2;
	SetConsoleCursorPosition(h, infobox);
	SetConsoleTextAttribute(h, Color::DARKRED);
	cout << "HEALTH: ";
	SetConsoleTextAttribute(h, Color::RED);
	cout << health << "\n"; // 100

	// нормальный показ количества шагов
	infobox.Y = 3;
	SetConsoleCursorPosition(h, infobox);
	SetConsoleTextAttribute(h, Color::DARKYELLOW);
	cout << "STEPS: ";
	SetConsoleTextAttribute(h, Color::YELLOW);
	cout << steps << "\n";

	infobox.Y = 4;
	SetConsoleCursorPosition(h, infobox);
	SetConsoleTextAttribute(h, Color::DARKRED);
	cout << "ENERGY: ";
	SetConsoleTextAttribute(h, Color::RED);
	cout << energy << " \n";


	infobox.Y = 5;
	SetConsoleCursorPosition(h, infobox);
	SetConsoleTextAttribute(h, Color::DARKYELLOW);
	cout << "ALL COINS: ";
	SetConsoleTextAttribute(h, Color::YELLOW);
	cout << coins_all << "\n"; 

	while (true)
	{
		if (coins == coins_all)
		{
			int answer0 = MessageBoxA(0, "Vi sobrali vse monetki\n\nwant to play again?", "YOU WIN!", MB_YESNO);
			system("cls");
			if (answer0 == IDYES)
			{
				main();
			}
			else
			{
				exit(0);
			}
		}
		if (energy <= 0)
		{
			int answer2 = MessageBoxA(0, "energy is over!\n\nwant to play again?", "FAIL!", MB_YESNO);
			system("cls");
			if (answer2 == IDYES)
			{
				main();
			}
			else
			{
				exit(0);
			}
		}
		
		
		
		
		
		// keyboard hit
		if (_kbhit()) // если было нажатие на клавиши пользователем
		{
			int code = _getch(); // get character, получение кода нажатой клавиши
			if (code == 224) { // если это стрелка
				code = _getch(); // получить конкретный код стрелки
			}

			// стирание персонажика в старой позиции
			SetConsoleCursorPosition(h, position);
			if (maze[position.Y][position.X] == MazeObject::GRAVE)
			{
				SetConsoleTextAttribute(h, Color::PINK);
				cout << "X";
			}
			else
			{
				SetConsoleTextAttribute(h, Color::BLACK);
				cout << " ";
			}

			if (code == KeyCode::ENTER)
			{
				if (maze[position.Y][position.X + 1] == MazeObject::ENEMY)
				{
					energy -= 10;
					maze[position.Y][position.X + 1] = MazeObject::GRAVE;
					COORD effects;
					effects.X = position.X + 1;
					effects.Y = position.Y;
					SetConsoleCursorPosition(h, effects);
					SetConsoleTextAttribute(h, Color::PINK);
					cout << "X";
				}
				else if (maze[position.Y][position.X - 1] == MazeObject::ENEMY)
				{
					energy -= 10;
					maze[position.Y][position.X - 1] = MazeObject::GRAVE;
					COORD effects;
					effects.X = position.X - 1;
					effects.Y = position.Y;
					SetConsoleCursorPosition(h, effects);
					SetConsoleTextAttribute(h, Color::PINK);
					cout << "X";
				}
				else if (maze[position.Y + 1][position.X] == MazeObject::ENEMY)
				{
					energy -= 10;
					maze[position.Y + 1][position.X] = MazeObject::GRAVE;
					COORD effects;
					effects.X = position.X;
					effects.Y = position.Y + 1;
					SetConsoleCursorPosition(h, effects);
					SetConsoleTextAttribute(h, Color::PINK);
					cout << "X";
				}
				else if (maze[position.Y - 1][position.X] == MazeObject::ENEMY)
				{
					energy -= 10;
					maze[position.Y - 1][position.X] = MazeObject::GRAVE;
					COORD effects;
					effects.X = position.X;
					effects.Y = position.Y - 1;
					SetConsoleCursorPosition(h, effects);
					SetConsoleTextAttribute(h, Color::PINK);
					cout << "X";
				}
				else if (maze[position.Y - 1][position.X + 1] == MazeObject::ENEMY)
				{
					energy -= 10;
					maze[position.Y - 1][position.X + 1] = MazeObject::GRAVE;
					COORD effects;
					effects.X = position.X + 1;
					effects.Y = position.Y - 1;
					SetConsoleCursorPosition(h, effects);
					SetConsoleTextAttribute(h, Color::PINK);
					cout << "X";
				}
				else if (maze[position.Y - 1][position.X - 1] == MazeObject::ENEMY)
				{
					energy -= 10;
					maze[position.Y - 1][position.X - 1] = MazeObject::GRAVE;
					COORD effects;
					effects.X = position.X - 1;
					effects.Y = position.Y - 1;
					SetConsoleCursorPosition(h, effects);
					SetConsoleTextAttribute(h, Color::PINK);
					cout << "X";
				}
				else if (maze[position.Y + 1][position.X + 1] == MazeObject::ENEMY)
				{
					energy -= 10;
					maze[position.Y + 1][position.X + 1] = MazeObject::GRAVE;
					COORD effects;
					effects.X = position.X + 1;
					effects.Y = position.Y + 1;
					SetConsoleCursorPosition(h, effects);
					SetConsoleTextAttribute(h, Color::PINK);
					cout << "X";
				}
				else if (maze[position.Y + 1][position.X - 1] == MazeObject::ENEMY)
				{
					energy -= 10;
					maze[position.Y + 1][position.X - 1] = MazeObject::GRAVE;
					COORD effects;
					effects.X = position.X - 1;
					effects.Y = position.Y + 1;
					SetConsoleCursorPosition(h, effects);
					SetConsoleTextAttribute(h, Color::PINK);
					cout << "X";
				}
			}
			else if (code == KeyCode::ESCAPE)
			{
				cout << "ESCAPE!\n";
				exit(0);
			}
			else if (code == KeyCode::SPACE)
			{
				cout << "SPACE!\n";
				exit(0);
			}
			else if (code == KeyCode::LEFT
				&& maze[position.Y][position.X - 1] != MazeObject::WALL
				&& maze[position.Y][position.X - 1] != MazeObject::BORDER)
			{
				position.X--;
				steps++;
				energy--;
			}
			else if (code == KeyCode::RIGHT // если ГГ собрался пойти направо
				&& maze[position.Y][position.X + 1] != MazeObject::WALL
				&& maze[position.Y][position.X + 1] != MazeObject::BORDER)
				// и при этом в лабиринте на той же строке (где ГГ) и
				// немного (на одну ячейку) правее на 1 столбец от ГГ
			{
				position.X++;
				steps++;
				energy--;
			}
			else if (code == KeyCode::UP
				&& maze[position.Y - 1][position.X] != MazeObject::WALL
				&& maze[position.Y - 1][position.X] != MazeObject::BORDER)
			{
				position.Y--;
				steps++;
				energy--;
			}
			else if (code == KeyCode::DOWN
				&& maze[position.Y + 1][position.X] != MazeObject::WALL
				&& maze[position.Y + 1][position.X] != MazeObject::BORDER)
			{
				position.Y++;
				steps++;
				energy--;
			}

			// показ ГГ в новой позиции
			SetConsoleCursorPosition(h, position);
			SetConsoleTextAttribute(h, Color::BLUE);
			cout << (char)2;

			/////////////////////////////////////
			// нормальный показ количества шагов
			infobox.X = WIDTH + 1;
			infobox.Y = 3;
			SetConsoleCursorPosition(h, infobox);
			SetConsoleTextAttribute(h, Color::DARKYELLOW);
			cout << "STEPS: ";
			SetConsoleTextAttribute(h, Color::YELLOW);
			cout << steps << "\n";

			infobox.X = WIDTH + 1;
			infobox.Y = 4;
			SetConsoleCursorPosition(h, infobox);
			SetConsoleTextAttribute(h, Color::DARKRED);
			cout << "ENERGY: ";
			SetConsoleTextAttribute(h, Color::RED);
			cout << energy << " \n";


			infobox.Y = 5;
			SetConsoleCursorPosition(h, infobox);
			SetConsoleTextAttribute(h, Color::DARKYELLOW);
			cout << "ALL COINS: ";
			SetConsoleTextAttribute(h, Color::YELLOW);
			cout << coins_all << "\n"; 

			////////////////////////////////////////////////////////////////
			// пересечение с элементами массива
			if (position.Y == HEIGHT - 3 &&
				position.X == WIDTH - 1)
			{
				MessageBoxA(0, "you find exit!", "WIN!", 0);
				system("cls");
				main(); // для запуска сначала, но на другой рандомной локации
			}

			// пересечение с монетками
			// если в лабиринте по позиции ГГ (под ним) находится монетка
			if (maze[position.Y][position.X] == MazeObject::COIN)
			{
				coins++; // на одну монетку собрали больше
				infobox.X = WIDTH + 1;
				infobox.Y = 1;
				SetConsoleCursorPosition(h, infobox);
				SetConsoleTextAttribute(h, Color::DARKYELLOW);
				cout << "COINS: ";
				SetConsoleTextAttribute(h, Color::YELLOW);
				cout << coins << "\n";
				maze[position.Y][position.X] = MazeObject::HALL; // убираем монетку из лабиринта
			}

			// пересечение с врагами
			// если в лабиринте по позиции ГГ (под ним) находится враг
			if (maze[position.Y][position.X] == MazeObject::ENEMY)
			{
				health -= 20;
				infobox.X = WIDTH + 1;
				infobox.Y = 2;
				SetConsoleCursorPosition(h, infobox);
				SetConsoleTextAttribute(h, Color::DARKRED);
				cout << "HEALTH: ";
				SetConsoleTextAttribute(h, Color::RED);
				cout << health << " \n";
				maze[position.Y][position.X] = MazeObject::GRAVE; // убираем врага из лабиринта

				if (health <= 0)
				{
					int answer1 = MessageBoxA(0, "health is over!\n\nwant to play again?", "FAIL!", MB_YESNO);
					system("cls");
					if (answer1 == IDYES)
					{
						main();
					}
					else
					{
						exit(0);
					}
				}
				
			}
		}
		else // нажатия не было, двигаем врагов
		{
			Sleep(15);
			///////////////////////////////////////////////////////////////
			// движение врагов
			COORD enemy_positions[100]; // массив который хранит координаты врагов
			int enemy_count = 0; // реальное количество врагов на данный момент после шага ГГ

			// перебор всех ячеек которые есть в лабиринте
			for (int y = 0; y < HEIGHT; y++) // перебор строк
			{
				for (int x = 0; x < WIDTH; x++) // перебор столбцов
				{
					// если очередная ячейка - это враг
					if (maze[y][x] == MazeObject::ENEMY)
					{
						enemy_positions[enemy_count].X = x;
						enemy_positions[enemy_count].Y = y;
						enemy_count++;
					}
				}
			}

			// перебор всех врагов
			for (int i = 0; i < enemy_count; i++)
			{
				int r = rand() % 100; // 0-left, 1-right, 2-up, 3-down
				if (r == LEFT &&
					maze[enemy_positions[i].Y][enemy_positions[i].X - 1] != MazeObject::WALL &&
					maze[enemy_positions[i].Y][enemy_positions[i].X - 1] != MazeObject::BORDER &&
					maze[enemy_positions[i].Y][enemy_positions[i].X - 1] != MazeObject::ENEMY)
				{
					// стирание врага в старой позиции
					COORD temp = enemy_positions[i];
					SetConsoleCursorPosition(h, temp);
					if (maze[enemy_positions[i].Y][enemy_positions[i].X - 1] == MazeObject::COIN)
					{
						SetConsoleTextAttribute(h, Color::YELLOW);
						cout << ".";
						maze[enemy_positions[i].Y][enemy_positions[i].X] = MazeObject::COIN;
					}
					
					else
					{
						cout << " ";
						maze[enemy_positions[i].Y][enemy_positions[i].X] = MazeObject::HALL;
					}

					// перемещаем врага в новую позицию
					temp.X = enemy_positions[i].X - 1;
					temp.Y = enemy_positions[i].Y;
					SetConsoleCursorPosition(h, temp);
					SetConsoleTextAttribute(h, Color::RED);
					cout << (char)1;
					maze[enemy_positions[i].Y][enemy_positions[i].X - 1] = MazeObject::ENEMY;
				}
				else if (r == RIGHT &&
					maze[enemy_positions[i].Y][enemy_positions[i].X + 1] != MazeObject::WALL &&
					maze[enemy_positions[i].Y][enemy_positions[i].X + 1] != MazeObject::BORDER &&
					maze[enemy_positions[i].Y][enemy_positions[i].X + 1] != MazeObject::ENEMY)
				{
					// стирание врага в старой позиции
					COORD temp = enemy_positions[i];
					SetConsoleCursorPosition(h, temp);
					if (maze[enemy_positions[i].Y][enemy_positions[i].X + 1] == MazeObject::COIN)
					{
						SetConsoleTextAttribute(h, Color::YELLOW);
						cout << ".";
						maze[enemy_positions[i].Y][enemy_positions[i].X] = MazeObject::COIN;
					}

					else
					{
						cout << " ";
						maze[enemy_positions[i].Y][enemy_positions[i].X] = MazeObject::HALL;
					}

					// перемещаем врага в новую позицию
					temp.X = enemy_positions[i].X + 1;
					temp.Y = enemy_positions[i].Y;
					SetConsoleCursorPosition(h, temp);
					SetConsoleTextAttribute(h, Color::RED);
					cout << (char)1;
					maze[enemy_positions[i].Y][enemy_positions[i].X + 1] = MazeObject::ENEMY;
				}

				else if (r == UP &&
					maze[enemy_positions[i].Y - 1][enemy_positions[i].X] != MazeObject::WALL &&
					maze[enemy_positions[i].Y - 1][enemy_positions[i].X] != MazeObject::BORDER &&
					maze[enemy_positions[i].Y - 1][enemy_positions[i].X] != MazeObject::ENEMY)
				{
					// стирание врага в старой позиции
					COORD temp = enemy_positions[i];
					SetConsoleCursorPosition(h, temp);
					if (maze[enemy_positions[i].Y - 1][enemy_positions[i].X] == MazeObject::COIN)
					{
						SetConsoleTextAttribute(h, Color::YELLOW);
						cout << ".";
						maze[enemy_positions[i].Y][enemy_positions[i].X] = MazeObject::COIN;
					}

					else
					{
						cout << " ";
						maze[enemy_positions[i].Y][enemy_positions[i].X] = MazeObject::HALL;
					}

					// перемещаем врага в новую позицию
					temp.X = enemy_positions[i].X;
					temp.Y = enemy_positions[i].Y - 1;
					SetConsoleCursorPosition(h, temp);
					SetConsoleTextAttribute(h, Color::RED);
					cout << (char)1;
					maze[enemy_positions[i].Y - 1][enemy_positions[i].X] = MazeObject::ENEMY;
				}
				else if (r == DOWN &&
					maze[enemy_positions[i].Y + 1][enemy_positions[i].X] != MazeObject::WALL &&
					maze[enemy_positions[i].Y + 1][enemy_positions[i].X] != MazeObject::BORDER &&
					maze[enemy_positions[i].Y + 1][enemy_positions[i].X] != MazeObject::ENEMY)
				{
					// стирание врага в старой позиции
					COORD temp = enemy_positions[i];
					SetConsoleCursorPosition(h, temp);
					if (maze[enemy_positions[i].Y + 1][enemy_positions[i].X] == MazeObject::COIN)
					{
						SetConsoleTextAttribute(h, Color::YELLOW);
						cout << ".";
						maze[enemy_positions[i].Y][enemy_positions[i].X] = MazeObject::COIN;
					}

					else
					{
						cout << " ";
						maze[enemy_positions[i].Y][enemy_positions[i].X] = MazeObject::HALL;
					}

					// перемещаем врага в новую позицию
					temp.X = enemy_positions[i].X;
					temp.Y = enemy_positions[i].Y + 1;
					SetConsoleCursorPosition(h, temp);
					SetConsoleTextAttribute(h, Color::RED);
					cout << (char)1;
					maze[enemy_positions[i].Y + 1][enemy_positions[i].X] = MazeObject::ENEMY;
				}
			}
		}
	}
}
