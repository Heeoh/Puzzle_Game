#define _CRT_SECURE_NO_WARNINGS
#include <bangtal.h>
using namespace bangtal;
#include <iostream>
using namespace std;
#include <ctime>
#include <cmath>
#include <fstream>
#include <cstring>



typedef struct {
	ObjectPtr object;
	int location;
}Puzzle;

typedef struct {
	ScenePtr scene;
	int x, xd, y, yd, sqrtN, num;  //sqrtN = 한 줄에 있는 퍼즐의 개수
	float* bestT;
}Info;

typedef struct {
	Puzzle* board;
	Puzzle blank;
	Info info;
	ObjectPtr start, homebutton, restart;
} Game;




const int N_of_Puzzle = 2;	// 퍼즐 개수, 2개
const int num1 = 9;			// 3 by 3 puzzle1
const int num2 = 16;		// 4 by 4 puzzle2

const int MAX_COUNT = 50;


/*============================================================================================================*/

// 퍼즐의 알맞은 위치를 찾아 놓기
void locate_puzzle(ObjectPtr object, ScenePtr scene, int i, int n, int x, int xd, int y, int yd) {
	object->locate(scene, x + ((i - 1) % n) * xd, y - ((i - 1) / n) * yd);
}

// 퍼즐 클릭 or mix_puzzle() --> blank와 퍼즐의 위치를 서로 바꿔줌
bool isMoved(Puzzle& p, Puzzle& b, Info info) {
	int num = info.num, n = info.sqrtN;
	if (((p.location % n) != 1 && (p.location - 1) == b.location) ||			// left
		((p.location % n) != 0 && (p.location + 1) == b.location) ||			// right
		(p.location > n && (p.location - n) == b.location) ||					// up
		(p.location <= (info.num - n) && (p.location + n) == b.location))		// down
	{
		int temp = p.location;
		p.location = b.location;
		b.location = temp;

		locate_puzzle(p.object, info.scene, p.location, n, info.x, info.xd, info.y, info.yd);
		locate_puzzle(b.object, info.scene, b.location, n, info.x, info.xd, info.y, info.yd);

		//cout << "move " << p.location << " <-> " << b.location << "\tnow blaink is at " << b.location << endl;

		return true;
	}
	return false;
}

// start 버튼 클릭 --> 퍼즐을 랜덤으로 섞기
void mix_puzzle(Puzzle board[], Puzzle& b, Info& info, int& count) {
	b.object->hide();

	auto timer = Timer::create(0.1f);

	timer->setOnTimerCallback([=, &b, &count](TimerPtr t)->bool {
		//cout << "timeout " << count << endl;
		int newBlank = 0;
		int n = info.sqrtN;

		bool isCounted = false;
		do {
			do {

				switch (rand() % 4) {
				case 0:		//left
					newBlank = b.location - 1;		break;
				case 1:		//right
					newBlank = b.location + 1;		break;
				case 2:		//up
					newBlank = b.location - n;		break;
				case 3:		//down
					newBlank = b.location + n;		break;
				default:
					break;
				}
			} while (newBlank < 1 || newBlank >(n * n));

			int i = 1;
			for (; i <= (n * n); i++)
				if (board[i].location == newBlank) break;

			if (isMoved(board[i], b, info))
				isCounted = true;

		} while (!isCounted);


		count++;
		if (count < MAX_COUNT) {
			t->set(0.1f);
			t->start();
		}

		return true;
		});

	timer->start();

}

// 파일에서 최고 기록을 읽기
bool read_file(float& bestT, int N) {
	ifstream recordFile;
	recordFile.open("record.txt");

	bool isOn = false;
	if (recordFile.is_open()) {
		while (!recordFile.eof()) {
			string str;
			getline(recordFile, str);

			//cout << "str : " << str << endl;

			int p;
			if (N == 0) {
				if (str.find("puzzle1") == 0) {
					p = str.find(":");
					isOn = true;
				}
			}
			else if (N == 1) {
				if (str.find("puzzle2") == 0) {
					p = str.find(":");
					isOn = true;
				}
			}

			if (isOn) {
				string buf(str.substr(p + 1));
				if (buf != " ") {
					bestT = stof(buf);
					recordFile.close();
					return true;
				}
				else {
					bestT = 0;
					recordFile.close();
					return false;
				}
			}
		}
	}
}

// 파일에 최고기록을 업데이트 하기
void update_file(float& best, int N) {
	float best1, best2;
	read_file(best1, 0);
	read_file(best2, 1);

	best = round(best * 100) / 100.0;

	ofstream recordFile("record.txt");

	if (N == 0) {
		string str1 = "puzzle1 : " + to_string(best) + "\n";
		string str2 = "puzzle2 : " + to_string(best2) + "\n";
		if (recordFile.is_open()) {
			recordFile << str1;
			recordFile << str2;
		}
	}
	else if (N == 1) {
		string str1 = "puzzle1 : " + to_string(best1) + "\n";
		string str2 = "puzzle2 : " + to_string(best) + "\n";
		if (recordFile.is_open()) {
			recordFile << str1;
			recordFile << str2;
		}
	}
}

// 퍼즐 완성 
void isSolved(Puzzle board[], int n, clock_t startT, float& best, int N) {
	for (int i = 1; i < n; i++) {
		if (board[i].location != i) return;
	}
	clock_t endT = clock();
	float exeT = static_cast<double>(endT - startT) / CLOCKS_PER_SEC;

	read_file(best, N);

	char sentence[100] = "Game Complete \ntime : ";
	char time[10]; sprintf(time, "%.2f", exeT);
	strcat(sentence, time); strcat(sentence, " sec");
	if (exeT < best) {
		best = exeT;
		strcat(sentence, "\n!!최고 기록 경신!!");
	}
	else if (best == 0)
		best = exeT;

	update_file(best, N);

	showMessage(sentence);

	board[n].object->show();

	return;
}

// home에서 퍼즐 선택 or restart 클릭 --> 모든 퍼즐의 위치를 원래대로 한 후 숨김
void resetting(Puzzle board[], Puzzle& b, Info info) {
	for (int i = 1; i < info.num; i++) {
		board[i].location = i;
		locate_puzzle(board[i].object, info.scene, i, info.sqrtN, info.x, info.xd, info.y, info.yd);
		board[i].object->hide();
	}
	b.location = info.num;
	locate_puzzle(b.object, info.scene, info.num, info.sqrtN, info.x, info.xd, info.y, info.yd);
	b.object->hide();

	return;
}


/*=================================================== function ===================================================*/

int main() {

	srand((unsigned)time(NULL));

	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);

	clock_t startT;
	int mixCount = 0;


	// home
	auto home = Scene::create("home", "images/home.png");
	auto forClick = Object::create("images/home.png", home);
	auto endbutton = Object::create("images/endbutton.jpg", home, 1100, 0);
	auto record = Object::create("images/record.jpg", home, 1130, 60);
	record->setScale(1.05f);


	// puzzle1
	auto scene1 = Scene::create("Puzzle1", "images/background1.jpg");
	auto start1 = Object::create("images/start.png", scene1, 550, 350);
	auto homebutton1 = Object::create("images/backbutton.jpg", scene1, 0, 670);
	homebutton1->setScale(0.7f);
	auto restart1 = Object::create("images/restartbutton.jpg", scene1, 0, 620);
	restart1->setScale(0.7f);

	int x1 = 700, xd1 = 135, y1 = 445, yd1 = 167;
	float bestT1;
	read_file(bestT1, 1);

	Info info1;
	info1.scene = scene1;
	info1.x = x1; info1.xd = xd1; info1.y = y1; info1.yd = yd1;
	info1.num = num1;
	info1.sqrtN = sqrt(num1);
	info1.bestT = &bestT1;

	Puzzle blank1;
	blank1.object = Object::create("images/puzzle1/9.png", scene1, 0, 0, false);
	locate_puzzle(blank1.object, scene1, num1, info1.sqrtN, x1, xd1, y1, yd1);
	blank1.object->setScale(0.35f);
	blank1.location = num1;

	Puzzle board1[num1 + 1];
	board1[num1] = blank1;


	// puzzle2
	auto scene2 = Scene::create("Puzzle2", "images/background2.jpg");
	auto start2 = Object::create("images/start.png", scene2, 550, 350);
	auto homebutton2 = Object::create("images/backbutton.jpg", scene2, 0, 670);
	homebutton2->setScale(0.7f);
	auto restart2 = Object::create("images/restartbutton.jpg", scene2, 0, 620);
	restart2->setScale(0.7f);

	int x2 = 713, xd2 = 106, y2 = 519, yd2 = 158;
	float bestT2;
	read_file(bestT2, 2);

	Info info2;
	info2.scene = scene2;
	info2.x = x2; info2.xd = xd2; info2.y = y2; info2.yd = yd2;
	info2.num = num2;
	info2.sqrtN = sqrt(num2);
	info2.bestT = &bestT2;

	Puzzle blank2;
	blank2.object = Object::create("images/puzzle2/16.png", scene2);
	locate_puzzle(blank2.object, scene2, num2, info2.sqrtN, x2, xd2, y2, yd2);
	blank2.object->setScale(0.35f);
	blank2.location = num2;

	Puzzle board2[num2 + 1];
	board2[num2] = blank2;


	// game
	Game game[N_of_Puzzle];
	game[0].board = board1;	game[0].info = info1; game[0].blank = blank1;
	game[0].start = start1;	game[0].homebutton = homebutton1;	game[0].restart = restart1;
	game[1].board = board2;	game[1].info = info2; game[1].blank = blank2;
	game[1].start = start2;	game[1].homebutton = homebutton2;	game[1].restart = restart2;


	/*--------------------------------------------- setting -------------------------------------------------*/

	int N = 0;
	int* pN = &N;

	for (; N < N_of_Puzzle; N++) {
		// start
		game[N].start->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			for (int j = 1; j < game[N].info.num; j++)
				game[N].board[j].object->show();
			mixCount = 0;
			mix_puzzle(game[N].board, game[N].blank, game[N].info, mixCount);
			game[N].start->hide();
			startT = clock();
			cout << "timer start" << endl;
			return true;
			});

		// homebutton
		game[N].homebutton->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
			home->enter();
			return true;
			});

		// restart
		game[N].restart->setOnMouseCallback([&](ObjectPtr, int, int, MouseAction)->bool {
			resetting(game[N].board, game[N].blank, game[N].info);
			game[N].start->show();
			return true;
			});

		// puzzle
		for (int i = 1; i < game[N].info.num; i++) {
			auto filename = "images/puzzle" + to_string(N + 1) + "/" + to_string(i) + ".png";
			game[N].board[i].object = Object::create(filename, game[N].info.scene, 0, 0, false);
			locate_puzzle(game[N].board[i].object, game[N].info.scene, i,
				game[N].info.sqrtN, game[N].info.x, game[N].info.xd, game[N].info.y, game[N].info.yd);
			game[N].board[i].object->setScale(0.35f);
			game[N].board[i].location = i;


			game[N].board[i].object->setOnMouseCallback([&](ObjectPtr object, int, int, MouseAction)->bool {
				int n = 1;
				for (; n <= game[N].info.num; n++)
					if (game[N].board[n].object == object) break;


				if (isMoved(game[N].board[n], game[N].blank, game[N].info)) {
					isSolved(game[N].board, game[N].info.num, startT, *(game[N].info.bestT), N);
				}

				return true;
				});

		}
	}


	/*--------------------------------------------- operation -------------------------------------------------*/


	//home(start page)
	forClick->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {

		if ((x > 310) && (x < 560) && (y > 80) && (y < 390))		*pN = 0;
		else if ((x > 673) && (x < 897) && (y > 50) && (y < 390))	*pN = 1;
		else return false;

		game[*pN].info.scene->enter();
		resetting(game[*pN].board, game[*pN].blank, game[*pN].info);
		game[*pN].start->show();

		return true;
		});

	record->setOnMouseCallback([&](ObjectPtr, int, int, MouseAction) -> bool {

		read_file(*(game[0].info.bestT), 0);
		read_file(*(game[1].info.bestT), 1);

		char sentence[100] = "puzzle1) ";

		if (*(game[0].info.bestT) == 0) strcat(sentence, "no data");
		else {
			char buf[10] = { 0, }; sprintf(buf, "%.2fs", *(game[0].info.bestT));
			strcat(sentence, buf);
		}

		strcat(sentence, "\npuzzle2) ");
		if (*(game[1].info.bestT) == 0) strcat(sentence, "no data");
		else {
			char buf[10] = { 0, };  sprintf(buf, "%.2fs", *(game[1].info.bestT));
			strcat(sentence, buf);
		}

		showMessage(sentence);
		cout << sentence << endl;


		return true;
		});

	endbutton->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		endGame();
		return true;
		});

	/* ------------------------------------------------ operation ------------------------------------------------*/


	startGame(home);


	return 0;

}