#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <bits.h>
using namespace std;
#define EASY 1
#define HARD 2
#define XLIMIT 60
#define YLIMIT 20
#define FRAME 5
#define UNIT 100
const int COIN = 1;
const int OBSTACLE = 2;
const int PLAYER = 3;

struct element {
    int x, y; //x,y��ǥ.
    int id; //��ü�� Ư��.(����/��ֹ�/ĳ����)
    int speed; //��ü�� �������� �ӵ� = FRAME * speed(ms).
    int period; //period == 0�� ��, object�� �� ĭ ������. (speed���� �ֱ��)
    bool create; //��ü�� ���� ����.
};

int start(); //���� ���� �� ���� ȭ��.
int running_game(int difficulty); //���� ���� ȭ��.
bool Gameover(int score, int runtime); //���â

void gotoxy(int x, int y);
void show(element object); //��ü ���
void erase(element object); //��ü �����
bool is_collide(element drop, element player); //ĳ���Ϳ� ��ü�� �浹 ����.
void outline(); //���� ȭ���� Ʋ �׸���.
//void textColor(int color); //�۾� �� ����.

int main() {
    while (true) {
        int difficulty = start(); //���̵��� ����ȭ�鿡�� ������.
        int period, score = 0;
        clock_t starttime, endtime;
        srand((unsigned)time(NULL));

        starttime = clock();
        score = running_game(difficulty);
        endtime = clock();

        int runtime = (int)endtime - (int)starttime;
        if (Gameover(score, runtime) == false)
            break;
    }
    gotoxy(10, 27);
    return 0;
}

void gotoxy(int x, int y) {
    COORD Pos = { x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

int start() {
    system("cls");
    //textColor(11);
    outline();
    gotoxy(30, 7);
    printf("[��ֹ� ���ϱ� ����]\n\n");
    gotoxy(35, 9);
    printf("���� ���\n");
    gotoxy(15, 11);
    printf("1) ����Ű�� ĳ���͸� ������ ��ֹ��� ���մϴ�.\n");
    gotoxy(15, 12);
    printf("2) ������ ������ 10���� ȹ���մϴ�.\n");
    gotoxy(15, 13);
    printf("3) �־��� �������� ��� �Ҹ��ϸ� ������ ����˴ϴ�.\n");
    gotoxy(20, 15);
    printf("���̵��� �����ϼ���(���� : 1, ����� : 2)");

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

    gotoxy(20, 17);
    printf("���̵� : %s�� �����ϼ̽��ϴ�.", (difficulty == EASY) ? "����" : "�����");
    Sleep(500);
    gotoxy(20, 19);
    printf("�ƹ�Ű�� �Է��Ͽ� ������ �����ϼ���!");
    getch();

    for (int i = 3; i > 0; i--) {
        system("cls");
        printf("%d�� �� ������ ���۵˴ϴ�...\n", i);
        Sleep(1000);
    }
    system("cls");
    return difficulty;
}

bool is_collide(element drop, element player) {
    //��ü�� x��ǥ ũ�Ⱑ 2�̹Ƿ�, x���� ������ ��.
    if (((player.x - 1 <= drop.x) && (player.x + 1 >= drop.x)) && (drop.y == player.y))
        return true;
    else
        return false;
}

int running_game(int difficulty) {
    element object[UNIT] = { {0} }, player = { 35, YLIMIT + 5 - 1,PLAYER };
    int i = 0, temp = 0;

    //probability : ��ü�� ������ Ȯ��.(1/probability), period[2] : ��ü�� �������� �ӵ��� �ִ�/�ּ� ����.
    int probability = 0, speed[2] = { 0,0 }, score = 0, life = 0;

    char move = 0; //ĳ���Ͱ� �����̴� ����.

    if (difficulty == EASY) {
        probability = 50; speed[0] = 30; speed[1] = 60; life = 3;
    }
    else if (difficulty == HARD) {
        probability = 10; speed[0] = 10; speed[1] = 20; life = 1;
    }
    else;

    outline();
    gotoxy(80, 5);
    printf("��  ���� : %4d ��", score);
    gotoxy(80, 7);
    printf("��  ������ : %d  ��", life);

    for (i = 0; i < UNIT; i++)
        object[i].create = false; //ó���� ��ü�� ���� �ȵ� ���·� �ʱ�ȭ.

    while (1) {
        //��ֹ� or ���� �Ѱ� ����.
        if (rand() % probability == 0) {
            for (i = 0; i < UNIT; i++) {
                if (object[i].create == false) {
                    object[i].create = true;
                    object[i].id = rand() % 2 + 1; //����or��ֹ� ���� ����.
                    //��ֹ��� x��ǥ ũ�Ⱑ 2�̹Ƿ�, x��ǥ���� ¦���� ����� ��ġ�°��� ����.
                    object[i].x = (rand() % (XLIMIT - 3)) / 2 * 2 + 12, object[i].y = 5 + 1;
                    object[i].period = rand() % 150 + 50; //��ֹ��� ��������� ������ ����.
                    object[i].speed = rand() % (speed[1] - speed[0] + 1) + speed[0];
                    break;
                }
            }
        }

        Sleep(FRAME);
        show(player);

        if (kbhit()) {
            erase(player);
            move = getch();
            switch (move) {
            case 72:// ��
                if (player.y > 5 + 1)
                    player.y--;
                break;
            case 75:// ��
                if (player.x > 10 + 2)
                    player.x--;
                break;
            case 77:// ��
                if (player.x < XLIMIT + 10 - 2)
                    player.x++;
                break;
            case 80:// ��
                if (player.y < YLIMIT + 5 - 1)
                    player.y++;
                break;
            default:
                break;
            }
            show(player);
        }

        for (i = 0; i < UNIT; i++) {
            if (object[i].create == true) {
                object[i].period--;
                if (object[i].period <= 0) {
                    object[i].period = object[i].speed; //�ٽ� ���� 1ĭ ������ ���ʸ� ��ٸ�.

                    erase(object[i]);
                    //1ĭ ������ ��, ĳ���Ϳ� �浹�ߴ��� Ȯ��.
                    object[i].y++;
                    show(object[i]);

                    if (is_collide(object[i], player)) {
                        erase(object[i]);
                        show(player);
                        object[i].create = false;
                        if (object[i].id == COIN) {
                            score += 10;
                            gotoxy(80, 5);
                            printf("��  ���� : %4d ��", score);
                        }
                        else if (object[i].id == OBSTACLE) {
                            life--;
                            gotoxy(80, 7);
                            printf("��  ������ : %d  ��", life);
                            if (life == 0) {
                                gotoxy(10, 27);
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
        printf("��");
    else if (object.id == OBSTACLE)
        printf("��");
    else if (object.id == PLAYER)
        printf("��");
    else;
}

void erase(element object) {
    gotoxy(object.x, object.y);
    printf("  ");
}

void outline()
{
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

/*void textColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}*/