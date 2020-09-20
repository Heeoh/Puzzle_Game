#define _CRT_SECURE_NO_WARNINGS
#include <bangtal.h>
using namespace bangtal;
#include <iostream>
using namespace std;
#include <ctime>
#include <cmath>

class Puzzle {
public:
	ObjectPtr object;
	int x, y, n;	
};

typedef struct Info {
	int num, x, y, xd, yd;
	int* puzzleNum, * direction;
	const int *answer;
};

const int num1 = 9;		// 3 by 3 puzzle1
const int num2 = 16;	// 4 by 4 puzzle2

void check_blank(int b, Info info) {
	for (int i = 0; i <= info.num; i++)
		info.direction[i] = 0;
	int sqrtN = sqrt(info.num);
	if (b % sqrtN != 0) info.direction[b + 1] = 1;				//left
	if (b % sqrtN != 1) info.direction[b - 1] = 2;				//right
	if (b + sqrtN <= info.num) info.direction[b + sqrtN] = 4;	//up
	if (b - sqrtN >= 1) info.direction[b - sqrtN] = 8;			//down
}

void mix_puzzle(Puzzle puzzle[], ScenePtr scene, Info info, int n) {
	int maxCount = 50;
	int num = info.num;

	int* data = new int[num];             
	for(int i=0; i<num; i++)	
		data[i] = i;

	srand((unsigned int)time(NULL));
	for (int i=0; i<maxCount; i++){
		int a = rand() % (num-1);
		int b = rand() % (num-1);
		if (a != b) {
			int temp = data[a];
			data[a] = data[b];
			data[b] = temp;
		}
	}

	int count = 0, x = info.x, y = info.y;
	for (int i = 0; i < sqrt(num); i++) {
		for (int j = 0; j<sqrt(num); j++) {
			int n = static_cast<int>(data[count]);
			if ( n == 0 ) {
				info.puzzleNum[0] = count +1;
				info.puzzleNum[count +1] = 0;
				check_blank(count+1, info);
			}
			else {
				info.puzzleNum[count+1] = n;
				puzzle[n].x = x;
				puzzle[n].y = y;
				puzzle[n].object->locate(scene, puzzle[n].x, puzzle[n].y);
				puzzle[n].object->show();
			}
			x += info.xd;
			count++;
			if (count >= num) break;
		}
		x = info.x;
		y -= info.yd;
	}
}

bool is_solved(clock_t startT, clock_t endT, float& exeT, Info info, float& best) {
	for (int i = 0; i <= info.num; i++) {
		if (info.puzzleNum[i] != info.answer[i]) return false;
		else if (i == info.num && info.puzzleNum[i] == info.answer[i]) {
			endT = clock();
			exeT = static_cast<double>(endT - startT)/CLOCKS_PER_SEC;

			char sentence[100] = "complete \ntime : ";
			char time[10]; sprintf(time, "%.2f", exeT);
			strcat(sentence, time); strcat(sentence, " sec");
			if (exeT < best) {
				best = exeT;
				strcat(sentence, "\n!!최고 기록 경신!!");
			}
			else if (best == 0) 
				best = exeT;

			showMessage(sentence);
			return true;
		}
		else continue;
	}
}

bool move_puzzle(Puzzle& puzzle, ScenePtr scene, Info info) {
	bool isMoved = false;

	int i = 1;
	for (; i <= info.num; i++)
		if (info.puzzleNum[i] == puzzle.n) break;

	if (info.direction[i] == 1) {
		puzzle.x -= info.xd;
		puzzle.object->locate(scene, puzzle.x, puzzle.y);
		isMoved = true;
	}
	else if (info.direction[i] == 2) {
		puzzle.x += info.xd;
		puzzle.object->locate(scene, puzzle.x, puzzle.y);
		isMoved = true;
	}
	else if (info.direction[i] == 4) {
		puzzle.y += info.yd;
		puzzle.object->locate(scene, puzzle.x, puzzle.y);
		isMoved = true;
	}
	else if (info.direction[i] == 8) {
		puzzle.y -= info.yd;
		puzzle.object->locate(scene, puzzle.x, puzzle.y);
		isMoved = true;
	}

	if (isMoved) {
		info.puzzleNum[info.puzzleNum[0]] = info.puzzleNum[i], info.puzzleNum[i] = 0, info.puzzleNum[0] = i;  // puzzleNum[] 수정
		check_blank(i, info);
		return true;
	}

	else return false;

}

int main() {

	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);

	clock_t startT, endT;
	float exeT;

	auto home = Scene::create("home", "images/home.jpg");


/*
**
puzzle1 ( 3 by 3)
**
*/
	auto scene1 = Scene::create("Puzzle1", "images/background1.jpg");
	
	auto p1 = Object::create("images/p1.png");
	auto p2 = Object::create("images/p2.png");
	auto p3 = Object::create("images/p3.png");
	auto p4 = Object::create("images/p4.png");
	auto p5 = Object::create("images/p5.png");
	auto p6 = Object::create("images/p6.png");
	auto p7 = Object::create("images/p7.png");
	auto p8 = Object::create("images/p8.png");
	auto p9 = Object::create("images/p9.png");

	auto start1 = Object::create("images/start.png", scene1, 550, 350);
	auto homebutton1 = Object::create("images/backbutton.jpg", scene1, 0, 670);
	homebutton1->setScale(0.7f);
	auto restart1 = Object::create("images/restartbutton.jpg", scene1, 0, 620);
	restart1->setScale(0.7f);


	//--------------------------------------------------------------------------------------------

	Puzzle puzzle1[num1];
	puzzle1[1].object = p1;
	puzzle1[2].object = p2;
	puzzle1[3].object = p3;
	puzzle1[4].object = p4;
	puzzle1[5].object = p5;
	puzzle1[6].object = p6;
	puzzle1[7].object = p7;
	puzzle1[8].object = p8;
	for (int i = 1; i < num1; i++) {
		puzzle1[i].object->setScale(0.35f);
		puzzle1[i].n = i;
	}

	int puzzleNum1[num1 + 1] = { 0, };		// [0] - blank 위치, 해당 위치에 있는 퍼즐 number
	int direction1[num1 + 1] = { 0, };		// blank위치로 옮길 수 있는 방향, 0 == 옮길 수 X, 1-Left 2-Right 4-Up 8-Down
	const int answer1[num1 + 1] = { 9, 1, 2, 3, 4, 5, 6, 7, 8, 0 };

	Info info1;
	info1.num = num1;
	info1.puzzleNum = puzzleNum1;	info1.direction = direction1;	info1.answer = answer1;
	info1.x = 700; info1.xd = 135; info1.y = 445; info1.yd = 167;
	
	float best1 = 0;

	//----------------------------------------------------------------------------------------------------

	homebutton1->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		home->enter();
		return true;
		});

	restart1->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		for (int i = 1; i < num1; i++)
			puzzle1[i].object->hide();
		p9->hide();
		start1->show();
		return true;
		});

	start1->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		mix_puzzle(puzzle1, scene1, info1, num1);
		start1->hide();
		startT = clock();
		cout << "timer start" << endl;
		return true;
	});


	p1->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle1[1], scene1, info1)) {
			if (is_solved(startT, endT, exeT, info1, best1)) {
				p9->locate(scene1, 970, 111);
				p9->setScale(0.35f);
				p9->show();
			}
		}
		return true;
		});
	p2->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if(move_puzzle(puzzle1[2], scene1, info1)) {
			if (is_solved(startT, endT, exeT, info1, best1)) {
				p9->locate(scene1, 970, 111);
				p9->setScale(0.35f);
				p9->show();
			}
		}
		return true;
		});
	p3->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if(move_puzzle(puzzle1[3], scene1, info1)) {
			if (is_solved(startT, endT, exeT, info1, best1)) {
				p9->locate(scene1, 970, 111);
				p9->setScale(0.35f);
				p9->show();
			}
		}
		return true;
		});
	p4->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if(move_puzzle(puzzle1[4], scene1, info1)) {
			if (is_solved(startT, endT, exeT, info1, best1)) {
				p9->locate(scene1, 970, 111);
				p9->setScale(0.35f);
				p9->show();
			}
		}
		return true;
		});
	p5->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if(move_puzzle(puzzle1[5], scene1, info1)) {
			if (is_solved(startT, endT, exeT, info1, best1)) {
				p9->locate(scene1, 970, 111);
				p9->setScale(0.35f);
				p9->show();
			}
		}
		return true;
		});
	p6->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if(move_puzzle(puzzle1[6], scene1, info1)) {
			if (is_solved(startT, endT, exeT, info1, best1)) {
				p9->locate(scene1, 970, 111);
				p9->setScale(0.35f);
				p9->show();
			}
		}
		return true;
		});
	p7->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if(move_puzzle(puzzle1[7], scene1, info1)) {
			if (is_solved(startT, endT, exeT, info1, best1)) {
				p9->locate(scene1, 970, 111);
				p9->setScale(0.35f);
				p9->show();
			}
		}
		return true;
		});
	p8->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle1[8], scene1, info1)) {
			if (is_solved(startT, endT, exeT, info1, best1)) {
				p9->locate(scene1, 970, 111);
				p9->setScale(0.35f);
				p9->show();
			}
		}
		return true;
	});


/*
**
puzzle2 ( 5 by 5 )
**
*/
	auto scene2 = Scene::create("Puzzle2", "images/background2.jpg");

	auto q1 = Object::create("images/q1.png");
	auto q2 = Object::create("images/q2.png");
	auto q3 = Object::create("images/q3.png");
	auto q4 = Object::create("images/q4.png");
	auto q5 = Object::create("images/q5.png");
	auto q6 = Object::create("images/q6.png");
	auto q7 = Object::create("images/q7.png");
	auto q8 = Object::create("images/q8.png");
	auto q9 = Object::create("images/q9.png");
	auto q10 = Object::create("images/q10.png");
	auto q11 = Object::create("images/q11.png");
	auto q12 = Object::create("images/q12.png");
	auto q13 = Object::create("images/q13.png");
	auto q14 = Object::create("images/q14.png");
	auto q15 = Object::create("images/q15.png");
	auto q16 = Object::create("images/q16.png");

	auto start2 = Object::create("images/start.png", scene2, 550, 350);
	auto homebutton2 = Object::create("images/backbutton.jpg", scene2, 0, 670);
	homebutton2->setScale(0.7f);
	auto restart2 = Object::create("images/restartbutton.jpg", scene2, 0, 620);
	restart2->setScale(0.7f);

	//------------------------------------------------------------------------------------

	Puzzle puzzle2[num2];
	puzzle2[1].object = q1;
	puzzle2[2].object = q2;
	puzzle2[3].object = q3;
	puzzle2[4].object = q4;
	puzzle2[5].object = q5;
	puzzle2[6].object = q6;
	puzzle2[7].object = q7;
	puzzle2[8].object = q8;
	puzzle2[9].object = q9;
	puzzle2[10].object = q10;
	puzzle2[11].object = q11;
	puzzle2[12].object = q12;
	puzzle2[13].object = q13;
	puzzle2[14].object = q14;
	puzzle2[15].object = q15;
	for (int i = 1; i < num2; i++) {
		puzzle2[i].object->setScale(0.35f);
		puzzle2[i].n = i;
	}

	int puzzleNum2[num2 + 1] = { 0, };		// [0] - blank 위치, 해당 위치에 있는 퍼즐 number
	int direction2[num2 + 1] = { 0, };		// blank위치로 옮길 수 있는 방향, 0 == 옮길 수 X, 1-Left 2-Right 4-Up 8-Down
	const int answer2[num2 + 1] = { 16, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0 };

	Info info2;
	info2.num = num2;
	info2.puzzleNum = puzzleNum2;	info2.direction = direction2;	info2.answer = answer2;
	info2.x = 713; info2.xd = 106; info2.y = 519; info2.yd = 158;

	float best2 = 0;

	//--------------------------------------------------------------------------------------------------------

	homebutton2->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		home->enter();
		return true;
		});

	restart2->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		for (int i = 1; i < num2; i++)
			puzzle2[i].object->hide();
		start2->show();
		q16->hide();
		return true;
		});

	
	start2->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		mix_puzzle(puzzle2, scene2, info2, num2);
		start2->hide();
		startT = clock();
		return true;
		});


	q1->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle2[1], scene2, info2)) {
			if (is_solved(startT, endT, exeT, info2, best2)) {
				q16->locate(scene2, 1031, 45);
				q16->setScale(0.35f);
				q16->show();
			}
		}
		return true;
		});
	q2->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle2[2], scene2, info2)) {
			if (is_solved(startT, endT, exeT, info2, best2)) {
				q16->locate(scene2, 1031, 45);
				q16->setScale(0.35f);
				q16->show();
			}
		}
		return true;
		});
	q3->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle2[3], scene2, info2)) {
			if (is_solved(startT, endT, exeT, info2, best2)) {
				q16->locate(scene2, 1031, 45);
				q16->setScale(0.35f);
				q16->show();
			}
		}
		return true;
		});
	q4->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle2[4], scene2, info2)) {
			if (is_solved(startT, endT, exeT, info2, best2)) {
				q16->locate(scene2, 1031, 45);
				q16->setScale(0.35f);
				q16->show();
			}
		}
		return true;
		});
	q5->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle2[5], scene2, info2)) {
			if (is_solved(startT, endT, exeT, info2, best2)) {
				q16->locate(scene2, 1031, 45);
				q16->setScale(0.35f);
				q16->show();
			}
		}
		return true;
		});
	q6->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle2[6], scene2, info2)) {
			if (is_solved(startT, endT, exeT, info2, best2)) {
				q16->locate(scene2, 1031, 45);
				q16->setScale(0.35f);
				q16->show();
			}
		}
		return true;
		});
	q7->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle2[7], scene2, info2)) {
			if (is_solved(startT, endT, exeT, info2, best2)) {
				q16->locate(scene2, 1031, 45);
				q16->setScale(0.35f);
				q16->show();
			}
		}
		return true;
		});
	q8->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle2[8], scene2, info2)) {
			if (is_solved(startT, endT, exeT, info2, best2)) {
				q16->locate(scene2, 1031, 45);
				q16->setScale(0.35f);
				q16->show();
			}
		}
		return true;
		});
	q9->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle2[9], scene2, info2)) {
			if (is_solved(startT, endT, exeT, info2, best2)) {
				q16->locate(scene2, 1031, 45);
				q16->setScale(0.35f);
				q16->show();
			}
		}
		return true;
		});
	q10->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle2[10], scene2, info2)) {
			if (is_solved(startT, endT, exeT, info2, best2)) {
				q16->locate(scene2, 1031, 45);
				q16->setScale(0.35f);
				q16->show();
			}
		}
		return true;
		});
	q11->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle2[11], scene2, info2)) {
			if (is_solved(startT, endT, exeT, info2, best2)) {
				q16->locate(scene2, 1031, 45);
				q16->setScale(0.35f);
				q16->show();
			}
		}
		return true;
		});
	q12->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle2[12], scene2, info2)) {
			if (is_solved(startT, endT, exeT, info2, best2)) {
				q16->locate(scene2, 1031, 45);
				q16->setScale(0.35f);
				q16->show();
			}
		}
		return true;
		});
	q13->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle2[13], scene2, info2)) {
			if (is_solved(startT, endT, exeT, info2, best2)) {
				q16->locate(scene2, 1031, 45);
				q16->setScale(0.35f);
				q16->show();
			}
		}
		return true;
		});
	q14->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle2[14], scene2, info2)) {
			if (is_solved(startT, endT, exeT, info2, best2)) {
				q16->locate(scene2, 1031, 45);
				q16->setScale(0.35f);
				q16->show();
			}
		}
		return true;
		});
	q15->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if (move_puzzle(puzzle2[15], scene2, info2)) {
			if (is_solved(startT, endT, exeT, info2, best2)) {
				q16->locate(scene2, 1031, 45);
				q16->setScale(0.35f);
				q16->show();
			}
		}
		return true;
		});


/*
** 
home(start page)
**
*/
	auto forClick = Object::create("images/home.jpg", home);

	forClick->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		if ((x > 348) && (x < 572) && (y > 50) && (y < 388)) {
			scene2->enter();
			for (int i = 1; i < num2; i++)
				puzzle2[i].object->hide();
			p9->hide();
			start2->show();
		}

		if ((x > 680) && (x < 928) && (y > 80) && (y < 388)) {
			scene1->enter();
			for (int i = 1; i < num1; i++)
				puzzle1[i].object->hide();
			q16->hide();
			start1->show();
		}

		return true;
		});



	startGame(home);


	return 0;

}