#pragma once
#include "cocos2d.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace std;

class HelloWorld: public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init() override;
	CREATE_FUNC(HelloWorld);

	void update(float) override;

	void playGame(Ref*);

	void removeMenu();
	void makeMenu();

	void createHUD();
	void createBackground();

	void loadSound();
	void preLoadScream();

	void createSnake();
	void createFood();

	void updateScore();
	void moveSnake(float);
	void removeFood();

	void gameOver();

	Sprite* makeSprite(string, float, float, int);
	Sprite* makeSprite(string, float, float, float, int);

private:
	//Snake stuff
	int startLength = 3;
	int cellWidth = 35;
	Vec2 startPosition = Vec2(300.0f, 200.0f);
	deque<Sprite*> snakeArray;
	Sprite* food;

	string sBodyName = "3.png";
	string sHeadName = "Head1.png";

	boolean addSection = false;
	boolean isMoveLocked = false;//To prevent the player from changing directions too fast

	//Menu
	Menu* menu;
	MenuItemFont* miPlay;
	MenuItem* miSettings;
	MenuItem* miHighscore;

	//Constants
	enum Direction { UP,DOWN,LEFT,RIGHT };
	enum DirectionRotate { R_UP = 0,R_DOWN = 180,R_LEFT = 270,R_RIGHT = 90 };
	enum Tag {SNAKE_HEAD, SNAKE_CELL, FOOD, BARRIER};
	enum FoodType { MALE,FEMALE,CHILD };
	
	Direction curDirection=RIGHT;
	FoodType curFoodType;

	int sbgPropColour = 40;

	//Audio and audio paths
	string screamPath;
	string seMusicPath = "Sounds/Background Music/Creepy Music Box.mp3";
	string seSplatPath = "Sounds/Sound Effects/Splat.mp3";
	CocosDenshion::SimpleAudioEngine* audio = CocosDenshion::SimpleAudioEngine::getInstance();

	//ZScores
	int zBackground = -1;
	int zSprite = 0;
	int zMenu = 1;
	int zGameOverCover = 2;
	int zGameOverMessage = 3;

	//HUD
	Label* scoreLabel;
	string scoreStirng = "Score: ";
	int score = 0;
	
	//Listeners
	bool onContactBegan(PhysicsContact &contact);
	void onKeyPressed(EventKeyboard::KeyCode, Event* );
};