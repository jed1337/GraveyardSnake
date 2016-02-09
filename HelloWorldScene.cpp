#include "HelloWorldScene.h"
#include <iostream>

using namespace CocosDenshion;
using namespace std;
using namespace cocos2d;

Scene* HelloWorld::createScene() {
	Scene* scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(Vec2::ZERO);
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

	Layer* layer = HelloWorld::create();
	scene->addChild(layer);
	return scene;
}

bool HelloWorld::init() {
	if (!Layer::init()) {
		return false;
	}
	srand((unsigned) time(NULL));//Seed Random numbers

										  //makeMenu();
	playGame(nullptr);
	return true;
}

void HelloWorld::removeMenu() {
	this->removeChild(menu);
}

void HelloWorld::makeMenu() {
	Size screenSize = this->getContentSize();
	miPlay = MenuItemFont::create("Play",CC_CALLBACK_1(HelloWorld::playGame,this));
	miPlay->setPosition(Vec2(screenSize.width/2,screenSize.height/2));
	menu = Menu::create(miPlay,nullptr);
	menu->setPosition(Vec2::ZERO);

	/*auto closeItem = MenuItemImage::create("CloseNormal.png","CloseSelected.png",
	CC_CALLBACK_1(HelloWorld::menuCloseCallback,this));
	auto menu = Menu::create(closeItem,NULL);*/

	this->addChild(menu,zMenu);
}

void HelloWorld::playGame(Ref* pSender) {
	removeMenu();
	createBackground();
	createHUD();
	loadSound();
	createSnake();
	createFood();
	this->scheduleUpdate();

	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyPressed,this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener,this);

	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegan,this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener,this);

}

void HelloWorld::loadSound() {
	audio->preloadBackgroundMusic(seMusicPath.c_str());
	audio->playBackgroundMusic(seMusicPath.c_str());

	audio->preloadBackgroundMusic(seSplatPath.c_str());
}

void HelloWorld::update(float delta) {
	moveSnake(delta);
}

void HelloWorld::createHUD() {
	Size screenSize = this->getContentSize();
	scoreLabel = Label::createWithTTF(scoreStirng+to_string(score),"fonts/Chiller.ttf",40);
	scoreLabel->setAnchorPoint(Vec2::ZERO);
	scoreLabel->setPosition(Vec2(screenSize.width-scoreLabel->getContentSize().width-20,10));
	this->addChild(scoreLabel,zMenu);

	Sprite* censor = Sprite::create("Extracted Sprites/Background Props/0.png");
	censor->setColor(Color3B(sbgPropColour,sbgPropColour,sbgPropColour));
	censor->setPosition(Vec2::ZERO);
	censor->setScale(1.5f);
	this->addChild(censor,zMenu);
}

void HelloWorld::createBackground() {
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Size winSize = Director::getInstance()->getVisibleSize();

	Sprite* background = Sprite::create("Extracted Sprites/Background/0.jpg");
	background->setAnchorPoint(Vec2::ZERO);
	background->setPosition(Vec2::ZERO);
	this->addChild(background,zBackground);

	float sbgScale = 1.5f;
	Size selfSize = this->getContentSize();
	for (int i = 0; i<(rand()%9)+8; i++) {//[8-16] items
		int nbgProp = rand()%18;
		Sprite* sbgProp = Sprite::create("Extracted Sprites/Background Props/"+to_string(nbgProp)+".png");

		Size sbgPropSize = sbgProp->getContentSize();
		int minX = sbgPropSize.width/2;
		int maxX = selfSize.width-sbgPropSize.width/2;
		int minY = sbgPropSize.height/2;
		int maxY = selfSize.height-sbgPropSize.height/2;
		int randX = (rand()%(maxX-minX))+minX;
		int randY = (rand()%(maxY-minY))+minY;

		sbgProp->setScale(sbgScale);
		sbgProp->setColor(Color3B(sbgPropColour,sbgPropColour,sbgPropColour));
		sbgProp->setPosition(Vec2(randX,randY));
		this->addChild(sbgProp,zBackground);
	}
}

void HelloWorld::updateScore() {
	Size screenSize = this->getContentSize();
	score += 10;
	scoreLabel->setString(scoreStirng+to_string(score));
	scoreLabel->setPosition(Vec2(screenSize.width-scoreLabel->getContentSize().width-20,10));
}

void HelloWorld::createSnake() {
	for (int i = 0; i<startLength; i++) {
		float x = (i*-cellWidth)+startPosition.x;
		float y = startPosition.y;

		Sprite* tempSnakeCell;
		string path = "";
		if (i==0)
			tempSnakeCell = HelloWorld::makeSprite(sHeadName,x,y,DirectionRotate::R_RIGHT,Tag::SNAKE_HEAD);
		else
			tempSnakeCell = HelloWorld::makeSprite(sBodyName,x,y,(rand()%4)*90,Tag::SNAKE_CELL);

		snakeArray.push_back(tempSnakeCell);
		this->addChild(tempSnakeCell,zSprite);
	}

	cout<<"Create Snake End";
}

void HelloWorld::removeFood() {
	if (food!=nullptr) {
		this->removeChild(food,true);
		audio->playEffect(screamPath.c_str());
		food = nullptr;
		//CCLOG(screamPath.c_str());
	}
}

void HelloWorld::preLoadScream() {
	screamPath = "Sounds/";
	switch (curFoodType) {
	case HelloWorld::MALE:
		screamPath += "Scream Male/";
		break;
	case HelloWorld::FEMALE:
		screamPath += "Scream Female/";
		break;
	case HelloWorld::CHILD:
		screamPath += "Scream Child/";
		break;
	default:
		cout<<"";
		break;
	}
	screamPath += to_string(rand()%3)+".mp3";
	//SimpleAudioEngine::sharedEngine()->preloadEffect(screamPath.c_str());
	//CCLOG(screamPath.c_str());
	audio->preloadEffect(screamPath.c_str());
}

void HelloWorld::createFood() {
	removeFood();

	curFoodType = FoodType(rand()%3);
	string foodPath = "Extracted Sprites/";//Male/";
	switch (curFoodType) {
	case HelloWorld::MALE:
		foodPath += "Male/";
		break;
	case HelloWorld::FEMALE:
		foodPath += "Female/";
		break;
	case HelloWorld::CHILD:
		foodPath += "Child/";
		break;
	default:
		cout<<"";
		break;
	}

	//srand((unsigned) time(NULL));
	int num = rand()%10;//[0-9]

	food = HelloWorld::makeSprite(foodPath+to_string(num)+".png",200,200,Tag::FOOD);
	preLoadScream();

	Size screenSize = this->getContentSize();
	Size fSize = food->getContentSize();

	int minX = fSize.width/2;
	int maxX = screenSize.width-minX;
	int minY = fSize.height/2;
	int maxY = screenSize.height-minY;

	boolean valid = false;
	int x;
	int y;

	int randX;
	int randY;
	while (!valid) {
		valid = true;
		randX = rand();
		randY = rand();

		x = (randX%(maxX-minX))+minX;
		y = (randY%(maxY-minY))+minY;

		for each (Sprite* s in snakeArray) {
			Size ss = s->getContentSize();
			Vec2 sp = s->getPosition();

			if ((x>sp.x && x<sp.x+ss.width)
				&&(y>sp.y && y<sp.y+ss.height)) {
				valid = false;
			}
		}
	}

	food->setPosition(Vec2(x,y));
	this->addChild(food,zSprite);
	auto move = MoveTo::create(0,Vec2(x,y));
	food->runAction(Sequence::create(move,nullptr));
	food->setScale(2.0);
	cout<<"";
}

void HelloWorld::moveSnake(float delta) {
	isMoveLocked = false;
	Sprite* snakeFirst = snakeArray.front();
	Vec2 newPosition = snakeFirst->getPosition();
	Vec2 oldPosition = Vec2(newPosition);

	Rect screen = this->getBoundingBox();
	float heightLimit = snakeFirst->getBoundingBox().size.height/2;
	float widthLimit = snakeFirst->getBoundingBox().size.width/2;

	float mov = cellWidth;
	snakeFirst->setRotation(0);//Resets the snake head rotation to its default

	switch (curDirection) {
	case UP:
		newPosition.y += mov;
		snakeFirst->setRotation(R_UP);//Rotates the snake head depending on the direction
		break;
	case DOWN:
		newPosition.y -= mov;
		snakeFirst->setRotation(R_DOWN);
		break;
	case LEFT:
		newPosition.x -= mov;
		snakeFirst->setRotation(R_LEFT);
		break;
	case RIGHT:
		newPosition.x += mov;
		snakeFirst->setRotation(R_RIGHT);
		break;
	}

	if (newPosition.y>screen.getMaxY()+heightLimit)//Goes past up screen
		newPosition.y = -heightLimit;
	else if (newPosition.y<-heightLimit)//Goes past down screen
		newPosition.y = screen.getMaxY()+heightLimit;
	else if (newPosition.x<-widthLimit)//Goes past left screen
		newPosition.x = screen.getMaxX()+widthLimit;
	else if (newPosition.x>screen.getMaxX()+widthLimit)//Goes past right screen
		newPosition.x = -widthLimit;

	Sprite* sNext;
	if (addSection) {//Adds another snake section
						  //sNext = HelloWorld::makeSprite(sBodyName,0,0,Tag::SNAKE_CELL);
		sNext = HelloWorld::makeSprite(sBodyName,0,0,(rand()%4)*90,Tag::SNAKE_CELL);
		this->addChild(sNext,zSprite);
		addSection = false;
	} else {//Normal move
		sNext = snakeArray.back();
		snakeArray.pop_back();//Removes snakeLast
	}
	snakeFirst->setPosition(newPosition);
	sNext->setPosition(oldPosition);

	//Readd them in the appropriate order
	snakeArray.pop_front();//Removes snakeFirst
	snakeArray.push_front(sNext);
	snakeArray.push_front(snakeFirst);//Makes snakeFirst be in snakeArray[0]

												 //vector<Vec2> sArrPositions;
												 //for each (Sprite* var in snakeArray) {
												 //	sArrPositions.push_back(var->getPosition());
												 //}
}

Sprite* HelloWorld::makeSprite(string path,float x,float y,int tag) {
	return makeSprite(path,x,y,0,tag);
}

Sprite* HelloWorld::makeSprite(string path,float x,float y,float rotation,int tag) {
	Sprite* sprite = Sprite::create(path);

	sprite->setPosition(Vec2(x,y));
	sprite->setRotation(rotation);
	//sprite->setAnchorPoint(Vec2::ZERO);

	PhysicsBody* physicsBody = PhysicsBody::createBox(sprite->getContentSize(),PhysicsMaterial());
	physicsBody->setCollisionBitmask(tag);
	physicsBody->setContactTestBitmask(true);

	sprite->setPhysicsBody(physicsBody);
	return sprite;
}

void HelloWorld::gameOver() {
	audio->playEffect(seSplatPath.c_str());
	Director::getInstance()->pause();

	Size screenSize = this->getContentSize();
	Label* gameOverLabel = Label::createWithTTF("YOU DIED","fonts/Day Rom.ttf",60);
	//gameOverLabel->setAnchorPoint(Vec2::ZERO);
	gameOverLabel->setPosition(Vec2(screenSize.width/2,screenSize.height/2));
	gameOverLabel->setColor(Color3B(166,48,48));
	this->addChild(gameOverLabel,zGameOverMessage);

	scoreLabel->setPosition(Vec2(screenSize.width/2,(screenSize.height/2)-50));
	scoreLabel->setColor(Color3B::WHITE);
	scoreLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	scoreLabel->setZOrder(zGameOverMessage);

	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Size winSize = Director::getInstance()->getVisibleSize();
	DrawNode* background = DrawNode::create();
	background->drawSolidRect(origin,winSize,Color4F(0,0,0,0.80f));
	this->addChild(background,zGameOverCover);
}

bool HelloWorld::onContactBegan(PhysicsContact &contact) {
	int a = contact.getShapeA()->getBody()->getCollisionBitmask();
	int b = contact.getShapeB()->getBody()->getCollisionBitmask();

	//Gets food
	if ((a==Tag::FOOD && b==Tag::SNAKE_HEAD)||(a==Tag::SNAKE_HEAD && b==Tag::FOOD)) {
		//Add score
		updateScore();

		//Increase Snake Length on next game loop
		addSection = true;

		createFood();

		return true;
	}
	//Hits self
	else if ((a==Tag::SNAKE_HEAD && b==Tag::SNAKE_CELL)||(a==Tag::SNAKE_CELL && b==Tag::SNAKE_HEAD)) {
		//cout<<"";
		gameOver();
		return true;
	}
	return false;
}

void HelloWorld::onKeyPressed(EventKeyboard::KeyCode keyCode,Event* event) {
	if (!isMoveLocked) {
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_A:
			if (curDirection!=RIGHT)
				curDirection = LEFT;
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:
			if (curDirection!=LEFT)
				curDirection = RIGHT;
			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
		case EventKeyboard::KeyCode::KEY_W:
			if (curDirection!=DOWN)
				curDirection = UP;
			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case EventKeyboard::KeyCode::KEY_S:
			if (curDirection!=UP)
				curDirection = DOWN;
			break;
		}
		isMoveLocked = true;
	}
}
