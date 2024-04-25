#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include<vector>
#include<string>
#include<stdlib.h>
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
bool PAUSE = false;
using namespace std;

SDL_Window* gWindow = NULL; // cửa sổ game
SDL_Renderer* gRenderer = NULL; // bút vẽ
Uint32 gOldTime;


int lv = 1;

// struct tọa độ 1 điểm
class Vector2D
{
public:
	float x;
	float y;

	Vector2D()// Hàm khởi tạo mặc định
	{
		x = 0.0f;
		y = 0.0f;
	}

	Vector2D(float a, float b)// Hàm khởi tạo có tham số
	{
		x = a;
		y = b;
	}
	Vector2D operator+(Vector2D other) {//Phép cộng vector
		return Vector2D(x + other.x, y + other.y);
	}
	Vector2D operator-(Vector2D other) {// Phép trừ vector
		return Vector2D(x - other.x, y - other.y);
	}
	Vector2D operator*(float t) {//Phép nhân vector với 1 số
		return Vector2D(x*t, y*t);
	}
	Vector2D& operator+=(const Vector2D& other) {// Phép cộng rồi gán
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
	if (SDL_Init(SDL_INIT_VIDEO) < 0)// Kiểm tra và hiển thị lỗi nếu fail
	{
		cout << "SDL did not initialise. Error: " << SDL_GetError();
		return false;
	}
	else
	{
		// Tạo cửa sổ game
		gWindow = SDL_CreateWindow("Games Chicken Shoot",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN);
		if (gWindow == NULL)// Kiểm tra và hiển thị lỗi nếu fail
		{
			cout << "Window was not created. Error : " << SDL_GetError();
			return false;
		}
	}
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);// Tạo bút vẽ render
	if (gRenderer != NULL)
	{
		// Khởi tạo SDL_Image và kiểm tả lỗi 
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

	SDL_Surface* pSurface = IMG_Load(path.c_str());//Load hình ảnh 
	if (pSurface != NULL)
	{
		pTexture = SDL_CreateTextureFromSurface(gRenderer, pSurface);// Tạo Texture từ Surface
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
	float mWidth;// Chiều rộng của texture
	float mHeight;// Chiều cao của texture
	SDL_Renderer* mRenderer; // bút để vẽ texture
	Vector2D mPosition;//Vị trí của texture
	SDL_Texture* mTexture;
	Texture2D() {//Hàm khởi tạo mặc định
		mRenderer = NULL;
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
	Texture2D(SDL_Renderer* renderer) { // Hàm khởi tạo với render
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

		//mRenderer = NULL;
	}
	bool LoadFromFile(string path) { // load ảnh
		Free(); // giải phóng ảnh cũ để load ảnh mới
		SDL_Surface* pSurface = IMG_Load(path.c_str());

		if (pSurface != NULL)
		{
			SDL_SetColorKey(pSurface, SDL_TRUE, SDL_MapRGB(pSurface->format, 0, 0xFF, 0xFF)); // xóa nền

			mWidth = pSurface->w; // Lấy chiểu rộng
			mHeight = pSurface->h;//Lấy chiều cao

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
	// Hàm vẽ texture lên màn hình, mặc định vị trí, flip và góc quay cố định
	void Render(SDL_RendererFlip flip, double angle = 0.0f) { 
		Render(mPosition, flip, angle);
	}
	// Hàm vẽ texture lên màn hình, mặc định vị trí, flip và góc quay chỉ định 
	void Render(Vector2D p, SDL_RendererFlip flip, double angle = 0.0f) { 
		SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0x00);
		SDL_Rect renderLocation = { p.x, p.y, mWidth, mHeight }; // lấy hình chữ nhật chứa ảnh
		// vẽ đối tượng lên màn hình xử lý, với khung vừa có, góc xoay angle, và cách lấy đối xứng flip
		SDL_RenderCopyEx(mRenderer, mTexture, NULL, &renderLocation, angle, NULL, flip);
	}
};
static class Math {
public:
	// Hàm chuyển đổi số nguyên thành chuỗi có độ dài 5 ký tự(dùng để hiển thị score)
	static string ConvertToInt(int value) {
		string s = to_string(value);
		while (s.size() < 5) {
			s = "0" + s;
		}
		return s;
	}
	static float Clamp(float value, float min, float max) {// Hàm kiểm tra và giới hạn giá trị trong khoảng min-max
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	}
	// Hàm tính khoảng cách 2D giữa hai điểm
	static float Distance2D(Vector2D x, Vector2D y) {
		return sqrt((x.x - y.x) * (x.x - y.x) + (x.y - y.y) * (x.y - y.y));
	}
	// Hàm tìm giá trị lớn nhất hoặc nhỏ nhất giữa hai số double
	static double MaxMin(double a, double b, int max) {//max = 1 thì tìm max , max = 0 thì tìm min
		if ((a > b && max) || (a < b && !max))
			return a;
		return b;
	}
	// Hàm tính diện tích 2 hcn giao nhau
	static double SChung(Vector2D A, Vector2D B, Vector2D C, Vector2D D,
		Vector2D M, Vector2D N, Vector2D P, Vector2D Q) {
		Vector2D P1;// góc trên bên trái
		P1.x = MaxMin(A.x, M.x, 1);
		P1.y = MaxMin(A.y, M.y, 0);

		Vector2D S1;// góc trên bên phải
		S1.x = MaxMin(B.x, N.x, 0);
		S1.y = MaxMin(B.y, N.y, 0);

		Vector2D Q1;// góc dưới bên phải
		Q1.x = MaxMin(C.x, P.x, 0);
		Q1.y = MaxMin(C.y, P.y, 1);

		Vector2D R1;// góc dưới bên trái
		R1.x = MaxMin(D.x, Q.x, 1);
		R1.y = MaxMin(D.y, Q.y, 1);

		double w = S1.x - P1.x;
		double h = P1.y - R1.y;

		return w * h;
	}
	//Kiểm tra va chạm giữa 2 hcn
	static bool CheckColliderBox2D(Texture2D* rect1, Texture2D *rect2) {
		/*SDL_Rect rect = { rect1->mPosition.x, rect1->mPosition.y, rect1->mWidth, rect1->mHeight };
		SDL_RenderDrawRect(gRenderer, &rect);

		SDL_Rect rect3 = { rect2->mPosition.x, rect2->mPosition.y, rect2->mWidth, rect2->mHeight };
		SDL_RenderDrawRect(gRenderer, &rect3);*/

		if (rect1->mPosition.x + (rect1->mWidth / 2) > rect2->mPosition.x && rect1->mPosition.x + (rect1->mWidth / 2) < rect2->mPosition.x + rect2->mWidth &&
			rect1->mPosition.y + (rect1->mHeight / 2) > rect2->mPosition.y && rect1->mPosition.y + (rect1->mHeight / 2) < rect2->mPosition.y + rect2->mHeight) {
			return true;// Có va chạm
		}
		return false;// Ko va chạm
	}
	// Kiểm tra va chạm giữa hcn và hình tròn 
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
	static int score;// Biến lưu trữ điểm số
	static string txtScore;// Chuỗi static lưu trữ điểm số dưới dạng chuỗi kí tự
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
		UpdateScore();// cập nhật số điểm ban đầu
	}
	// Hiển thị điểm số lên màn hình
	static void Update(Vector2D index = { 10, 20 }) {
		for (int i = 0; i < txtScore.size(); i++) {
			num[txtScore[i] - 48]->Render(Vector2D(i * num[txtScore[i] - 48]->mWidth + index.x, index.y), SDL_FLIP_NONE);
		}
	}

	static void AddScore(int value) {
		score += value;
		UpdateScore();// Cập nhật số điểm sau khi thêm
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
		Mix_PlayChannel(-1, sound2, 0);//Phát âm thanh trên kênh trống đầu tiên
	}
};
enum State {
	normal,//TT bình thường
	slows, // TT chậm
	increases_fire_rate,// tăng tốc độ bắn
	enemy// TT kẻ địch
};
class Information {
public:
	float speed; // tốc độ 
	float dame; // sát thương căn bản
	float defense; // phòng thủ
	Information() {//Hàm khởi tạo mặc định
		this->speed = 0;
		this->dame = 0;
		this->defense = 0;
	}
	// Hàm khởi tạo nhận các giá trị tốc độ, sát thương và phòng thủ
	Information(float speed, float dame, float defense) {
		this->speed = speed;
		this->dame = dame;
		this->defense = defense;
	}
	// Hàm cập nhật thông tin về tốc độ, sát thương và phòng thủ của Character
	void Init(float speed, float dame, float defense) {
		this->speed = speed;
		this->dame = dame;
		this->defense = defense;
	}
};
class Character {
public:
	string tag;//kiểu của đối tượng
	Texture2D *texture;//con trẻ để hiển thị hình ảnh
	Information information;//Lưu trữ thông tin
	State state;//Lưu trữ trạng thái
	string link; // đường dẫn file tổng
	int frame;//Số lượng khung hình
	int deltaFrame = 2;//Tg để chuyển sang khung hình tiếp theo
	float tiLe = 0.02f;//Tỉ lệ tg giữa các khung hình
	bool isActive = true;
	bool flipX = false;
	Character(SDL_Renderer* renderer, Information* infor, string link)  { // khởi tạo
		texture = new Texture2D(renderer);
		if(infor != NULL)
			information = *infor;
		this->link = link;
		frame = 0;
	}
	~Character() {
		texture->Free();
		delete texture;
	}
	
	virtual void Update(SDL_Event &e, float deltaTime = 0){}// Hàm ảo cập nhật trạng thái của Character
	virtual void FixUpdate(){}//Hàm ảo cập nhật thông số của Character
};
class Effect : public Character {
public:
	float timeDelay= 0.2f;// độ trễ giữa các frame
	int frame = 0;//frame hiện tại 
	float k;
	Vector2D p;// vị trí của frame
	Effect(SDL_Renderer* renderer, Vector2D p, float k = 1) : Character(renderer, NULL, "") {
		this->p = p;
		this->k = k;
		//cout << "./Images/Effect/" + to_string(frame) + ".png" << endl;
		texture->LoadFromFile("./Images/Effect/" + to_string(frame) + ".png");
		texture->mHeight *= k;
		texture->mWidth *= k;
		texture->mPosition = p - Vector2D(texture->mWidth / 2, texture->mHeight / 2);//Đặt vị trí ban đầu 
	}
	void Update(SDL_Event& e, float deltaTime = 0) override {// Cập nhật trạng thái của Effect
		if (frame > 4) return;
		if (!PAUSE) {
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
		}
		texture->Render(flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	}
	void FixUpdate() override {}
};
static class ManagerCharacter {
public:
	static vector<Character*> list;// vector lưu trữ các con trỏ đến các Character
	static void push(Character* x) {//Hàm thêm 1 Character vào danh sách
		list.push_back(x);
	}
	// hàm cập nhật trạng thái của Character
	static bool Update(SDL_Event& e, float deltaTime, Character *player) {
		for (int i = 0; i < list.size(); i++) {
			if (!PAUSE) {
				if (!list[i]->isActive) {//Nếu character không hoạt động thì xóa ra khỏi danh sách 
					Character* p = list[i];
					list.erase(list.begin() + i);
					delete p;
					i--;
				}
				else {
					//Kiểm tra va chạm giữa đạn của Enemy với Player
					if (list[i]->tag == "Bullet_enemy") {
						if (Math::CheckColliderBox2D(list[i]->texture, player->texture)) {
							player->isActive = false;// Player trúng đạn
							list[i]->isActive = false;
							return true;//Có va chạm
						}
					}
					//cout << list[i]->tag << endl;
					list[i]->Update(e, deltaTime);//Cập nhật Character
				}
			}else // Nếu game đang tạm dừng, chỉ cập nhật character mà không kiểm tra va chạm
				list[i]->Update(e, deltaTime);
		}
		return false;//Không có va chạm 
		//cout << list.size() << endl;
	}
	//giải phóng bộ nhớ
	static void Free() {
		for (int i = 0; i < list.size(); i++)
			list[i]->texture->Free();
	}
};
vector<Character*> ManagerCharacter::list;
class Bullet : public Character {
public:
	State type; // 0: binh thuong, 1 slow, 2 speed
	//float distance;
	Bullet(SDL_Renderer* renderer, State type, Vector2D p) :Character(renderer, NULL, "") {
		// Xác định tag của đạn dựa trên loại (player hoặc enemy)
		if(type == State::enemy)
			this->tag = "Bullet_enemy";
		else
			this->tag = "Bullet_player";
		string name[] = { "red.png", "orange.png", "blue.png", "enemy.png"};
		//float distancis[] = { 5, 10, 20, 5};
		float dames[] = { 20, 15, 10, 5 };//Dame của các loại đạn
		float speeds[] = { 23, 21, 25, 10 };//Tốc độ của các loại đạn
		float sizes[] = { 1, 1.4, 1 , 2};//Kích thước của các loại đoạn
		//Khởi tạo thôn gitn cho đạn
		this->information.dame = dames[(int)type];
		this->information.speed = speeds[(int)type];
		this->type = type;
		//this->distance = distancis[(int)type];
		//Load ảnh và thay đổi kích thước
		texture->LoadFromFile("./Images/Bullet/" + name[(int)type]);
		texture->mHeight /= sizes[type];
		texture->mWidth /= sizes[type];
		texture->mPosition = p - Vector2D(texture->mWidth/2, texture->mHeight/2);//Đặt vị trí ban đầu
	}
	/*bool CheckCollider(Character charater) {
		return Math::Distance2D(charater.texture->mPosition, texture->mPosition) < distance;
	}*/
	void Run() {
		if (type == State::enemy)//Nếu là đạn của Enemy thì rơi xuống
			texture->mPosition += Vector2D(0, 1) * information.speed * tiLe;
		else//Nếu là đạn của Player thì bắn lên
			texture->mPosition += Vector2D(0, -1) * information.speed * tiLe;
		// nếu vượt quá màn hình thì biến mất
		if (texture->mPosition.x <= 0 || texture->mPosition.x >= SCREEN_WIDTH - texture->mWidth)
			isActive = false;
		if (texture->mPosition.y <= 0 || texture->mPosition.y >= SCREEN_HEIGHT - texture->mHeight)
			isActive = false;
	}
	void Update(SDL_Event &e, float deltaTime = 0) override {
		if (!PAUSE) {
			Run();
		}
		texture->Render(flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);//Cập nhật đạn lên màn hình
	}
	void FixUpdate() override {}
};
class Player : public Character {
public:
	Vector2D move;//Chuyển động của Player
	vector<Bullet*> bullets;//Danh sách đạn của Player
	float delaySpawn = 0.35f;//thời gian giữa mỗi lần bắn đạn 
	float _delaySpawn = 0;
	Player(SDL_Renderer* renderer, Information *infor, string link, Vector2D p) : Character(renderer, infor, link) {
		this->tag = "Player";
		texture->LoadFromFile(link);
		//Thay đổi kích thước của Player
		texture->mHeight /= 4.5;
		texture->mWidth /= 4.5;
		texture->mPosition = p - Vector2D(texture->mWidth, texture->mHeight) * 0.5f;// Đặt vị trí ban đầu 
		state = State::normal;// Trạng thái ban đầu là normal
	}
	~Player() {
		for (int i = 0; i < bullets.size(); i++)
			bullets[i]->texture->Free();
	}
	// Xử lí sự kiện bàn phím 
	void CheckInput(SDL_Event &e) {
		switch (e.type)
		{
			case SDL_KEYDOWN: // bấm phím
				switch (e.key.keysym.sym)
				{
				case SDLK_UP: {
					move.y = -1;//Lên
					break;
				}
				case SDLK_DOWN: {//Xuống
					move.y = 1;
					break;
				}
				case SDLK_LEFT: {//Trái
					move.x = -1;
					break;
				}
				case SDLK_RIGHT: {//Phải
					move.x = 1;
					break;
				}
				}
				break;
			case SDL_KEYUP: // nhả phím
				switch (e.key.keysym.sym)
				{
					case SDLK_UP: 
					case SDLK_DOWN: {
						move.y = 0;//Dừng di chuyển theo trục y
						break;
					}
					case SDLK_LEFT: 
					case SDLK_RIGHT: {
						move.x = 0;//Dừng di chuyển theo trục x
						break;
					}
					case SDLK_s: {//Khi bấm phím s (Bấm s để đổi đạn
						State values[] = { State::normal, State::slows, State::increases_fire_rate};
						float delays[] = { 0.19f, 0.16f, 0.14f };//Tốc độ ra đạn của từng loại đạn (red,orange,blue)
						state = values[(state + 1) % 3];//Chuyển đổi trạng thái của người chơi
						delaySpawn = delays[state];//Cập nhật thời gian giữa mỗi lần bắn
						break;
					}
				}
		}
	}
	void Run() {
		//Di chuyển cảu người chơi
		texture->mPosition += move*information.speed*tiLe;
		// giới hạn người chơi trong màn hình
		texture->mPosition.x = Math::Clamp(texture->mPosition.x, 0, SCREEN_WIDTH - texture->mWidth);
		texture->mPosition.y = Math::Clamp(texture->mPosition.y, 0, SCREEN_HEIGHT - texture->mHeight);
	}
	void Attack(float deltaTime) {
		if (_delaySpawn > 0) {// Nếu > 0 thì chưa tới thời gian bắn và bắt đầu giảm dần cho đến khi <= 0 
			_delaySpawn -= deltaTime;
		}
		else {// <= 0 thì bắn
			_delaySpawn = delaySpawn;
			Audio::Play(0);//Âm thanh của đạn
			//Tạo đạn mới và thêm vào danh sách
			Bullet* p = new Bullet(texture->mRenderer, state, Vector2D(texture->mPosition.x + texture->mWidth/2, texture->mPosition.y));
			bullets.push_back(p);
		}
	}
	void Update(SDL_Event &e, float deltaTime = 0) override {
		if (!isActive) return;//Không hđ thì thoảt
		if (!PAUSE) {
			CheckInput(e);//kiểm tra sự kiện bàn phím
			Run();// Thực hiện di chuyển
			Attack(deltaTime);// Thực hiện bắn
		}
		Show(e, deltaTime);//Hiển thị đạn và người chơi
	}

	void FixUpdate() override {
		
	}
	// Hàm hiển thị người chơi và đạn lên màn hình
	void Show(SDL_Event e, float deltaTime = 0) {
		for (int i = 0; i < bullets.size(); i++) {
			// nếu đạn không hđ nữa thì xóa đạn ra khỏi dsach
			if (!bullets[i]->isActive) {
				Bullet* p = bullets[i];
				bullets.erase(bullets.begin() + i);
				delete p;
				i--;
			}
			else {
				bullets[i]->Update(e);//Cập nhật
			}
		}
		texture->Render(SDL_FLIP_NONE);//Hiển thị
	}
};

class Enemy : public Character {
public:
	float _delaySpawn;//Thời gian tạo Enemy mới
	int type;//loại Enemy
	bool isLeft;//Hướng di chuyển của Enemy
	Vector2D p;//Vị trí hiện tại của Enemy
	Vector2D p2;//Vị trí ban đầu(gốc) của Enemy
	Enemy(SDL_Renderer* renderer, Information* infor, string link, Vector2D p, int type, bool isLeft = true, float k = 1) : Character(renderer, infor, link) {
		this->tag = "Enemy";
		this->isLeft = isLeft;
		//Load hình ảnh và thay đổi kích thước của Enemy
		string names[] = { "1", "2", "3", "boss"};
		texture->LoadFromFile(link + names[type] + ".png");
		texture->mHeight *= k;
		texture->mWidth *= k;
		texture->mPosition = p - Vector2D(texture->mWidth, texture->mHeight) * 0.5f;//Đặt vị trí ban đầu
		p2 = texture->mPosition;//Lưu trữ vị trí gốc của Enemy
		_delaySpawn = (rand() % 11) * 0.7f;//Khởi tạo thời gian chờ giữa các lần bắn đạn
	}
	void Run(float deltatime = 0) {
		switch (type) {
		case 0: {
			p.x += (isLeft ? -1 : 1) * information.speed * tiLe;//Di chuyển theo hướng trái hoặc phải
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
		texture->mPosition = p + p2;//Cập nhật vị trí của Enemy
		//Kiểm tra Enemy với biên màn hình (Nếu vượt quá thì biến mất)
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
			_delaySpawn = (rand() % 11) * 0.7f;//Reset thời gian đợi
			//Tạo đạn mới và thêm vào danh sách quản lí
			Bullet* p = new Bullet(texture->mRenderer, State::enemy, Vector2D(texture->mPosition.x + texture->mWidth / 2, texture->mPosition.y + texture->mHeight));
			ManagerCharacter::push(p);
			
		}
	}
	void Update(SDL_Event& e, float deltaTime = 0) override {
		if (!isActive) return;
		if (!PAUSE) {
			Run(deltaTime);// Enemy di chuyển
			Attack(deltaTime);//Enemy tấn công
		}
		Show();
	}
	//Hiển thị Enemy lên màn hình
	void Show() {
		texture->Render(SDL_FLIP_NONE);
	}
	//Kiểm tra va chạm giữa Enemy và đạn của Player
	void CheckCollider(vector<Bullet*> &bullet) {
		//cout << bullet.size() << endl;
		for (int i = 0; i < bullet.size(); i++)
			if (Math::CheckColliderBox2D(texture, bullet[i]->texture)) {
				bullet[i]->isActive = false;
				isActive = false;
				//Tạo hiệu ứng nổ
				Effect* e = new Effect(texture->mRenderer, texture->mPosition + Vector2D(texture->mWidth, texture->mHeight) * 0.5, 0.5);
				ManagerCharacter::push(e);
				Score::AddScore((type + 1) * 20);// Cộng điểm (Loại 0:20;Loại 1:40 ...)
				Audio::Play(0);//Âm thanh khi Enemy bị tiêu diệt
				//cout << "DES";
				break;
			}
	}
};
class EnemyController {
public:
	SDL_Renderer* renderer;
	vector<Enemy*> listEnemy;//Danh sách đối tượng Enemy
	int level;
	bool isSpawn = false;
	float delaySpawn = 0;
	float _delay = 0;
	int type = 0;//Loại Enemy cần spawn
	int n = 0;//Số lượng Enemy cần spawn
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
	}
	void Update(SDL_Event& e, float deltaTime = 0) {
		for (int i = 0; i < listEnemy.size(); i++) {
			if (!listEnemy[i]->isActive) {//Nếu Enemy ko hđ thì xóa khỏi dsach
				Enemy* p = listEnemy[i];
				listEnemy.erase(listEnemy.begin() + i);
				i--;
				delete p;
			}else
				listEnemy[i]->Update(e, deltaTime);
		}
		if (PAUSE) return;
		Spawn(deltaTime);
	}
	int k = 0;
	void Spawn(float deltaTime = 0) {
		if (isSpawn) {
			if (_delay > 0) {//Nếu vẫn còn thời gian chờ thì giảm thời gian chờ
				_delay -= deltaTime;
				return;
			}
			if (n > 0) {
				switch (type)
				{
					case 0: {
						_delay = 0.75f;//Thời gian Spawn
						// Spwan hai Enemy ở hai đầu màn hình
						listEnemy.push_back(new Enemy(renderer, new Information(5, 20, 2), "./Images/Enemy/", Vector2D(50, 50), type, false, 0.5f));
						listEnemy.push_back(new Enemy(renderer, new Information(5, 20, 2), "./Images/Enemy/", Vector2D(SCREEN_WIDTH - 50, 50), type, true, 0.5f));
						break;
					}
					case 1: {
						_delay = 0.5f;
						// Spwan Enemy theo chiều dọc
						for (int i = 0; i < k; i++) {
							int x = rand() % 2;
							listEnemy.push_back(new Enemy(renderer, new Information(5, 20, 2), "./Images/Enemy/", Vector2D(x ? 550 : 50, i*50), type, x, 0.5f));
						}
						break;
					}
					case 2: {
						_delay = 0.75f;//Thời gian Spawn
						// Spwan hai Enemy ở hai đầu màn hình
						listEnemy.push_back(new Enemy(renderer, new Information(5, 20, 2), "./Images/Enemy/", Vector2D(50, 50), type, false, 0.5f));
						listEnemy.push_back(new Enemy(renderer, new Information(5, 20, 2), "./Images/Enemy/", Vector2D(SCREEN_WIDTH - 50, 50), type, true, 0.5f));
						break;
					}
				}
				n--;// Giảm số lượng Enemy còn lại cần spwan
			}
			else {
				isSpawn = false;// Không còn cần spwan nữa
				delaySpawn = (rand() % 2) * 1;// Thiết lập thời gian chờ giữa các lần spwan mới
			}
		}
		else {
			delaySpawn -= deltaTime;// Giảm thời gian chờ giữa các lần spwan mới
			if (delaySpawn <= 0) {// Nếu hết thời gian chờ
				isSpawn = true;// Bắt đầu spwan Enemy mới
				type = rand() % 3;// Chọn ngẫu nhiên loại của Enemy
				n = rand()%(level + 2)*3;// Số lượng Enemy sẽ được spawn
				if (type == 1) {
					k = rand() % 4 + 3; // Số lượng Enemy theo chiều dọc
				}
				lv++;
			}
		}
	}
	void CheckCollider(vector<Bullet*> &bullets) {
		for (int i = 0; i < listEnemy.size(); i++)
			if(listEnemy[i]->isActive)
				listEnemy[i]->CheckCollider(bullets);
	}
	void CheckCollider(Player* player) {
		for (int i = 0; i < listEnemy.size(); i++)
			if (listEnemy[i]->isActive)
				if (Math::CheckColliderBox2D(listEnemy[i]->texture, player->texture)) {// Kiểm tra va chạm với Player
					player->isActive = false;
					listEnemy[i]->isActive = false;
					//Tạo hiệu ứng
					Effect* e = new Effect(listEnemy[i]->texture->mRenderer, listEnemy[i]->texture->mPosition + Vector2D(listEnemy[i]->texture->mWidth, listEnemy[i]->texture->mHeight) * 0.5, 0.5);
					ManagerCharacter::push(e);
					Score::AddScore((type + 1) * 20);//Cộng điểm
					Audio::Play(0);
				}
	}
};
class BackGround {
	Character* BG1;
	Character* BG2;
public:
	BackGround(SDL_Renderer* renderer) {
		BG1 = new Character(renderer, NULL, "");
		BG1->texture->LoadFromFile("./Images/stars_0.png");
		BG1->texture->mHeight = SCREEN_HEIGHT; //chiều cao của hình nền bằng chiều cao màn hình
		BG1->texture->mWidth = SCREEN_WIDTH;// chiều rộng của hình nền bằng chiều rộng màn hình


		BG2 = new Character(renderer, NULL, "");
		BG2->texture->LoadFromFile("./Images/stars_1.png");
		BG2->texture->mHeight = SCREEN_HEIGHT;
		BG2->texture->mWidth = SCREEN_WIDTH;
	}
	void Update() {
		BG1->texture->Render(SDL_FLIP_NONE);// Vẽ BG1
		BG2->texture->Render(SDL_FLIP_NONE);//Vẽ BG2
	}
	void Free() {
		delete BG1;
		delete BG2;
	}
};
class BOSS : public Character {
public:
	float _delaySpawn;
	int type;
	bool facingRight;
	int hp = 5;
	Player* player;
	Vector2D p;
	Vector2D p2;
	BOSS(SDL_Renderer* renderer, Information* infor, string link, Vector2D p, float k = 1, Player* player = NULL) : Character(renderer, infor, link) {
		this->tag = "Enemy";
		texture->LoadFromFile(link + "boss.png");
		texture->mHeight *= k;
		texture->mWidth *= k;
		texture->mPosition = p - Vector2D(texture->mWidth, texture->mHeight) * 0.5f;
		p2 = texture->mPosition;
		this->player = player;
		_delaySpawn = (rand() % 15) * 1.8f;
		facingRight = true;
		type = 1;
	}
	void Run(float deltatime = 0) {
		switch (type) {
		case 0: {
			
			break;
		}
		case 1: {
			
			break;
		}
		case 2: {
			
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
		switch (type) {
		case 0: {
			
			break;
		}
		case 1: {
			
			break;
		}
		case 2: {

			break;
		}
		}
	}
	void SetType(float deltaTime) {
		_delaySpawn -= deltaTime;
		if (_delaySpawn <= 0) {
			_delaySpawn = (rand() % 15) * 1.8f;
			type = rand() % 3;
		}
	}
	void Update(SDL_Event& e, float deltaTime = 0) override {
		if (!isActive) return;
		if (!PAUSE) {
			SetType(deltaTime);
			Run(deltaTime);
			Attack(deltaTime);
		}
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
				hp--;
				Effect* e = new Effect(texture->mRenderer, texture->mPosition + Vector2D(texture->mWidth, texture->mHeight) * 0.5, 0.5);
				ManagerCharacter::push(e);
				Score::AddScore((type + 1) * 30);
				Audio::Play(0);
				//cout << "DES";
				if (hp <= 0) {
					isActive = false;
					break;
				}
			}
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
		img = new Character(renderer, NULL, "");//Khởi tạo hình ảnh
		txt = new Character(renderer, NULL, "");//Khởi tạo văn bản
		this->type = type;//Loại của button
		this->isActive = true;
		string path = "./Images/UI/";
		string linksImg[] = { "Play.png", "Quit.png" };
		string linksTxt[] = { "txtPlay.png", "txtQuit.png" };
		// Tải hình ảnh cho button và đặt kích thước
		img->texture->LoadFromFile(path + linksImg[type]);
		img->texture->mHeight *= 2*k;
		img->texture->mWidth *= 2*k;
		size = Vector2D(img->texture->mWidth, img->texture->mHeight);
		position = p;
		img->texture->mPosition = p - Vector2D(img->texture->mWidth, img->texture->mHeight)*0.5f;

		// Tải văn bản cho button và đặt kích thước
		txt->texture->LoadFromFile(path + linksTxt[type]);
		txt->texture->mHeight /= 8/k;
		txt->texture->mWidth /= 8/k;
		txt->texture->mPosition = img->texture->mPosition + Vector2D(img->texture->mWidth/2 - txt->texture->mWidth/2, img->texture->mHeight);
	}
	~Button() {
		delete img;
		delete txt;
	}
	//Xử lí sự kiện chuột
	void OnMouse(SDL_Event e) {
		int x = e.button.x;
		int y = e.button.y;
		if (x >= img->texture->mPosition.x && x <= img->texture->mPosition.x + img->texture->mWidth
			&& y >= img->texture->mPosition.y && y <= img->texture->mPosition.y + img->texture->mHeight) {
			OnHigh();//Nếu chuột được di chuyển vào button thì tăng kích thước của button
			switch (e.type)
			{
			case SDL_MOUSEBUTTONUP: {
				OnClick(); // Nếu chuột được nhấn, thay đổi trạng thái của button
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
	// Xử lý sự kiện nhấn nút
	void OnClick() {
		isActive = !isActive;
	}
	// Xử lý sự kiện khi chuột di chuyển vào nút
	void OnHigh() {
		float value = 1;
		img->texture->mWidth = Math::Clamp(img->texture->mWidth + 15, 0, size.x*1.2f); // Tăng chiều rộng lên 10 pixel
		img->texture->mHeight = Math::Clamp(img->texture->mHeight + 15, 0, size.y * 1.2f); // Tăng chiều cao lên 10 pixel
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
	}
	CloseSDL();
	return 0;
}
void Play() {
	SDL_Event e;
	BackGround bg(gRenderer);//Khởi tạo Bground
	Button quit(gRenderer, 1, Vector2D(570, 20), 0.5f);
	Button pause(gRenderer, 0, Vector2D(500, 20), 0.5f);

	Score::SetScore(gRenderer, 1.5f); // Thiết lập hiển thị điểm số
	Player *player = new Player(gRenderer, new Information(10, 50, 20), "./Images/Player/Player1/Player1.png", Vector2D(300, 800));

	bool endGame = false;
	EnemyController enemyController(gRenderer);// Khởi tạo bộ điều khiển kẻ địch
	//BOSS *boss = new BOSS(gRenderer, new Information(5, 20, 2), "./Images/Enemy/", Vector2D(350, 200), 0.5f, player);
	float deltatime = 0; // chenh lẹch 2 frame
	
	while (!endGame) {
		gOldTime = SDL_GetTicks(); // lấy thời gian hiện tại
		SDL_PollEvent(&e); // sự kiện 
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(gRenderer); // xóa dữ liệu cũ
		bg.Update();// Cập nhật nền
		if (ManagerCharacter::Update(e, deltatime, player)) {
			endGame = true;
		}
		
		if (!quit.isActive)
			endGame = true;

		PAUSE = !pause.isActive;// Kiểm tra trạng thái Pause
		enemyController.Update(e, deltatime);
		player->Update(e, deltatime);
		enemyController.CheckCollider(player->bullets);// Kiểm tra va chạm với đạn của người chơi
		enemyController.CheckCollider(player);// Kiểm tra va chạm với người chơi
		if (!player->isActive)
		{
			endGame = true;
		}
		//boss->Update(e, deltatime);
		//boss->CheckCollider(player->bullets);
		Score::Update();// Cập nhật điểm số
		

		quit.Update(e);
		pause.Update(e);
		SDL_zero(e);
		SDL_RenderPresent(gRenderer); // hiển thị ra màn hình
		deltatime = (float)(SDL_GetTicks() - gOldTime) / 1000.0f; // tính độ chênh giữa 2 khung hình
	}
	//giải phóng
	delete player;
	Score::Free();
	bg.Free();
	ManagerCharacter::Free();
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
			Play();//Bắt đầu trò chơi
			play.isActive = true;
			e.button.x = -1;
		}
		else
			if (!quit.isActive) {//Nếu chọn quit thì thoát trò chơi
				return;
			}
		//cout << "main" << endl;
		deltatime = (float)(SDL_GetTicks() - gOldTime) / 1000.0f; // tính độ chênh giữa 2 khung hình
	}


}