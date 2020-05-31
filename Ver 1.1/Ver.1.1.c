/*
최초 작성자 : 이동빈 (인천대학교 정보통신공학과)
최초 작성일 : 2020.04.15
최초 변경일 : 2020.04.21
목적 : 개인 프로젝트
개정 이력 : 이동빈, 2020.04.27(ver.1.1)

최종 버전 : ver.1.1
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <stdbool.h>

#define EASY 1 //난이도
#define HARD 2 //난이도
#define XLIMIT 60 //가로길이 : 60
#define YLIMIT 20 //세로길이 : 20
#define FRAME 5 //게임 프레임
#define UNIT 100 //물체 최대 개수

//글씨 색깔 상수,
#define DARKGRAY 8
#define LIGHTCYAN 11
#define YELLOW 14
#define LIGHTGREEN 10
#define LIGHTMAGENTA 13

//물체가 무엇인지 구분하는 상수.
const int COIN = 1;
const int OBSTACLE = 2;
const int PLAYER = 3;

typedef struct element {
    int x, y; //x,y좌표.
    int id; //물체의 특성.(코인/장애물/캐릭터)
    int speed; //물체가 떨어지는 속도 = FRAME * speed(ms).
    int period; //period == 0일 때, object가 한 칸 내려감. (speed값을 주기로)
    bool create; //물체의 생성 여부.
}element;

int start(); //게임 시작 전 메인 화면.
int running_game(int difficulty); //게임 구동 화면.
bool Gameover(int score, int runtime); //결과창

void gotoxy(int x, int y); //커서 위치 이동.
void show(element object); //물체 출력
void erase(element object); //물체 지우기
bool is_collide(element drop, element player); //캐릭터와 물체의 충돌 여부.
element create_object(element object, int speed[2]); //물체 생성.
void move_player(element* player); //캐릭터의 움직임 구현.
void outline(); //게임 화면의 틀 그리기.
void textColor(int color); //글씨 색 조정.

int main() {
    while (true) {
        int difficulty = start(); //난이도는 시작화면에서 결정됨.
        int score = 0;
        clock_t starttime, endtime;
        srand((unsigned)time(NULL));

        starttime = clock();
        score = running_game(difficulty);
        endtime = clock();

        int runtime = (int)endtime - (int)starttime;
        if (Gameover(score, runtime) == false)
            break;
    }
    gotoxy(10, 27); //종료 메시지 출력 위치.
    return 0;
}

void gotoxy(int x, int y) {
    COORD Pos = { x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

int start() {
    textColor(YELLOW);
    system("cls");
    outline();
    {gotoxy(5, 1);
    printf("다운로드 : https://github.com/dongbin1999/Programming-Project");
    gotoxy(5, 2);
    printf("연락처(github) : https://dongbin1999.github.io/jekyll/update/Programming-project-Game-Demo/");
    gotoxy(5, 3);
    printf("연락처(email) : dongbin1999@inu.ac.kr");
    gotoxy(27, 7);
    printf("[장애물 피하기 게임 Ver.1.1]\n\n");
    gotoxy(35, 11);
    textColor(LIGHTCYAN);
    printf("[게임 방법]\n");
    gotoxy(15, 13);
    printf("1) 방향키로 캐릭터를 움직여 장애물을 피합니다.\n");
    gotoxy(15, 14);
    printf("2) 코인을 먹으면 10점을 획득합니다.\n");
    gotoxy(15, 15);
    printf("3) 주어진 라이프를 모두 소모하면 게임이 종료됩니다.\n");
    gotoxy(20, 17);
    textColor(YELLOW);
    printf("난이도를 설정하세요 (쉬움 : 1, 어려움 : 2)"); }

    int difficulty = 0;
    char command = 0;
    while (true) {
        command = getch();
        if ((command != '1') && (command != '2'))
            continue;
        else {
            difficulty = command - '0';
            break;
        }
    }

    gotoxy(20, 19);
    printf("난이도 : %s를 선택하셨습니다.", (difficulty == EASY) ? "쉬움" : "어려움");
    {Sleep(500);
    gotoxy(20, 21);
    printf("아무키나 입력하여 게임을 시작하세요!");
    getch();

    for (int i = 3; i > 0; i--) {
        system("cls");
        printf("%d초 후 게임이 시작됩니다...\n", i);
        Sleep(1000);
    }
    system("cls"); }
    return difficulty;
}

bool is_collide(element drop, element player) {
    //물체의 x좌표 크기가 2이므로, x축은 범위로 비교.
    if (((player.x - 1 <= drop.x) && (player.x + 1 >= drop.x)) &&
        //y축도 코인 획득 오류를 없애기 위해, 아래 한칸까지 허용.
        ((drop.y == player.y) || (drop.y - 1 == player.y)))
        return true;
    else
        return false;
}

int running_game(int difficulty) {
    element object[UNIT] = { {0} }, player = { 35, YLIMIT + 5 - 1,PLAYER };
    int i = 0;

    //probability : 물체가 생성될 확률.(1/probability), period[2] : 물체가 떨어지는 속도의 최대/최소 범위.
    int probability = 0, speed[2] = { 0,0 }, score = 0, life = 0;

    for (i = 0; i < UNIT; i++)
        object[i].create = false; //처음엔 물체가 생성 안된 상태로 초기화.

    if (difficulty == EASY) {
        probability = 50; speed[0] = 30; speed[1] = 60; life = 3;
    }
    else if (difficulty == HARD) {
        probability = 10; speed[0] = 10; speed[1] = 20; life = 1;
    }
    else;

    textColor(LIGHTGREEN);
    system("cls");
    outline();
    gotoxy(80, 5);
    textColor(YELLOW);
    printf("▤  점수 : %4d ▤", score);
    gotoxy(80, 7);
    if (life == 1)
        textColor(LIGHTMAGENTA);
    printf("▤  라이프 : %d  ▤", life);

    while (1) {
        //장애물 or 코인 한개 생성.
        if (rand() % probability == 0)
            for (i = 0; i < UNIT; i++)
                if (object[i].create == false) {
                    object[i] = create_object(object[i], speed);
                    break;
                }

        Sleep(FRAME);
        show(player);

        //캐릭터의 움직임 구현. 주소값을 직접 전달.
        move_player(&player);

        //물체가 떨어지고, 그 때 캐릭터와 충돌했는지 확인하는 반복문.
        for (i = 0; i < UNIT; i++) {
            if (object[i].create == true) {
                object[i].period--;
                if (object[i].period <= 0) {
                    object[i].period = object[i].speed; //다시 다음 1칸 떨어질 차례를 기다림.

                    erase(object[i]);
                    object[i].y++;//1칸 아래로 떨어짐.
                    show(object[i]);

                    if (is_collide(object[i], player)) {
                        erase(object[i]);
                        show(player);
                        object[i].create = false;
                        if (object[i].id == COIN) {
                            score += 10;
                            gotoxy(80, 5);
                            textColor(YELLOW);
                            printf("▤  점수 : %4d ▤", score);
                        }
                        else if (object[i].id == OBSTACLE) {
                            life--;
                            gotoxy(80, 7);
                            if (life <= 1)
                                textColor(LIGHTMAGENTA);
                            else
                                textColor(YELLOW);
                            printf("▤  라이프 : %d  ▤", life);
                            if (life == 0) {
                                gotoxy(10, 27);
                                textColor(YELLOW);
                                system("pause");
                                return score;
                            }
                        }
                        else;
                    }

                    //물체가 바닥에 도달해 사라짐.
                    if (object[i].y >= YLIMIT + 4) {
                        erase(object[i]);
                        object[i].create = false;
                    }
                }
            }
        }
    }
}

bool Gameover(int score, int runtime) {
    textColor(YELLOW);
    system("cls");
    Sleep(1000);
    outline();
    gotoxy(35, 7);
    printf("[GameOver]");
    gotoxy(25, 13);
    printf("획득 점수 : %d점\n", score);
    gotoxy(25, 15);
    printf("생존 시간 : %d.%d초\n\n", runtime / 1000, runtime % 1000);
    Sleep(1000);
    gotoxy(20, 20);
    printf("다시 시작하려면 r, 종료하려면 e를 눌러주세요.");
    char command = 0;
    while (true) {
        command = getch();
        if ((command != 'r') && (command != 'e'))
            continue;
        else if (command == 'r')
            return true;
        else
            return false;
    }
}

void show(element object) {
    gotoxy(object.x, object.y);
    if (object.id == COIN)
    {
        textColor(YELLOW);
        printf("●");
    }
    else if (object.id == OBSTACLE)
    {
        textColor(DARKGRAY);
        printf("▼");
    }
    else if (object.id == PLAYER)
    {
        textColor(LIGHTCYAN);
        printf("★");
    }
    else;
}

void erase(element object) {
    gotoxy(object.x, object.y);
    printf("  ");
}

void outline() {
    int i = 0;
    gotoxy(10, 5);
    for (i = 0; i < XLIMIT; i++)
        printf("─");

    gotoxy(10, 5 + YLIMIT);
    for (i = 0; i < XLIMIT; i++)
        printf("─");

    for (i = 5; i < YLIMIT + 5; i++) {
        gotoxy(10, i);
        printf("│");
        gotoxy(10 + XLIMIT, i);
        printf("│");
    }
    gotoxy(10, 5); printf("┌");
    gotoxy(10 + XLIMIT, 5); printf("┐");
    gotoxy(10, 5 + YLIMIT); printf("└");
    gotoxy(10 + XLIMIT, 5 + YLIMIT); printf("┘");
}

void move_player(element* player) {
    char move = 0;
    if (kbhit()) {
        erase(*player);
        move = getch();
        switch (move) {
        case 72:// ↑
            if (player->y > 5 + 1)
                player->y--;
            break;
        case 75:// ←
            if (player->x > 10 + 2)
                player->x--;
            break;
        case 77:// →
            if (player->x < XLIMIT + 10 - 2)
                player->x++;
            break;
        case 80:// ↓
            if (player->y < YLIMIT + 5 - 1)
                player->y++;
            break;
        default:
            break;
        }
        show(*player);
    }
}

void textColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

element create_object(element object, int speed[2]) {
    object.create = true;
    object.id = rand() % 2 + 1; //코인or장애물 랜덤 생성.
    //물체의 x좌표 크기가 2이므로, x좌표값을 짝수로 만들어 겹치는것을 방지.
    object.x = (rand() % (XLIMIT - 3)) / 2 * 2 + 12, object.y = 5 + 1;
    object.period = rand() % 150 + 50; //물체가 만들어지는 딜레이 설정.
    object.speed = rand() % (speed[1] - speed[0] + 1) + speed[0]; //물체가 떨어지는 속도 설정.
    return object;
}