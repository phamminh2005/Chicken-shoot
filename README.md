# ***GAME CHICKEN SHOOT***
## **Giới thiệu**
+ Họ và Tên : Phạm Văn Minh.
+ MSSV: 23020118 Lớp : K68CB.
+ Bài tập lớn: **Game Chicken Shoot** / INT2215 50 - Lập tình nâng cao.
## **Cách cài đặt Game**
***Nếu chỉ muốn tải và chơi Game (không bao gồm code) thì hảy tải và giải nén file zip được đóng gói qua link dưới đây và click vào file setup.exe để chơi Game.***
+ [Link :]() **https://drive.google.com/file/d/1cA98LLtge_AK81Tb1b5-M10Q7ZH9KbyD/view?usp=sharing**

***Nếu muốn cài đặt game và mã nguồn thì cần thực hiện những bước sau đây:***
*Trước tiên hãy cài đặt Mingw và thue viện SDL2 cho máy của bạn(bao gồm SDL_image,SDL_mixer và SDL_ttf)*
### Cài đặt thư viện SDL2 và Mingw
- Hướng dẫn cài Mingw: https://www.geeksforgeeks.org/installing-mingw-tools-for-c-c-and-changing-environment-variable/
- Hướng dẫn cài SDL: https://lazyfoo.net/tutorials/SDL/01_hello_SDL/index.php
- Hướng dẫn cài SDL_Image: https://lazyfoo.net/tutorials/SDL/06_extension_libraries_and_loading_other_image_formats/index.php
- Hướng dẫn cài SDL_Mixer: https://lazyfoo.net/tutorials/SDL/21_sound_effects_and_music/index.php
- Hướng dẫn cài SDL_TTF: https://lazyfoo.net/tutorials/SDL/16_true_type_fonts/index.php
### Cài đặt và chơi game
- Bước tiếp theo là cài đặt SDL2 vào Visual Studio Community 2022: Các bạn có thể tham khảo cách cài đặt ở đây:[]()**https://www.youtube.com/watch?v=UgJadfInqyo&t=1117s**

- Tải game về dưới dạng file zip hoặc dùng câu lệnh sau:
```
git clone https://github.com/phamminh2005/Chicken-shoot.git
```
- Cuối cùng, mở project (giải nén file nếu cần) và trải nghiệm game.
- Nếu không thể khởi động Game hãy liên hệ qua gmail: []()**minhrooney161005@gmail.com**

## Mô tả Game
### Câu chuyện
- Người chơi sẽ nhập vai thành một cỗ máy chiến đấu đang chống lại lũ gà xâm chiếm hành tinh. Ở nơi đây, người chơi phải sự dụng sự thông minh và nhanh nhẹn của mình để tiêu diệt lũ gà. Hãy cố gắng tiêu diệt được thật nhiều gà.
### Cách chơi
- Người chơi có 1 mạng và phải di chuyển nhanh nhẹn để tiêu diệt được nhiều quái và đạt được số điểm cao.
- Có 3 loại quái trong game:

   + Loại 1: ![alt tag](https://github.com/phamminh2005/Chicken-shoot/blob/master/Debug/Images/Enemy/1.png)
   + Loại 2: ![alt tag](https://github.com/phamminh2005/Chicken-shoot/blob/master/Images/Enemy/2.png)
   + Loại 3: ![alt tag](https://github.com/phamminh2005/Chicken-shoot/blob/master/Images/Enemy/3.png)

- Có 4 loại đạn trong game(3 loại của Player và 1 loại của Enemy):
   + Loại 1(Player): ![alt tag](https://github.com/phamminh2005/Chicken-shoot/blob/master/Images/Bullet/red.png)
   + Loại 2(Pkayer): ![alt tag](https://github.com/phamminh2005/Chicken-shoot/blob/master/Images/Bullet/orange.png)
   + Loại 3(Player): ![alt tag](https://github.com/phamminh2005/Chicken-shoot/blob/master/Images/Bullet/blue.png)
   + Loại 1(Enemy): ![alt tag](https://github.com/phamminh2005/Chicken-shoot/blob/master/Images/Bullet/enemy.png)
- Quái sẽ được xuất hiện ngẫu nhiên giữa 3 loại trên, các quái sẽ được xuất hiện theo kiểu riêng của từng loại. Khi quái xuất hiện, các quái sẽ ngẫu nhiên bắn ra bullet và người chơi phải sử dụng sự nhanh nhẹn của mình để né bullet của quái. Nếu bị bắn trúng người chơi sẽ thua cuộc. Ngoài ra khi va chạm với quái, người chơi cũng sẽ thua cuộc . Hãy nhớ di chuyển thật cẩn thận nhé !!!
### Hướng dẫn chơi
- Đối với Menu:
   + Sử dụng chuột để tương tác với các chức năng của menu.
   + Click vào Play để bắt đầu chơi Game và click vào Quit để thoát game.
   + Trong quá trình chơi click vào Play(Bên trên góc phải màn hình) để tạm dừng và click vào quit để thoát game.
- Đối với người chơi:
   + Sử dụng phím UP để di chuyển lên trên.
   + Sử dụng phím DOWN để di chuyển xuống dưới.
   + Sử dụng phím RIGHT để di chuyển sang phải.
   + Sử dụng phím LEFT để di chuyển sang trái.
   + Sử dụng phím s để đổi các loại đạn.
## **Các kỹ thuật sử dụng trong Game**
- Xây dựng và xử lý va chạm giữa Enemy và Player, đạn của Enemy với Player, đạn của Player với Enemy. Khi xảy ra va chạm thì Enemy sẽ có hiệu ứng nổ.
- Xây dựng các class. Sử dụng lớp kế thừa, quản lý và liên kết các class.
- Xử lý đồ họa, tương tác âm thanh và chuyển động của các class( Player, Enemy... ).
- Đối với các đối tượng quái (Enemy):
   + Tự động xuất hiện từ 2 phía màn hình.
   + Tự động bắn đạn để tiêu diệt Player.
- Đối với các đối tượng Player: 
   + Tự động bắn đạn và có thể đối loại đạn.
## **Source code game**
- Folder setup chứa các folder nhỏ hơn:
   + Folder Images: Chứa các hình ảnh được sử dụng trong game (Enemy,Player,Bullet,...)
   + Folder Sounds: Chứa các file âm thanh nền của game, âm thanh của đạn bắn.
   + Folder SDL Files: Chứa các file cần thiết của thư viện SDL2 được sử dụng để chạy Game.
- File main.cpp: Chứa toàn bộ code của Game.
- File setup.exe: Dùng để chạy Game.

## **Preview**
![alt tag](https://github.com/phamminh2005/Chicken-shoot/blob/master/Images/Preview/Preview1.png)
![alt tag](https://github.com/phamminh2005/Chicken-shoot/blob/master/Images/Preview/Preview2.png)
![alt tag](https://github.com/phamminh2005/Chicken-shoot/blob/master/Images/Preview/Preview3.png)
![alt tag](https://github.com/phamminh2005/Chicken-shoot/blob/master/Images/Preview/Preview4.png)

## **Tổng kết**
### ***Tâm đắc***

- Toàn bộ code là do em tự tham khảo và áp dụng để thực hiện. Tìm hiểu các chức năng của thư viện SDL2 và áp dụng để phát triển game.
- Em học được thêm về cách quản lí project , biết code 1 game C++ đơn giản có sử dụng thư viện SDL2 .Học được cách làm việc với github, kỹ năng debug và tạo README. 
- Biết được cách 1 game hoạt động như thế nào.
### ***Hạn chế***
- Chưa tách được file thành các file .h,.cpp và .hpp để quản lí project 1 cách dễ dàng hơn. Em đan tìm hiểu và thực hiện việc tách file nhưng trong quá trình đó xuất hiện 1 số lỗi và em chưa khắc phục được. 
- Game chưa có sự hấp dẫn:
   + Chưa có sự xuất hiện của BOSS với cách tấn công đa dạng và hay hơn(Bắn tỉa,...).Em đã cố gắng thêm BOSS vào game nhưng vẫn đang còn 1 số lỗi chưa debug được.
   + Chưa có nhiều loại đạn của Player và cách để thay đổi loại đạn . Có thể là hạ gục Enemy để nhận được 1 item nhất định và sau đó sẽ được nâng cấp đạn.
- Hướng phát triển về sau:
   + Tạo thêm BOSS và các item để Game được hấp dẫn hơn.
   + Xây dựng chế độ đánh theo màn, đánh giới hạn thời gian và có thể chọn độ khó cho Game.

**Cảm ơn thầy và các bạn đã đọc và chơi Game. Nếu có thắc mắc về game hãy liên hệ với gmail: []() **minhrooney161005@gmail.com**** 






