#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include<vector>
#include<string>
#include<stdlib.h>
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

using namespace std;

SDL_Window* gWindow = NULL; // cửa sổ game
SDL_Renderer* gRenderer = NULL; // màn hình xử lý
Uint32 gOldTime;


int lv = 1;

// struct tọa độ 1 điểm
class Vector2D
{
public:
	float x;
	float y;

	Vector2D()
	{
		x = 0.0f;
		y = 0.0f;
	}

	Vector2D(float a, float b)
	{
		x = a;
		y = b;
	}
	Vector2D operator+(Vector2D other) {
		return Vector2D(x + other.x, y + other.y);
	}
	Vector2D operator-(Vector2D other) {
		return Vector2D(x - other.x, y - other.y);
	}
	Vector2D operator*(float t) {
		return Vector2D(x * t, y * t);
	}
	Vector2D& operator+=(const Vector2D& other) {
		x += other.x;
		y += other.y;
		return *this;
	}
};

// khởi tạo sdl
bool InitSDL()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	Mix_Init(MIX_INIT_MP3);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "SDL did not initialise. Error: " << SDL_GetError();
		return false;
	}
	else
	{
		gWindow = SDL_CreateWindow("Games Engine Creation",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			cout << "Window was not created. Error : " << SDL_GetError();
			return false;
		}
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer != NULL)
	{
		int imageFlags = IMG_INIT_PNG;
		if (!(IMG_Init(imageFlags) & imageFlags))
		{
			cout << "SDL_Image could not initialise. Error: " << IMG_GetError();
			return false;
		}

	}

	else
	{
		cout << "Renderer could not initialise. Error: " << SDL_GetError();
		return false;
	}

	return true;


}
// đóng sdl
void CloseSDL()
{
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	IMG_Quit();
	SDL_Quit();

	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	Mix_CloseAudio();
	Mix_Quit();
}
// load ảnh
SDL_Texture* LoadTextureFromFile(string path)
{
	SDL_Texture* pTexture = NULL;

	SDL_Surface* pSurface = IMG_Load(path.c_str());
	if (pSurface != NULL)
	{
		pTexture = SDL_CreateTextureFromSurface(gRenderer, pSurface);
		if (pTexture == NULL)
		{
			cout << "Unable to create texture from surface. Error: " << SDL_GetError() << endl;
		}
		SDL_FreeSurface(pSurface);
	}
	else
	{
		cout << "Unable to create texture from surface. Error: " << IMG_GetError() << endl;
	}
	return pTexture;
}
void Play();
void Menu();
// class lưu trữ khi load ảnh lên
class Texture2D
{
public:
	float mWidth;
	float mHeight;
	SDL_Renderer* mRenderer;
	Vector2D mPosition;
	SDL_Texture* mTexture;
	Texture2D() {
		mRenderer = NULL;
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
	Texture2D(SDL_Renderer* renderer) { // khởi tạo
		mRenderer = renderer;
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
	void Free() { // xóa bộ nhớ
		if (mTexture != NULL)
		{
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
		}
		mWidth = 0;
		mHeight = 0;
	}
	~Texture2D() { // giải phóng bộ nhớ
		Free();

		mRenderer = NULL;
	}
	bool LoadFromFile(string path) { // load ảnh
		Free(); // giải phóng cũ
		SDL_Surface* pSurface = IMG_Load(path.c_str());

		if (pSurface != NULL)
		{
			SDL_SetColorKey(pSurface, SDL_TRUE, SDL_MapRGB(pSurface->format, 0, 0xFF, 0xFF)); // xóa nền

			mWidth = pSurface->w; // giảm kích thước ban đầu
			mHeight = pSurface->h;

			mTexture = SDL_CreateTextureFromSurface(mRenderer, pSurface);
			if (mTexture == NULL)
			{
				cout << "Unable to create texture from surface. Error: " << SDL_GetError() << endl;
			}
			SDL_FreeSurface(pSurface);
		}
		else
		{
			cout << "Unable to create texture from surface. Error: " << IMG_GetError() << endl;
		}
		return mTexture != NULL;
	}
	void Render(SDL_RendererFlip flip, double angle = 0.0f) { // load ảnh lên màn hình xử lý
		Render(mPosition, flip, angle);
	}
	void Render(Vector2D p, SDL_RendererFlip flip, double angle = 0.0f) { // load ảnh lên màn hình xử lý
		SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0x00);
		SDL_Rect renderLocation = { p.x, p.y, mWidth, mHeight }; // lấy hình chữ nhật chứa ảnh
		// vẽ đối tượng lên màn hình xử lý, với khung vừa có, góc xoay angle, và cách lấy đối xứng flip
		SDL_RenderCopyEx(mRenderer, mTexture, NULL, &renderLocation, angle, NULL, flip);
	}
};
static class Math {
public:
	static string ConvertToInt(int value) {
		string s = to_string(value);
		while (s.size() < 5) {
			s = "0" + s;
		}
		return s;
	}
	static float Clamp(float value, float min, float max) {
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	}
	static float Distance2D(Vector2D x, Vector2D y) {
		return sqrt((x.x - y.x) * (x.x - y.x) + (x.y - y.y) * (x.y - y.y));
	}
	static bool CheckColliderBox2D(Texture2D* rect1, Texture2D* rect2) {
		/*SDL_Rect rect = { rect1.mPosition.x, rect1.mPosition.y, rect1.mWidth, rect1.mHeight };
		SDL_RenderDrawRect(gRenderer, &rect);

		SDL_Rect rect3 = { rect2.mPosition.x, rect2.mPosition.y, rect2.mWidth, rect2.mHeight };
		SDL_RenderDrawRect(gRenderer, &rect3);*/

		if (rect1->mPosition.x + (rect1->mWidth / 2) > rect2->mPosition.x && rect1->mPosition.x + (rect1->mWidth / 2) < rect2->mPosition.x + rect2->mWidth &&
			rect1->mPosition.y + (rect1->mHeight / 2) > rect2->mPosition.y && rect1->mPosition.y + (rect1->mHeight / 2) < rect2->mPosition.y + rect2->mHeight) {
			return true;
		}
		return false;
	}
	static bool CheckColliderCircle2D(Vector2D O, float r, Texture2D x) {
		// Lấy tọa độ các góc của texture x
		Vector2D topLeft = x.mPosition;
		Vector2D topRight = Vector2D(x.mPosition.x + x.mWidth, x.mPosition.y);
		Vector2D bottomLeft = Vector2D(x.mPosition.x, x.mPosition.y + x.mHeight);
		Vector2D bottomRight = Vector2D(x.mPosition.x + x.mWidth, x.mPosition.y + x.mHeight);

		// Kiểm tra khoảng cách từ các góc đến tâm của hình tròn O
		float distanceTopLeft = Distance2D(topLeft, O);
		float distanceTopRight = Distance2D(topRight, O);
		float distanceBottomLeft = Distance2D(bottomLeft, O);
		float distanceBottomRight = Distance2D(bottomRight, O);

		// Kiểm tra va chạm
		if (distanceTopLeft <= r || distanceTopRight <= r || distanceBottomLeft <= r || distanceBottomRight <= r) {
			return true; // Có va chạm
		}

		return false; // Không có va chạm
	}
};
static class Score {
public:
	static int score;
	static string txtScore;
	static vector<Texture2D*> num;

	static void SetScore(SDL_Renderer* renderer, float k = 1) {
		score = 0;
		string name[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
		for (int i = 0; i < 10; i++) {
			Texture2D* x = new Texture2D(renderer);
			string value = "./Images/Num/" + name[i] + ".png";
			x->LoadFromFile(value);
			x->mHeight *= k;
			x->mWidth *= k;
			num.push_back(x);
		}
		UpdateScore();
	}

	static void Update(Vector2D index = { 10, 20 }) {
		for (int i = 0; i < txtScore.size(); i++) {
			num[txtScore[i] - 48]->Render(Vector2D(i * num[txtScore[i] - 48]->mWidth + index.x, index.y), SDL_FLIP_NONE);
		}
	}

	static void AddScore(int value) {
		score += value;
		UpdateScore();
	}

	static void UpdateScore() {
		txtScore = Math::ConvertToInt(score);
	}
	static void Free() {
		for (int i = 0; i < 10; i++)
			num[i]->Free();
	}
};
int Score::score;
string Score::txtScore;
vector<Texture2D*> Score::num;

static class Audio {
public:
	static void Play(int value) {
		string path = "./Sounds/";
		string name[] = { "gun.wav", "bom.wav" };
		Mix_Chunk* sound2 = Mix_LoadWAV((path + name[value]).c_str());
		Mix_PlayChannel(-1, sound2, 0);
	}
};
enum State {
	normal,
	slows,
	increases_fire_rate,
	enemy
};
class Information {
public:
	//float hp; // máu tối đa
	//float _hp;
	//float mp; // năng lượng tối đa
	//float _mp;
	float speed; // tốc độ 
	float dame; // sát thương căn bản
	float defense; // phòng thủ
	Information() {
		/*this->hp = 0;
		this->_hp = 0;
		this->mp = 0;
		this->_mp = 0;*/
		this->speed = 0;
		this->dame = 0;
		this->defense = 0;
	}
	Information(float speed, float dame, float defense) {
		/*this->hp = hp;
		this->_hp = hp;
		this->mp = mp;
		this->_mp = mp;*/
		this->speed = speed;
		this->dame = dame;
		this->defense = defense;
	}
	void Init(float speed, float dame, float defense) {
		/*this->hp = hp;
		this->_hp = hp;
		this->mp = mp;
		this->_mp = mp;*/
		this->speed = speed;
		this->dame = dame;
		this->defense = defense;
	}
	/*bool AddHp(int value) {
		_hp += value;
		if (_hp <= 0)
			return false;
		if (_hp > hp)
			_hp = hp;
		return true;
	}*/
	/*bool AddMp(int value) {
		_mp += value;
		if (_mp <= 0)
			return false;
		if (_mp > mp)
			_mp = mp;
		return true;
	}*/
};
class Character {
public:
	string tag;
	Texture2D* texture;
	Information information;
	State state;
	string link; // đường dẫn file tổng
	int frame;
	int deltaFrame = 2;
	float tiLe = 0.02f;
	bool isActive = true;
	bool flipX = false;
	Character(SDL_Renderer* renderer, Information* infor, string link) { // khởi tạo
		texture = new Texture2D(renderer);
		if (infor != NULL)
			information = *infor;
		this->link = link;
		frame = 0;
	}
	~Character() {
		texture->Free();
		delete texture;
	}

	virtual void Update(SDL_Event& e, float deltaTime = 0) {}
	virtual void FixUpdate() {}
};
class Effect : public Character {
public:
	float timeDelay = 0.2f;
	int frame = 0;
	float k;
	Vector2D p;
	Effect(SDL_Renderer* renderer, Vector2D p, float k = 1) : Character(renderer, NULL, "") {
		this->p = p;
		this->k = k;
		//cout << "./Images/Effect/" + to_string(frame) + ".png" << endl;
		texture->LoadFromFile("./Images/Effect/" + to_string(frame) + ".png");
		texture->mHeight *= k;
		texture->mWidth *= k;
		texture->mPosition = p - Vector2D(texture->mWidth / 2, texture->mHeight / 2);
	}
	void Update(SDL_Event& e, float deltaTime = 0) override {
		if (frame > 4) return;
		if (timeDelay > 0) {
			timeDelay -= deltaTime;
		}
		else {
			timeDelay = 0.2f;
			frame++;
			if (frame > 8) {
				isActive = false;
			}
			else {
				//cout << "./Images/Effect/" + to_string(frame) + ".png" << endl;
				texture->LoadFromFile("./Images/Effect/" + to_string(frame) + ".png");
				texture->mHeight *= k;
				texture->mWidth *= k;
				texture->mPosition = p - Vector2D(texture->mWidth / 2, texture->mHeight / 2);
			}
		}
		texture->Render(flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	}
	void FixUpdate() override {}
};
static class ManagerCharacter {
public:
	static vector<Character*> list;
	static void push(Character* x) {
		list.push_back(x);
	}
	static bool Update(SDL_Event& e, float deltaTime, Character* player) {
		for (int i = 0; i < list.size(); i++) {
			if (!list[i]->isActive) {
				Character* p = list[i];
				list.erase(list.begin() + i);
				delete p;
				i--;
			}
			else {
				if (list[i]->tag == "Bullet_enemy") {
					if (Math::CheckColliderBox2D(list[i]->texture, player->texture)) {
						player->isActive = false;
						list[i]->isActive = false;
						return true;
					}
				}
				//cout << list[i]->tag << endl;
				list[i]->Update(e, deltaTime);
			}
		}
		return false;
		//cout << list.size() << endl;
	}
	static void Free() {
		for (int i = 0; i < list.size(); i++)
			list[i]->texture->Free();
	}
};
vector<Character*> ManagerCharacter::list;
class Bullet : public Character {
public:
	State type; // 0: binh thuong, 2 slow, 3 speed
	//float distance;
	Bullet(SDL_Renderer* renderer, State type, Vector2D p) :Character(renderer, NULL, "") {
		if (type == State::enemy)
			this->tag = "Bullet_enemy";
		else
			this->tag = "Bullet_player";
		string name[] = { "red.png", "green.png", "blue.png", "enemy.png" };
		//float distancis[] = { 5, 10, 20, 5};
		float dames[] = { 20, 15, 10, 5 };
		float speeds[] = { 20, 22, 18, 6 };
		float sizes[] = { 2, 1.5, 1 , 2 };
		this->information.dame = dames[(int)type];
		this->information.speed = speeds[(int)type];
		this->type = type;
		//this->distance = distancis[(int)type];
		texture->LoadFromFile("./Images/Bullet/" + name[(int)type]);
		texture->mHeight /= sizes[type];
		texture->mWidth /= sizes[type];
		texture->mPosition = p - Vector2D(texture->mWidth / 2, texture->mHeight / 2);
	}
	/*bool CheckCollider(Character charater) {
		return Math::Distance2D(charater.texture->mPosition, texture->mPosition) < distance;
	}*/
	void Run() {
		if (type == State::enemy)
			texture->mPosition += Vector2D(0, 1) * information.speed * tiLe;
		else
			texture->mPosition += Vector2D(0, -1) * information.speed * tiLe;

		if (texture->mPosition.x <= 0 || texture->mPosition.x >= SCREEN_WIDTH - texture->mWidth)
			isActive = false;
		if (texture->mPosition.y <= 0 || texture->mPosition.y >= SCREEN_HEIGHT - texture->mHeight)
			isActive = false;
	}
	void Update(SDL_Event& e, float deltaTime = 0) override {
		Run();
		texture->Render(flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	}
	void FixUpdate() override {}
};
class Player : public Character {
public:
	Vector2D move;
	vector<Bullet*> bullets;
	float delaySpawn = 0.35f;
	float _delaySpawn = 0;
	Player(SDL_Renderer* renderer, Information* infor, string link, Vector2D p) : Character(renderer, infor, link) {
		this->tag = "Player";
		texture->LoadFromFile(link);
		texture->mHeight /= 5;
		texture->mWidth /= 5;
		texture->mPosition = p - Vector2D(texture->mWidth, texture->mHeight) * 0.5f;
		state = State::normal;
	}
	~Player() {
		for (int i = 0; i < bullets.size(); i++)
			bullets[i]->texture->Free();
	}
	void CheckInput(SDL_Event& e) {
		switch (e.type)
		{
		case SDL_KEYDOWN: // bấm phím
			switch (e.key.keysym.sym)
			{
			case SDLK_UP: {
				move.y = -1;
				break;
			}
			case SDLK_DOWN: {
				move.y = 1;
				break;
			}
			case SDLK_LEFT: {
				move.x = -1;
				break;
			}
			case SDLK_RIGHT: {
				move.x = 1;
				break;
			}
			}
			break;
		case SDL_KEYUP: // bấm phím
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:
			case SDLK_DOWN: {
				move.y = 0;
				break;
			}
			case SDLK_LEFT:
			case SDLK_RIGHT: {
				move.x = 0;
				break;
			}
			case SDLK_s: {
				State values[] = { State::normal, State::slows, State::increases_fire_rate };
				float delays[] = { 0.2f, 0.12f, 0.3f };
				state = values[(state + 1) % 3];
				delaySpawn = delays[state];
				break;
			}
			}
		}
	}
	void Run() {
		texture->mPosition += move * information.speed * tiLe;
		texture->mPosition.x = Math::Clamp(texture->mPosition.x, 0, SCREEN_WIDTH - texture->mWidth);
		texture->mPosition.y = Math::Clamp(texture->mPosition.y, 0, SCREEN_HEIGHT - texture->mHeight);
	}
	void Attack(float deltaTime) {
		if (_delaySpawn > 0) {
			_delaySpawn -= deltaTime;
		}
		else {
			_delaySpawn = delaySpawn;
			Audio::Play(0);
			Bullet* p = new Bullet(texture->mRenderer, state, Vector2D(texture->mPosition.x + texture->mWidth / 2, texture->mPosition.y));
			bullets.push_back(p);
		}
	}
	void Update(SDL_Event& e, float deltaTime = 0) override {
		if (!isActive) return;
		CheckInput(e);
		Run();
		Attack(deltaTime);

		Show(e, deltaTime);
	}

	void FixUpdate() override {

	}
	void Show(SDL_Event e, float deltaTime = 0) {
		for (int i = 0; i < bullets.size(); i++) {
			if (!bullets[i]->isActive) {
				Bullet* p = bullets[i];
				bullets.erase(bullets.begin() + i);
				delete p;
				i--;
			}
			else {
				bullets[i]->Update(e);
			}
		}
		texture->Render(SDL_FLIP_NONE);
	}
};

class Enemy : public Character {
public:
	float _delaySpawn;
	int type;
	bool isLeft;
	Vector2D p;
	Vector2D p2;
	Enemy(SDL_Renderer* renderer, Information* infor, string link, Vector2D p, int type, bool isLeft = true, float k = 1) : Character(renderer, infor, link) {
		this->tag = "Enemy";
		this->isLeft = isLeft;
		string names[] = { "1", "2", "3", "boss" };
		texture->LoadFromFile(link + names[type] + ".png");
		texture->mHeight *= k;
		texture->mWidth *= k;
		texture->mPosition = p - Vector2D(texture->mWidth, texture->mHeight) * 0.5f;
		p2 = texture->mPosition;
		_delaySpawn = (rand() % 15) * 1.8f;
	}
	void Run(float deltatime = 0) {
		switch (type) {
		case 0: {
			p.x += (isLeft ? -1 : 1) * information.speed * tiLe;
			p.y = abs(p.x);
			break;
		}
		case 1: {
			p.x += information.speed * tiLe;
			break;
		}
		case 2: {
			p.x += (rand() % 2 ? information.speed * tiLe : -information.speed * tiLe) * 4;
			p.y += (rand() % 2 ? information.speed * tiLe : -information.speed * tiLe) * 4;
			break;
		}
		}
		texture->mPosition = p + p2;
		if (texture->mPosition.x <= 0 || texture->mPosition.x >= SCREEN_WIDTH - texture->mWidth)
			isActive = false;
		if (texture->mPosition.y <= 0 || texture->mPosition.y >= SCREEN_HEIGHT - texture->mHeight)
			isActive = false;
	}
	void Attack(float deltaTime) {
		if (_delaySpawn > 0) {
			_delaySpawn -= deltaTime;
		}
		else {
			_delaySpawn = (rand() % 15) * 1.8f;
			Bullet* p = new Bullet(texture->mRenderer, State::enemy, Vector2D(texture->mPosition.x + texture->mWidth / 2, texture->mPosition.y + texture->mHeight));
			ManagerCharacter::push(p);

		}
	}
	void Update(SDL_Event& e, float deltaTime = 0) override {
		if (!isActive) return;
		Run(deltaTime);
		Attack(deltaTime);

		Show();
	}
	void Show() {
		texture->Render(SDL_FLIP_NONE);
	}
	void CheckCollider(vector<Bullet*>& bullet) {
		//cout << bullet.size() << endl;
		for (int i = 0; i < bullet.size(); i++)
			if (Math::CheckColliderBox2D(texture, bullet[i]->texture)) {
				bullet[i]->isActive = false;
				isActive = false;
				Effect* e = new Effect(texture->mRenderer, texture->mPosition + Vector2D(texture->mWidth, texture->mHeight) * 0.5, 0.5);
				ManagerCharacter::push(e);
				Score::AddScore((type + 1) * 30);
				Audio::Play(0);
				//cout << "DES";
				break;
			}
	}
};
class EnemyController {
public:
	SDL_Renderer* renderer;
	vector<Enemy*> listEnemy;
	int level;
	bool isSpawn = false;
	float delaySpawn = 0;
	float _delay = 0;
	int type = 0;
	int n = 0;
	EnemyController(SDL_Renderer* g) {
		renderer = g;
		level = 1;
	}
	~EnemyController() {
		for (int i = 0; i < listEnemy.size(); i++) {
			delete listEnemy[i];
			listEnemy.erase(listEnemy.begin() + i);
			i--;
		}
		delete renderer;
	}
	void Update(SDL_Event& e, float deltaTime = 0) {
		for (int i = 0; i < listEnemy.size(); i++) {
			if (!listEnemy[i]->isActive) {
				Enemy* p = listEnemy[i];
				listEnemy.erase(listEnemy.begin() + i);
				i--;
				delete p;
			}
			else
				listEnemy[i]->Update(e, deltaTime);
		}
		//cout << listEnemy.size() << endl;
		Spawn(deltaTime);
	}
	int k = 0;
	void Spawn(float deltaTime = 0) {
		if (isSpawn) {
			if (_delay > 0) {
				_delay -= deltaTime;
				return;
			}
			if (n > 0) {
				switch (type)
				{
				case 0: {
					_delay = 0.75f;
					listEnemy.push_back(new Enemy(renderer, new Information(5, 20, 2), "./Images/Enemy/", Vector2D(50, 50), type, false, 0.5f));
					listEnemy.push_back(new Enemy(renderer, new Information(5, 20, 2), "./Images/Enemy/", Vector2D(SCREEN_WIDTH - 50, 50), type, true, 0.5f));
					break;
				}
				case 1: {
					_delay = 0.75f;
					for (int i = 0; i < k; i++) {
						int x = rand() % 2;
						listEnemy.push_back(new Enemy(renderer, new Information(5, 20, 2), "./Images/Enemy/", Vector2D(x ? 550 : 50, i * 50), type, x, 0.5f));
					}
					break;
				}
				case 2: {
					_delay = 0.75f;
					listEnemy.push_back(new Enemy(renderer, new Information(5, 20, 2), "./Images/Enemy/", Vector2D(50, 50), type, false, 0.5f));
					listEnemy.push_back(new Enemy(renderer, new Information(5, 20, 2), "./Images/Enemy/", Vector2D(SCREEN_WIDTH - 50, 50), type, true, 0.5f));
					break;
				}
				}
				n--;
			}
			else {
				isSpawn = false;
				delaySpawn = (rand() % 2) * 1;
			}
		}
		else {
			delaySpawn -= deltaTime;
			if (delaySpawn <= 0) {
				isSpawn = true;
				type = rand() % 3;
				n = rand() % (level + 2) * 3;
				if (type == 1) {
					k = rand() % 4 + 3;
					//cout << "k " << k << endl;
				}
				lv++;
				//cout << type << " " << n << endl;
			}
		}
	}
	void CheckCollider(vector<Bullet*>& bullets) {
		for (int i = 0; i < listEnemy.size(); i++)
			if (listEnemy[i]->isActive)
				listEnemy[i]->CheckCollider(bullets);
	}
};
class BackGround {
	Character* BG1;
	Character* BG2;
public:
	BackGround(SDL_Renderer* renderer) {
		BG1 = new Character(renderer, NULL, "");
		BG1->texture->LoadFromFile("./Images/stars_0.png");
		BG1->texture->mHeight = SCREEN_HEIGHT;
		BG1->texture->mWidth = SCREEN_WIDTH;

		BG2 = new Character(renderer, NULL, "");
		BG2->texture->LoadFromFile("./Images/stars_1.png");
		BG2->texture->mHeight = SCREEN_HEIGHT;
		BG2->texture->mWidth = SCREEN_WIDTH;
	}
	void Update() {
		BG1->texture->Render(SDL_FLIP_NONE);
		BG2->texture->Render(SDL_FLIP_NONE);
	}
	void Free() {
		delete BG1;
		delete BG2;
	}
};



class Button {
public:
	Character* img;
	Character* txt;
	Vector2D size;
	Vector2D position;
	int type;
	bool isActive;
	Button(SDL_Renderer* renderer, int type, Vector2D p, float k) {
		img = new Character(renderer, NULL, "");
		txt = new Character(renderer, NULL, "");
		this->type = type;
		this->isActive = true;
		string path = "./Images/UI/";
		string linksImg[] = { "Play.png", "Quit.png" };
		string linksTxt[] = { "txtPlay.png", "txtQuit.png" };
		img->texture->LoadFromFile(path + linksImg[type]);
		img->texture->mHeight *= 2 * k;
		img->texture->mWidth *= 2 * k;
		size = Vector2D(img->texture->mWidth, img->texture->mHeight);
		position = p;
		img->texture->mPosition = p - Vector2D(img->texture->mWidth, img->texture->mHeight) * 0.5f;


		txt->texture->LoadFromFile(path + linksTxt[type]);
		txt->texture->mHeight /= 8 / k;
		txt->texture->mWidth /= 8 / k;
		txt->texture->mPosition = img->texture->mPosition + Vector2D(img->texture->mWidth / 2 - txt->texture->mWidth / 2, img->texture->mHeight);
	}
	~Button() {
		delete img;
		delete txt;
	}
	void OnMouse(SDL_Event e) {
		int x = e.button.x;
		int y = e.button.y;
		if (x >= img->texture->mPosition.x && x <= img->texture->mPosition.x + img->texture->mWidth
			&& y >= img->texture->mPosition.y && y <= img->texture->mPosition.y + img->texture->mHeight) {
			OnHigh();
			switch (e.type)
			{
			case SDL_MOUSEBUTTONUP: {
				OnClick();
				break;
			}
			}

		}
		else {
			img->texture->mWidth = size.x;
			img->texture->mHeight = size.y;
			img->texture->mPosition = position - Vector2D(img->texture->mWidth, img->texture->mHeight) * 0.5f;
		}

	}
	void OnClick() {
		isActive = false;
	}
	void OnHigh() {
		float value = 1;
		img->texture->mWidth = Math::Clamp(img->texture->mWidth + 10, 0, size.x * 1.2f); // Tăng chiều rộng lên 10 pixel
		img->texture->mHeight = Math::Clamp(img->texture->mHeight + 10, 0, size.y * 1.2f); // Tăng chiều cao lên 10 pixel
		img->texture->mPosition = position - Vector2D(img->texture->mWidth, img->texture->mHeight) * 0.5f;
	}
	void Update(SDL_Event e) {
		OnMouse(e);
		Render();
	}
	void Render() {
		img->texture->Render(SDL_FLIP_NONE);
		txt->texture->Render(SDL_FLIP_NONE);
	}
};

int main(int argc, char* args[])
{
	if (InitSDL())
	{
		srand(time(0)); // cập nhật thời gian hiện tại để làm mới random
		Mix_Chunk* sound = Mix_LoadWAV("./Sounds/BackGround.wav");
		Mix_PlayChannel(-1, sound, 0);
		Menu();
		//Play();
		/*Texture2D *A = new Texture2D(gRenderer);
		A->mPosition = Vector2D(200, 50);
		A->mHeight = 50;
		A->mWidth = 50;

		Texture2D *B = new Texture2D(gRenderer);
		B->mPosition = Vector2D(50, 50);
		B->mHeight = 50;
		B->mWidth = 50;

		if (Math::CheckColliderBox2D(*A, *B))
			cout << "true";*/
	}
	CloseSDL();
	return 0;
}
void Play() {


	SDL_Event e;
	BackGround bg(gRenderer);
	Button quit(gRenderer, 1, Vector2D(570, 20), 0.5f);
	Score::SetScore(gRenderer, 1.5f);
	Player* player = new Player(gRenderer, new Information(10, 50, 20), "./Images/Player/Ship1.png", Vector2D(300, 800));

	bool endGame = false;
	EnemyController enemyController(gRenderer);
	float deltatime = 0; // chenh lẹch 2 frame
	while (!endGame) {
		gOldTime = SDL_GetTicks(); // lấy thời gian hiện tại
		SDL_PollEvent(&e); // sự kiện 
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(gRenderer); // xóa dữ liệu cũ

		bg.Update();
		if (ManagerCharacter::Update(e, deltatime, player)) {
			endGame = true;
			/*CloseSDL();
			return;*/
		}

		if (!quit.isActive)
			endGame = true;
		enemyController.Update(e, deltatime);
		//x.Update(e, deltatime);
		player->Update(e, deltatime);

		enemyController.CheckCollider(player->bullets);


		quit.Update(e);
		Score::Update();
		SDL_zero(e);
		SDL_RenderPresent(gRenderer); // hiển thị ra màn hình
		deltatime = (float)(SDL_GetTicks() - gOldTime) / 1000.0f; // tính độ chênh giữa 2
	}
	delete player;
	Score::Free();
	bg.Free();
	ManagerCharacter::Free();
	//enemyController.Free();
}
void Menu() {
	SDL_Event e;
	BackGround bg(gRenderer);

	Button play(gRenderer, 0, Vector2D(300, 300), 1.5f);
	Button quit(gRenderer, 1, Vector2D(300, 500), 1.5f);
	float deltatime = 0; // chenh lẹch 2 frame
	while (true) {
		SDL_PollEvent(&e); // sự kiện 
		gOldTime = SDL_GetTicks(); // lấy thời gian hiện tại
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(gRenderer); // xóa dữ liệu cũ

		bg.Update();
		play.Update(e);
		quit.Update(e);

		SDL_RenderPresent(gRenderer); // hiển thị ra màn hình
		if (!play.isActive) {
			Play();
			play.isActive = true;
			e.button.x = -1;
		}
		else
			if (!quit.isActive) {
				return;
			}
		//cout << "main" << endl;
		deltatime = (float)(SDL_GetTicks() - gOldTime) / 1000.0f; 
		CloseSDL();
	}


}