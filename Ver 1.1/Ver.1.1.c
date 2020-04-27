/*
���� �ۼ��� : �̵��� (��õ���б� ������Ű��а�)
���� �ۼ��� : 2020.04.15
���� ������ : 2020.04.21
���� : ���� ������Ʈ
���� �̷� : �̵���, 2020.04.27(ver.1.1)

���� ���� : ver.1.1
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <stdbool.h>

#define EASY 1 //���̵�
#define HARD 2 //���̵�
#define XLIMIT 60 //���α��� : 60
#define YLIMIT 20 //���α��� : 20
#define FRAME 5 //���� ������
#define UNIT 100 //��ü �ִ� ����

//�۾� ���� ���,
#define DARKGRAY 8
#define LIGHTCYAN 11
#define YELLOW 14
#define LIGHTGREEN 10
#define LIGHTMAGENTA 13

//��ü�� �������� �����ϴ� ���.
const int COIN = 1;
const int OBSTACLE = 2;
const int PLAYER = 3;

typedef struct element {
    int x, y; //x,y��ǥ.
    int id; //��ü�� Ư��.(����/��ֹ�/ĳ����)
    int speed; //��ü�� �������� �ӵ� = FRAME * speed(ms).
    int period; //period == 0�� ��, object�� �� ĭ ������. (speed���� �ֱ��)
    bool create; //��ü�� ���� ����.
}element;

int start(); //���� ���� �� ���� ȭ��.
int running_game(int difficulty); //���� ���� ȭ��.
bool Gameover(int score, int runtime); //���â

void gotoxy(int x, int y); //Ŀ�� ��ġ �̵�.
void show(element object); //��ü ���
void erase(element object); //��ü �����
bool is_collide(element drop, element player); //ĳ���Ϳ� ��ü�� �浹 ����.
element create_object(element object, int speed[2]); //��ü ����.
void move_player(element* player); //ĳ������ ������ ����.
void outline(); //���� ȭ���� Ʋ �׸���.
void textColor(int color); //�۾� �� ����.

int main() {
    while (true) {
        int difficulty = start(); //���̵��� ����ȭ�鿡�� ������.
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
    gotoxy(10, 27); //���� �޽��� ��� ��ġ.
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
    printf("�ٿ�ε� : https://github.com/dongbin1999/Programming-Project");
    gotoxy(5, 2);
    printf("����ó(github) : https://dongbin1999.github.io/jekyll/update/Programming-project-Game-Demo/");
    gotoxy(5, 3);
    printf("����ó(email) : dongbin1999@inu.ac.kr");
    gotoxy(27, 7);
    printf("[��ֹ� ���ϱ� ���� Ver.1.1]\n\n");
    gotoxy(35, 11);
    textColor(LIGHTCYAN);
    printf("[���� ���]\n");
    gotoxy(15, 13);
    printf("1) ����Ű�� ĳ���͸� ������ ��ֹ��� ���մϴ�.\n");
    gotoxy(15, 14);
    printf("2) ������ ������ 10���� ȹ���մϴ�.\n");
    gotoxy(15, 15);
    printf("3) �־��� �������� ��� �Ҹ��ϸ� ������ ����˴ϴ�.\n");
    gotoxy(20, 17);
    textColor(YELLOW);
    printf("���̵��� �����ϼ��� (���� : 1, ����� : 2)"); }

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
    printf("���̵� : %s�� �����ϼ̽��ϴ�.", (difficulty == EASY) ? "����" : "�����");
    {Sleep(500);
    gotoxy(20, 21);
    printf("�ƹ�Ű�� �Է��Ͽ� ������ �����ϼ���!");
    getch();

    for (int i = 3; i > 0; i--) {
        system("cls");
        printf("%d�� �� ������ ���۵˴ϴ�...\n", i);
        Sleep(1000);
    }
    system("cls"); }
    return difficulty;
}

bool is_collide(element drop, element player) {
    //��ü�� x��ǥ ũ�Ⱑ 2�̹Ƿ�, x���� ������ ��.
    if (((player.x - 1 <= drop.x) && (player.x + 1 >= drop.x)) &&
        //y�൵ ���� ȹ�� ������ ���ֱ� ����, �Ʒ� ��ĭ���� ���.
        ((drop.y == player.y) || (drop.y - 1 == player.y)))
        return true;
    else
        return false;
}

int running_game(int difficulty) {
    element object[UNIT] = { {0} }, player = { 35, YLIMIT + 5 - 1,PLAYER };
    int i = 0;

    //probability : ��ü�� ������ Ȯ��.(1/probability), period[2] : ��ü�� �������� �ӵ��� �ִ�/�ּ� ����.
    int probability = 0, speed[2] = { 0,0 }, score = 0, life = 0;

    for (i = 0; i < UNIT; i++)
        object[i].create = false; //ó���� ��ü�� ���� �ȵ� ���·� �ʱ�ȭ.

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
    printf("��  ���� : %4d ��", score);
    gotoxy(80, 7);
    if (life == 1)
        textColor(LIGHTMAGENTA);
    printf("��  ������ : %d  ��", life);

    while (1) {
        //��ֹ� or ���� �Ѱ� ����.
        if (rand() % probability == 0)
            for (i = 0; i < UNIT; i++)
                if (object[i].create == false) {
                    object[i] = create_object(object[i], speed);
                    break;
                }

        Sleep(FRAME);
        show(player);

        //ĳ������ ������ ����. �ּҰ��� ���� ����.
        move_player(&player);

        //��ü�� ��������, �� �� ĳ���Ϳ� �浹�ߴ��� Ȯ���ϴ� �ݺ���.
        for (i = 0; i < UNIT; i++) {
            if (object[i].create == true) {
                object[i].period--;
                if (object[i].period <= 0) {
                    object[i].period = object[i].speed; //�ٽ� ���� 1ĭ ������ ���ʸ� ��ٸ�.

                    erase(object[i]);
                    object[i].y++;//1ĭ �Ʒ��� ������.
                    show(object[i]);

                    if (is_collide(object[i], player)) {
                        erase(object[i]);
                        show(player);
                        object[i].create = false;
                        if (object[i].id == COIN) {
                            score += 10;
                            gotoxy(80, 5);
                            textColor(YELLOW);
                            printf("��  ���� : %4d ��", score);
                        }
                        else if (object[i].id == OBSTACLE) {
                            life--;
                            gotoxy(80, 7);
                            if (life <= 1)
                                textColor(LIGHTMAGENTA);
                            else
                                textColor(YELLOW);
                            printf("��  ������ : %d  ��", life);
                            if (life == 0) {
                                gotoxy(10, 27);
                                textColor(YELLOW);
                                system("pause");
                                return score;
                            }
                        }
                        else;
                    }

                    //��ü�� �ٴڿ� ������ �����.
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
    printf("ȹ�� ���� : %d��\n", score);
    gotoxy(25, 15);
    printf("���� �ð� : %d.%d��\n\n", runtime / 1000, runtime % 1000);
    Sleep(1000);
    gotoxy(20, 20);
    printf("�ٽ� �����Ϸ��� r, �����Ϸ��� e�� �����ּ���.");
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
        printf("��");
    }
    else if (object.id == OBSTACLE)
    {
        textColor(DARKGRAY);
        printf("��");
    }
    else if (object.id == PLAYER)
    {
        textColor(LIGHTCYAN);
        printf("��");
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
        printf("��");

    gotoxy(10, 5 + YLIMIT);
    for (i = 0; i < XLIMIT; i++)
        printf("��");

    for (i = 5; i < YLIMIT + 5; i++) {
        gotoxy(10, i);
        printf("��");
        gotoxy(10 + XLIMIT, i);
        printf("��");
    }
    gotoxy(10, 5); printf("��");
    gotoxy(10 + XLIMIT, 5); printf("��");
    gotoxy(10, 5 + YLIMIT); printf("��");
    gotoxy(10 + XLIMIT, 5 + YLIMIT); printf("��");
}

void move_player(element* player) {
    char move = 0;
    if (kbhit()) {
        erase(*player);
        move = getch();
        switch (move) {
        case 72:// ��
            if (player->y > 5 + 1)
                player->y--;
            break;
        case 75:// ��
            if (player->x > 10 + 2)
                player->x--;
            break;
        case 77:// ��
            if (player->x < XLIMIT + 10 - 2)
                player->x++;
            break;
        case 80:// ��
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
    object.id = rand() % 2 + 1; //����or��ֹ� ���� ����.
    //��ü�� x��ǥ ũ�Ⱑ 2�̹Ƿ�, x��ǥ���� ¦���� ����� ��ġ�°��� ����.
    object.x = (rand() % (XLIMIT - 3)) / 2 * 2 + 12, object.y = 5 + 1;
    object.period = rand() % 150 + 50; //��ü�� ��������� ������ ����.
    object.speed = rand() % (speed[1] - speed[0] + 1) + speed[0]; //��ü�� �������� �ӵ� ����.
    return object;
}