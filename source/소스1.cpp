/*
	지뢰찾기
*/

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstring>

#define MINES_NUM 10
#define BOARD_WIDTH 9

// 보드판
int board[BOARD_WIDTH+2][BOARD_WIDTH+2];
int boardValue[BOARD_WIDTH+2][BOARD_WIDTH+2];

// 설정-----------------------------------------------------
// window setting
int appWindow_width = 11, appWindow_height = 13, appWindow_scale = 50;
int tileSize = 50;

// gameTime, minesFlag(깃발 세웠느냐안세웠느냐), loseFlag, winFlag
int minesFlag = 0; //count
int loseFlag = 0;
int winFlag = 0;
int gameTime = 0;

int minesLeft = MINES_NUM; // 10
int boardLeft = BOARD_WIDTH * BOARD_WIDTH; //81


// 1. 지뢰 설치----------------------------
void placeMine();
void MineNeighborhood(int _x, int _y);

// 2.지뢰 찾기------------------------------
void onClick(int _x, int _y, int _left_right);
void openBlock(int _x, int _y);

//-----------------------------------------
void ShowBoard(); // 보드 보여주기
void reset(); // 새로고침

int main()
{
	sf::RenderWindow appWindow(sf::VideoMode(appWindow_width*appWindow_scale, appWindow_height*appWindow_scale), "ddaisyujin", sf::Style::Close);

	//파일 불러오기--------------------------------
	//file
	sf::Texture _background; // 사용하고 싶은 이미지
	if (!_background.loadFromFile("background.png"))
		return -1;
	sf::Sprite background;
	background.setTexture(_background);

	//폰트
	sf::Font font;
	font.loadFromFile("fonts/visitor1.ttf");

	//win, lose, tile
	sf::Texture image_sprite, win_sprite, lose_sprite;
	image_sprite.loadFromFile("tiles.png");
	win_sprite.loadFromFile("win.png");
	lose_sprite.loadFromFile("lose.png");

	sf::Sprite s_tile(image_sprite);
	sf::Sprite s_win(win_sprite);
	sf::Sprite s_lose(lose_sprite);

	//timer, 남은지뢰 텍스트 세팅
	sf::Text timerText; // timer
	timerText.setFont(font);
	timerText.setCharacterSize(50);
	timerText.setStyle(sf::Text::Bold);
	timerText.setFillColor(sf::Color::Black);

	sf::Text leftminesText; // 남은 지뢰 개수
	leftminesText.setFont(font);
	leftminesText.setCharacterSize(50);
	leftminesText.setStyle(sf::Text::Bold);
	leftminesText.setFillColor(sf::Color::Red);


	std::fill(&board[0][0], &board[BOARD_WIDTH][BOARD_WIDTH + 1], 0);
	std::fill(&boardValue[0][0], &boardValue[BOARD_WIDTH+1][BOARD_WIDTH + 2], 16); //회색타일로 뒤덮음
	//--------------------------------------------

	placeMine();
	ShowBoard(); 

	sf::Clock clock;
	sf::Time timer;

	//--------------------앱가동
	while (appWindow.isOpen())
	{
		//배경
		appWindow.draw(background);

		//score칸
		leftminesText.setString(std::to_string(minesLeft));
		leftminesText.setPosition(3 * appWindow_scale - 5, 1 * appWindow_scale - 7);
		appWindow.draw(leftminesText);

		//Timer칸
		timer = clock.getElapsedTime();
		if (winFlag == 0 && loseFlag == 0)
			gameTime = (int)timer.asSeconds(); 
		timerText.setString(std::to_string(gameTime));
		timerText.setPosition(6 * appWindow_scale + 10, 1 * appWindow_scale - 7);
		appWindow.draw(timerText);

		// 타일 깔기
		for (int i = 1; i <= BOARD_WIDTH; i++)
		{
			for (int j = 1; j <= BOARD_WIDTH; j++)
			{
				s_tile.setTextureRect(sf::IntRect(boardValue[i][j] * tileSize, 0, tileSize, tileSize));
				s_tile.setPosition(j*appWindow_scale, (i + 2)*appWindow_scale);
				appWindow.draw(s_tile);
			}
		}


		if (winFlag == 1)
		{
			clock.restart();

			for (int i = 1; i <= BOARD_WIDTH; i++)
			{
				for (int j = 1; j <= BOARD_WIDTH; j++)
				{
					if (board[i][j] == 9)
					{
						boardValue[i][j] = 9;
					}
				}
			}

			s_win.setTextureRect(sf::IntRect(0, 0, 350, 350));
			s_win.setPosition(2 * appWindow_scale, 4 * appWindow_scale);
			appWindow.draw(s_win);
		}
		else if (loseFlag == 1)
		{
			clock.restart();

			for (int i = 1; i <= BOARD_WIDTH; i++)
			{
				for (int j = 1; j <= BOARD_WIDTH; j++)
				{
					if (board[i][j] == 9)
					{
						boardValue[i][j] = 9;
					}
				}
			}
			s_lose.setTextureRect(sf::IntRect(0, 0, 350, 350));
			s_lose.setPosition(2 * appWindow_scale, 4 * appWindow_scale);
			appWindow.draw(s_lose);
		}

		sf::Vector2i pos = sf::Mouse::getPosition(appWindow);
		int mouseX = pos.x / appWindow_scale;
		int mouseY = pos.y / appWindow_scale-2;


		sf::Event event;
		while (appWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				appWindow.close();

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				//리셋 버튼 눌렀을 때
				if (mouseX == 9 & mouseY == -1)
				{
					clock.restart();
					reset();
				}
				//보드 눌렀을 때
				else
				{
					std::cout << mouseY << "," << mouseX << '\n';

					onClick(mouseY, mouseX, 0);
					std::cout << '\n';
					ShowBoard();
				}
			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				std::cout << mouseY << "," << mouseX << '\n';
				
				onClick(mouseY, mouseX, 1);
				std::cout << '\n';
				ShowBoard();
			}
		}

		appWindow.display();
	}
}

//--------------------------------------
void placeMine()
{
	srand((unsigned int)time(0));

	//지뢰 세팅
	int mines_num = 0; // 지뢰 세팅 개수
	while (mines_num < MINES_NUM)
	{
		int randPosition1 = (rand() % BOARD_WIDTH) + 1; //1~9
		int randPosition2 = (rand() % BOARD_WIDTH) + 1; //1~9

		if ((board[randPosition1][randPosition2] != 9)) {
			board[randPosition1][randPosition2] = 9;
			mines_num++;
		}
	}

	//이웃들 카운트
	MineNeighborhood(1, 1); //1,1 ~ 81,81

}

void MineNeighborhood(int _x, int _y)
{
	if (_x == BOARD_WIDTH && _y == BOARD_WIDTH)
		return;

	if (board[_x][_y] == 9)
	{
		for (int i = _x - 1; i <= _x + 1; i++)
		{
			for (int j = _y - 1; j <= _y + 1; j++)
			{
				if (board[i][j] == 9)
					continue;

				board[i][j]++;
			}
		}
	}

	if (_y == BOARD_WIDTH)
		MineNeighborhood(_x + 1, 1);
	else
		MineNeighborhood(_x, (_y + 1));
}

//--------------------------------------

void onClick(int _x, int _y, int _left_right)
{

	// 왼쪽 클릭시
	if (_left_right == 0)
	{
		//지뢰인가
		if (board[_x][_y] == 9)
		{
			//지뢰인데 플래그 세워진
			if (boardValue[_x][_y] == 12)
			{
				board[_x][_y] = -2; // -2: 찾음 상태
				boardValue[_x][_y] = 11;
				minesLeft--;
			}
			//무방비 상태 ( 플래그가 안꽂힘)
			else
			{
				loseFlag = 1;
				return;
			}
		}
		//지뢰가 아닌가
		else
		{
			openBlock(_x, _y);// 타일 오픈
		}
	}
	// 오른쪽 클릭시
	else if (_left_right == 1) 
	{// 깃발 관리
		
		// 깃발이 안꽂혀져있
		if (boardValue[_x][_y] != 12 && board[_x][_y] != -2) // 깃발이 안꽂혀져있고, 눌려진 상태가 아니다.
		{
			boardValue[_x][_y] = 12;
			minesFlag++;
		}
		// 깃발이 꽂혀져있
		else if(boardValue[_x][_y]==12)
		{
			boardValue[_x][_y] = 16; // 원상태로 복구
			minesFlag--;
		}
	}

	// 이긴지 판단
	if (minesLeft == 0 || (minesFlag == MINES_NUM && boardLeft == MINES_NUM))
	{
		winFlag = 1;
		return;
	}
}

void openBlock(int _x, int _y)
{

	// 이미 클린된 것이거나, 플래그가 쳐져있을때
	if (board[_x][_y] == -2 || boardValue[_x][_y] == 12)
		return;
	// 모서리일때
	else if (_x == 0 || _x == BOARD_WIDTH + 1 || _y == 0 || _y == BOARD_WIDTH + 1)
		return;

	boardValue[_x][_y] = board[_x][_y];
	board[_x][_y] = -2; //바로 클릭 = 바로 뒤집
	boardLeft--;

	// 2. 3x3내에 지뢰가 없으면 다음 칸으로 넘어가기
	if (board[_x - 1][_y - 1] != 9
		&& board[_x - 1][_y] != 9
		&& board[_x - 1][_y + 1] != 9
		&& board[_x][_y - 1] != 9
		&& board[_x][_y + 1] != 9
		&& board[_x + 1][_y - 1] != 9
		&& board[_x + 1][_y] != 9
		&& board[_x + 1][_y + 1] != 9)
	{
		openBlock(_x - 1, _y - 1);
		openBlock(_x - 1, _y );
		openBlock(_x - 1, _y + 1);
		openBlock(_x , _y - 1);
		openBlock(_x, _y + 1);
		openBlock(_x + 1, _y - 1);
		openBlock(_x + 1, _y);
		openBlock(_x + 1, _y + 1);
	}
	else
	{
		return;
	}
}

void ShowBoard()
{
	for (int i = 1; i < BOARD_WIDTH + 1; i++)
	{
		for (int j = 1; j < BOARD_WIDTH + 1; j++)
		{
			if (board[i][j] == -2)
			{
				std::cout << '*';
				continue;
			}
			std::cout << board[i][j];
		}
		std::cout << '\n';
	}

	std::cout << '\n';
}


void reset()
{
	std::fill(&board[0][0], &board[BOARD_WIDTH][BOARD_WIDTH + 1], 0);
	std::fill(&boardValue[0][0], &boardValue[BOARD_WIDTH][BOARD_WIDTH + 1], 16); //회색타일로 뒤덮음

	placeMine(); //지뢰 재설치

	minesFlag = 0; //count
	loseFlag = 0;
	winFlag = 0;
	gameTime = 0;

	minesLeft = MINES_NUM; // 10
	boardLeft = BOARD_WIDTH * BOARD_WIDTH; //81
}