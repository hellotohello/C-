#include <conio.h>  //用于引入 _getch() 函数
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>  // 用于引入 Sleep 函数
#define BAR_LENGTH 20
#define MAXPLAYER 200                         //玩家初始生命
#define MAXAI 500                             // AI初始生命
#define RED "\x1b[31m"                        //烧伤
#define GREEN "\x1b[32m"                      //寄生
#define RESET "\x1b[0m"                       //白色
#define GOLD "\x1b[38;5;214m"                 //胜利
#define GRAY "\x1b[90m"                       //睡眠
#define DARK_RED "\x1b[38;2;255;0;0m"         //血条
#define BLOOD_RED "\x1b[91m"                  //攻击相关
#define LIGHT_BLUE "\x1b[94m"                 //能量
#define YELLOW_BROWN "\x1b[38;2;184;115;51m"  //防御
#define LIGHT_GREEN "\x1b[92m"                //回血、净化
#define BEIGE "\x1b[37m"                      //旁白
#define ZHEN          \
    Sleep(s100s);     \
    printf("\x1b[A"); \
    printf("\r\x1b[K");

typedef struct {
    char name[10];
    int health;
    int energy;
    int attack;
    int defending;  // 防御标记
} Character;

typedef struct {
    int burn;
    int parasite;
    int cleanse;
    int sleep;
} Status;

int s500s = 500;  //基础时间片，加速功能使用
int s100s = 100;  //动画时间片，动画加速用

int skilTemp = -1;  //记录技能使用位置
int skilFlag = 0;

void printGameRules();
void printGameState(Character player, Character ai, Status playerStatus, Status aiStatus);
void applyStatus(Character* character, Status* status, Character* character_);
int getRandomNumber(int min, int max);
void printRoundSummary(int round, Status playerStatus, Status aiStatus, Character player, Character ai);
void printHealthBar(Character p, int max);
void printEnergyBar(int current);
void turn(Character* p1, Character* p2, Status* Status1, Status* Status2);
//智能ai
int AI(Character p1, Character p2, Status Status1, Status Status2);
//方便操作
int optimizeOperations();

//攻击
void attack1(Character p1, Character p2, int healthValue, int energyValue, int attackValue);
void attack2(Character p1, Character p2, int healthValue, int energyValue, int attackValue);
//防御
void defense(Character p1, Character p2, int healthValue, int energyValue, int attackValue);
//恢复
void heal(Character p1, Character p2, int healthValue, int energyValue, int attackValue);
// 充能
void charge(Character p1, Character p2, int healthValue, int energyValue, int attackValue);
// 强化
void strengthen(Character p1, Character p2, int healthValue, int energyValue, int attackValue);
// 蓄力一击
void chargedAttack1(Character p1, Character p2, int healthValue, int energyValue, int attackValue);
void chargedAttack2(Character p1, Character p2, int healthValue, int energyValue, int attackValue);

int main() {
    srand(time(NULL));

    printGameRules();

    getchar();  // 暂停等待用户按 Enter 键

    Character player = {"你", MAXPLAYER, 20, 0, 0};
    Character ai = {"AI", MAXAI, 20, 0, 0};

    Status playerStatus = {0, 0, 0, 0};
    Status aiStatus = {0, 0, 0, 0};

    int round = 1;
    int playAgain = 1;     // 初始化为1，表示默认继续游戏
    int playerChoice = 0;  // 将 playerChoice 移到循环外部定义

    while (playAgain) {
        while (player.health > 0 && ai.health > 0) {
            system("cls");  // 清空控制台

            printGameState(player, ai, playerStatus, aiStatus);

            printf("第 %d 回合开始:\n", round);

            // playerturn
            applyStatus(&player, &playerStatus, &ai);
            if (player.health <= 0 || ai.health <= 0)
                break;
            turn(&player, &ai, &playerStatus, &aiStatus);
            if (player.health <= 0 || ai.health <= 0)
                break;

            printf("\n");

            // aiturn
            applyStatus(&ai, &aiStatus, &player);
            if (player.health <= 0 && ai.health <= 0)
                break;
            turn(&ai, &player, &aiStatus, &playerStatus);
            if (player.health <= 0 && ai.health <= 0)
                break;
            // 打印本回合结束时状态
            Sleep(s500s * 2);
            player.energy += 6;
            printf(LIGHT_BLUE "\n你 回复了6点能量\n");
            Sleep(s500s * 2);
            printf("\x1b[A");
            printf("\r\x1b[K");
            ai.energy += 6;
            printf("AI 回复了6点能量\n" BEIGE);
            Sleep(s500s * 2);
            printf("\x1b[A");
            printf("\r\x1b[K");
            printRoundSummary(round, playerStatus, aiStatus, player, ai);
            round++;
        }

        system("cls");  // 清空控制台

        printGameState(player, ai, playerStatus, aiStatus);

        if (player.health <= 0) {
            printf(GOLD "\t\t★★AI获胜！★★\n" BEIGE);
        } else {
            printf(GOLD "\t\t★★玩家获胜！★★\n" BEIGE);
        }

        printf(">>>是否想要再次进行游戏？ (1: 是, 0: 否): ");
        scanf("%d", &playAgain);

        if (playAgain) {
            // 重新初始化游戏状态
            player = (Character){"你", MAXPLAYER, 20, 0, 0};
            ai = (Character){"AI", MAXAI, 20, 0, 0};
            playerStatus = (Status){0, 0, 0, 0};
            aiStatus = (Status){0, 0, 0, 0};
            round = 1;
        }
    }

    printf("感谢游玩！\n");
    Sleep(s500s);

    return 0;
}

void printEnergyBar(int current) {
    int t = current / 2.5;
    printf("[");
    for (int i = 0; i < t; ++i)
        printf(LIGHT_BLUE "*");
    printf(BEIGE "] " LIGHT_BLUE "%d" BEIGE "\n", current);
}
void printHealthBar(Character p, int max) {
    printf("[" DARK_RED);
    for (int i = 0; i < 20; ++i) {
        if (i < (p.health + max / 40) * 20 / max) {  //加个小值，用来使显示血量四舍五入
            printf("#");
        } else {
            printf("-");
        }
    }

    //生命溢出的显示
    if (p.health + max / 40 > max && p.health > max) {
        printf(BEIGE "|" DARK_RED);
        int temp = p.health - max;
        for (int i = 0; i < 20; ++i) {
            if (i < (temp + max / 40) * 20 / max) {  //加个小值，用来使显示血量四舍五入
                printf("#");
            } else {
                break;
            }
        }
    }

    if (p.defending == 1)
        printf(BEIGE "]" YELLOW_BROWN "防御生效中… " DARK_RED "%d/%d" BEIGE "\n", p.health, max);
    else

        printf(BEIGE "] " DARK_RED "%d/%d" BEIGE "\n", p.health, max);
}

void printGameRules() {
    printf(BEIGE "欢迎来到战斗游戏！\n");
    printf("游戏规则：\n");
    printf("\t1. 角色初始生命值为" DARK_RED "%d" BEIGE "(AI有" DARK_RED "%d" BEIGE ")，初始能量值为" LIGHT_BLUE "20" BEIGE "，攻击力为" BLOOD_RED "0" BEIGE "。\n", MAXPLAYER, MAXAI);
    printf("\t2. 每回合开始时，双方自动回复" LIGHT_BLUE "6" BEIGE "点能量。\n");
    printf("\t3. 技能列表：\n");
    printf(BLOOD_RED "\t   1. 攻击（消耗10点能量）：对对方造成（攻击力+10）点伤害。\n" BEIGE);
    printf(YELLOW_BROWN "\t   2. 防御（消耗7点能量）：减少下一次攻击技能90%%的伤害。\n" BEIGE);
    printf(LIGHT_GREEN "\t   3. 回复（消耗8点能量）：恢复自身70点生命值。\n" BEIGE);
    printf(LIGHT_BLUE "\t   4. 充能（消耗3点能量）：获得18点能量。\n" BEIGE);
    printf(BLOOD_RED "\t   5. 强化（消耗11点能量、5点生命值）：永久提高15点攻击力。\n" BEIGE);
    printf(BLOOD_RED "\t   5. 蓄力一击（消耗80点能量）：造成(2.5倍攻击力+30)点伤害。\n" BEIGE);
    printf("\t4. 状态效果：\n");
    printf(RED "\t   - 灼烧（Burn）：每回合生命值减少5点。\n" BEIGE);
    printf(GREEN "\t   - 寄生（Parasite）：每回合吸取对方生命值5点，同时回复自身5点生命值。\n" BEIGE);
    printf(LIGHT_GREEN "\t   - 净化（Cleanse）：解除一种异常状态。\n" BEIGE);
    printf(GRAY "\t   - 催眠（Sleep）：回合内无法行动。\n" BEIGE);
    printf("\t5. 每次角色受到伤害时有50%%的概率获得一种状态效果，状态持续回合数由2-4的随机数生成，重复获得同一状态则叠加回合数。\n");
    printf("\t6. 游戏结束条件：玩家或AI的生命值降至0以下。\n");
    printf("准备好了吗？让我们开始游戏吧！\n");
    printf("----------------------------------------------------------------\n");
    printf("按 Enter 键继续...(全屏状态游戏体验更佳！)");
}

void printGameState(Character player, Character ai, Status playerStatus, Status aiStatus) {
    printf("玩家状态：\n");
    printf("\t生 命 值：");
    printHealthBar(player, MAXPLAYER);
    printf("\t 能  量 ：");
    printEnergyBar(player.energy);
    printf("\t攻 击 力：" BLOOD_RED "%d\n" BEIGE, player.attack);
    printf("\t状态效果：" RED "灼烧：%d" BEIGE "  " GREEN "寄生：%d" BEIGE "  " GRAY "催眠：%d" BEIGE "\n", playerStatus.burn, playerStatus.parasite, playerStatus.sleep);

    printf("\nAI状态：\n");
    printf("\t生 命 值：");
    printHealthBar(ai, MAXAI);
    printf("\t 能  量 ：");
    printEnergyBar(ai.energy);
    printf("\t攻 击 力：" BLOOD_RED "%d\n" BEIGE, ai.attack);
    printf("\t状态效果：" RED "灼烧：%d" BEIGE "  " GREEN "寄生：%d" BEIGE "  " GRAY "催眠：%d" BEIGE "\n", aiStatus.burn, aiStatus.parasite, aiStatus.sleep);

    printf("----------------------------------------------------------------\n");
}

void applyStatus(Character* character, Status* status, Character* character_) {
    if (status->cleanse) {
        int nonZeroStatusCount = 0;
        int nonZeroStatusIndices[3];  // 最多有三种状态

        if (status->burn != 0) {
            nonZeroStatusIndices[nonZeroStatusCount++] = 1;
        }

        if (status->parasite != 0) {
            nonZeroStatusIndices[nonZeroStatusCount++] = 2;
        }

        if (status->sleep != 0) {
            nonZeroStatusIndices[nonZeroStatusCount++] = 3;
        }

        if (nonZeroStatusCount > 0) {
            // 随机选择一种非零状态进行净化
            int randomIndex = getRandomNumber(0, nonZeroStatusCount - 1);
            int selectedStatus = nonZeroStatusIndices[randomIndex];

            // 根据选择净化相应状态
            switch (selectedStatus) {
                case 1:
                    status->burn = 0;
                    printf(LIGHT_GREEN "%s的[灼烧]状态消失了。\n" BEIGE, character->name);
                    Sleep(s500s * 4);
                    printf("\x1b[A");
                    printf("\r\x1b[K");
                    break;
                case 2:
                    status->parasite = 0;
                    printf(LIGHT_GREEN "%s的[寄生]状态消失了。\n" BEIGE, character->name);
                    Sleep(s500s * 4);
                    printf("\x1b[A");
                    printf("\r\x1b[K");
                    break;
                case 3:
                    status->sleep = 0;
                    printf(LIGHT_GREEN "%s苏醒了。\n" BEIGE, character->name);
                    Sleep(s500s * 4);
                    printf("\x1b[A");
                    printf("\r\x1b[K");
                    break;
                default:
                    // 处理未知状态
                    break;
            }
        }

        status->cleanse--;
    }
    if (status->burn) {
        character->health -= 5;
        status->burn--;
        printf(RED "%s受到[烧伤]伤害，生命-5。" BEIGE "\n", character->name);
        Sleep(s500s * 4);
        printf("\x1b[A");
        printf("\r\x1b[K");
    }
    if (status->parasite) {
        character->health -= 5;
        character_->health += 5;
        status->parasite--;
        printf(GREEN "%s受到[寄生]伤害，生命被吸取5。" BEIGE "\n", character->name);
        Sleep(s500s * 4);
        printf("\x1b[A");
        printf("\r\x1b[K");
    }
    if (status->sleep) {
        if (status->sleep == 1) {
            printf(LIGHT_GREEN "%s苏醒了\n" BEIGE, character->name);
            Sleep(s500s * 4);
            printf("\x1b[A");
            printf("\r\x1b[K");
        }
        status->sleep--;
    }
}

int getRandomNumber(int min, int max) {
    return rand() % (max - min + 1) + min;
}

void printRoundSummary(int round, Status playerStatus, Status aiStatus, Character player, Character ai) {
    printf("第 %d 回合结束。\n", round);
    printf("----------------------------------------------------------------\n");
    Sleep(s500s * 2);
    printGameState(player, ai, playerStatus, aiStatus);

    // 等待用户按 Enter 键继续
    printf("按 Enter 键继续...");
    getchar();
}

void turn(Character* p1, Character* p2, Status* Status1, Status* Status2) {
    int p1Choice = 0;

    if (Status1->sleep) {
        Sleep(s500s);
        printf(GRAY ">>>%s在睡眠中，无法操作。\n" BEIGE, p1->name);
    } else {
        int flag = 0;
        do {
            if (strcmp(p1->name, "你")) {
                if (!flag) {
                    printf(">>>AI操作中:\n");
                    flag++;
                } else
                    ;
                // AI智能攻击模块
                p1Choice = 3;
                AI(*p1, *p2, *Status1, *Status2);
            } else {
                p1Choice = optimizeOperations();
            }
            // 添加检查，确保玩家有足够的能量来选择任何技能
            if ((p1Choice == 1 && p1->energy >= 10) ||
                (p1Choice == 2 && p1->energy >= 7) ||
                (p1Choice == 3 && p1->energy >= 5) ||
                (p1Choice == 4 && p1->energy >= 3) ||
                (p1Choice == 5 && p1->energy >= 11) ||
                (p1Choice == 6 && p1->energy >= 80)) {
                break;  // 能量足够，跳出循环
            } else if (p1->energy < 3) {
                printf(LIGHT_BLUE "%s能量过低，无法操作。\n" BEIGE, p1->name);
                return;
            } else {
                if (strcmp(p1->name, "你"))
                    ;
                else {
                    printf(LIGHT_BLUE "能量不足，请重新选择技能。\n" BEIGE);
                    Sleep(s500s);
                    int i = 3;  //这里要回退3行
                    while (i > 0) {
                        printf("\x1b[A");
                        printf("\r\x1b[K");
                        --i;
                    }
                }
            }
        } while (1);  // 无限循环直到玩家选择了有效的技能

        Sleep(s500s);
        int healthValue = 0, energyValue = 0, attackValue = 0;
        switch (p1Choice) {
            case 1:
                if (p2->defending != 0) {
                    healthValue = -1 * (p1->attack + 10) * 0.1;
                    energyValue = -10;
                    attackValue = 0;
                    attack2(*p1, *p2, healthValue, energyValue, attackValue);
                    p2->health += healthValue;
                    p1->energy += energyValue;
                    p2->defending = 0;
                    printf(BLOOD_RED "%s攻击了%s", p1->name, p2->name);
                    Sleep(s500s);
                    printf("，" YELLOW_BROWN "被%s防御了。\n" BEIGE, p2->name);

                } else {
                    healthValue = -1 * (p1->attack + 10);
                    energyValue = -10;
                    attackValue = 0;
                    attack1(*p1, *p2, healthValue, energyValue, attackValue);
                    printf(BLOOD_RED "%s攻击了%s", p1->name, p2->name);
                    p2->health += healthValue;
                    p1->energy += energyValue;
                    printf("。\n" BEIGE);
                }
                Sleep(s500s);

                // 触发状态效果的概率，这里设置为50%
                if (getRandomNumber(1, 100) <= 50) {
                    // 随机选择一个状态效果触发
                    int randomEffect = getRandomNumber(1, 4);  // 有四种状态效果

                    switch (randomEffect) {
                        case 1:
                            Status2->burn += getRandomNumber(2, 4);
                            printf("%s的攻击使%s受到" RED "[灼烧]" BEIGE "效果，持续 " RED "%d" BEIGE " 回合！\n", p1->name, p2->name, Status2->burn);
                            break;
                        case 2:
                            Status2->parasite += getRandomNumber(2, 4);
                            printf("%s的攻击使%s受到" GREEN "[寄生]" BEIGE "效果，持续 " GREEN "%d" BEIGE " 回合！\n", p1->name, p2->name, Status2->parasite);
                            break;
                        case 3:
                            Status2->cleanse = 1;
                            printf("%s的攻击意外使%s受到" LIGHT_GREEN "[净化]" BEIGE "效果，立即解除%s一种异常状态！\n", p1->name, p2->name, p2->name);
                            break;
                        case 4:
                            Status2->sleep += getRandomNumber(2, 3);
                            printf("%s的攻击使%s受到" GRAY "[催眠]" BEIGE "效果，持续 " GRAY "%d" GRAY " 回合！\n", p1->name, p2->name, Status2->sleep);
                            break;
                        // 添加其他状态的处理
                        default:
                            printf("未知状态效果！\n");
                    }
                } else
                    printf("普通的一击，无状态效果。\n");
                break;
            case 2:
                healthValue = 0;
                energyValue = -7;
                attackValue = 0;
                defense(*p1, *p2, healthValue, energyValue, attackValue);
                p1->defending = 1;  // 设置防御标记
                p1->energy += energyValue;
                printf(YELLOW_BROWN "%s使用了防御，", p1->name);
                Sleep(s500s);
                printf("下一次受到攻击伤害时，免伤90%%！\n" BEIGE);
                break;
            case 3:
                if (strcmp(p1->name, "你"))
                    healthValue = 0.3 * MAXAI;
                else
                    healthValue = 0.3 * MAXPLAYER;
                energyValue = -5;
                attackValue = 0;
                heal(*p1, *p2, healthValue, energyValue, attackValue);
                p1->health += healthValue;
                p1->energy += energyValue;
                printf(LIGHT_GREEN "%s使用了回复，", p1->name);
                Sleep(s500s);
                printf("回复了部分生命值！\n" BEIGE);
                break;
            case 4:
                healthValue = 18;
                energyValue = -3;
                attackValue = 0;
                charge(*p1, *p2, healthValue, energyValue, attackValue);
                p1->energy += energyValue;
                p1->energy += healthValue;
                printf(LIGHT_BLUE "%s使用了充能，", p1->name);
                Sleep(s500s);
                printf("能量提高了！\n" BEIGE);
                break;
            case 5:
                healthValue = -5;
                energyValue = -11;
                attackValue = 15;
                strengthen(*p1, *p2, healthValue, energyValue, attackValue);
                p1->attack += attackValue;
                p1->health += healthValue;
                p1->energy += energyValue;
                printf(BLOOD_RED "%s使用了强化，", p1->name);
                Sleep(s500s);
                printf("攻击提高了！\n" BEIGE);
                break;
            case 6:
                if (p2->defending != 0) {
                    healthValue = -1 * (p1->attack * 2.5 + 30) * 0.1;
                    energyValue = -80;
                    attackValue = 0;
                    chargedAttack2(*p1, *p2, healthValue, energyValue, attackValue);
                    p2->health += healthValue;
                    p1->energy += energyValue;
                    p2->defending = 0;
                    printf(BLOOD_RED "%s对%s使用了蓄力一击" BEIGE, p1->name, p2->name);
                    Sleep(s500s);
                    printf(BEIGE "，" YELLOW_BROWN "被%s防御了\n" BEIGE, p2->name);
                } else {
                    healthValue = -1 * (p1->attack * 2.5 + 30);
                    energyValue = -80;
                    attackValue = 0;
                    chargedAttack1(*p1, *p2, healthValue, energyValue, attackValue);
                    p2->health += healthValue;
                    p1->energy += energyValue;
                    printf(BLOOD_RED "%s对%s使用了蓄力一击" BEIGE, p1->name, p2->name);
                    Sleep(s500s);
                    printf(BLOOD_RED "，造成了高额伤害！\n" BEIGE);
                }
                break;
            default:
                printf("无效的选择！\n");
        }
    }
}

int AI(Character ai, Character player, Status StatusAi, Status StatusPlayer) {
    // 设置每个技能的基础概率
    double baseProbabilities[] = {0.2, 0.2, 0.2, 0.2, 0.2, 0.2};
    baseProbabilities[0] += 0.05;  //上调攻击的使用概率

    // 根据条件调整技能选择概率
    if (player.health <= MAXPLAYER * 0.5 && player.health >= MAXPLAYER * 0.25) {
        // 在player血量处于25%-50%时，增加6、4技能的使用概率
        baseProbabilities[5] += 0.15;
        baseProbabilities[3] += 0.15;
    }

    if (ai.energy >= 80) {
        // 在ai能量达到80时，增加6技能的使用概率
        baseProbabilities[5] += 0.3;
    }

    if (player.defending == 1) {
        // player存在护盾时，不使用蓄力一击，增加使用攻击的概率
        baseProbabilities[5] -= 0.2;
        baseProbabilities[5] += 0.2;
    }

    if (player.energy > 70 && ai.defending == 0) {
        // 当player能量高于70时，增加2技能的使用概率
        baseProbabilities[1] += 0.1;
    }

    if (ai.defending == 1) {
        // 当ai存在护盾时不再使用护盾
        baseProbabilities[1] = 0;
    }

    if (ai.health <= MAXAI * 0.3) {
        // 当ai自身血量低于30%时，增加3技能的使用概率
        baseProbabilities[2] += 0.2;
    }

    if (ai.energy <= 15) {
        // 当ai自身能量低于15时，增加4技能的使用概率
        baseProbabilities[3] += 0.15;
    }

    // 归一化概率，确保总和为1
    double totalProbability = 0.0;
    for (int i = 0; i < 6; ++i) {
        totalProbability += baseProbabilities[i];
    }
    for (int i = 0; i < 6; ++i) {
        baseProbabilities[i] /= totalProbability;
    }

    // 根据概率随机选择技能
    double randomValue = (double)rand() / RAND_MAX;
    double cumulativeProbability = 0.0;
    int aiChoice = 0;

    for (int i = 0; i < 6; ++i) {
        cumulativeProbability += baseProbabilities[i];
        if (randomValue <= cumulativeProbability) {
            aiChoice = i + 1;  // 技能编号从1开始
            break;
        }
    }

    return aiChoice;
}

int optimizeOperations() {
    printf(">>>请玩家操作(使用“↑”“↓”“←”“→”选择技能，按Enter确认；使用空格开/关加速):\n");
    if (skilFlag == 0)
        printf("\\\\" BLOOD_RED " 1攻击  " YELLOW_BROWN "2防御  " LIGHT_GREEN "3回复  " LIGHT_BLUE "4充能  " BLOOD_RED "5强化  " BLOOD_RED "6蓄力一击 " BEIGE "//\n\n");
    else {
        switch (((skilTemp % 6) + 6) % 6) {
            case 0:
                printf("\\\\[" BLOOD_RED "1攻击" BEIGE "] " YELLOW_BROWN "2防御  " LIGHT_GREEN "3回复  " LIGHT_BLUE "4充能  " BLOOD_RED "5强化  " BLOOD_RED "6蓄力一击 " BEIGE "//\n");
                printf("\t造成一定伤害，50%%概率使对方触发某种状态。\n");
                break;
            case 1:
                printf("\\\\" BLOOD_RED " 1攻击 " BEIGE "[" YELLOW_BROWN "2防御" BEIGE "] " LIGHT_GREEN "3回复  " LIGHT_BLUE "4充能  " BLOOD_RED "5强化  " BLOOD_RED "6蓄力一击 " BEIGE "//\n");
                printf("\t制造一个抵御伤害的护盾。\n");
                break;
            case 2:
                printf("\\\\" BLOOD_RED " 1攻击  " YELLOW_BROWN "2防御 " BEIGE "[" LIGHT_GREEN "3回复" BEIGE "] " LIGHT_BLUE "4充能  " BLOOD_RED "5强化  " BLOOD_RED "6蓄力一击 " BEIGE "//\n");
                printf("\t恢复部分生命。\n");
                break;
            case 3:
                printf("\\\\" BLOOD_RED " 1攻击  " YELLOW_BROWN "2防御  " LIGHT_GREEN "3回复 " BEIGE "[" LIGHT_BLUE "4充能" BEIGE "] " BLOOD_RED "5强化  " BLOOD_RED "6蓄力一击 " BEIGE "//\n");
                printf("\t提高能量。\n");
                break;
            case 4:
                printf("\\\\" BLOOD_RED " 1攻击  " YELLOW_BROWN "2防御  " LIGHT_GREEN "3回复  " LIGHT_BLUE "4充能 " BEIGE "[" BLOOD_RED "5强化" BEIGE "] " BLOOD_RED "6蓄力一击 " BEIGE "//\n");
                printf("\t损失些许生命，提高攻击力。\n");
                break;
            case 5:
                printf("\\\\" BLOOD_RED " 1攻击  " YELLOW_BROWN "2防御  " LIGHT_GREEN "3回复  " LIGHT_BLUE "4充能  " BLOOD_RED "5强化 " BEIGE "[" BLOOD_RED "6蓄力一击" BEIGE "]" BEIGE "//\n");
                printf("\t耗费巨大能量，造成高额伤害。\n");
                break;
        }
    }

    while (1) {
        int flag = 0;

        char ch = _getch();                                // 使用 _getch 函数捕捉键盘输入
        if (ch == 72 || ch == 75 || ch == 77 || ch == 80)  //优化键盘输入错误
            continue;
        if (ch == 13 && skilFlag != 0) {  //函数出口，需要判断是否选择过技能

            //回退一行，删除技能介绍
            printf("\x1b[A");
            printf("\r\x1b[K");

            return ((skilTemp % 6) + 6) % 6 + 1;
        } else if (ch == 32) {  //使用空格后进入此处
            if (s500s == 500) {
                s500s = 250;
                s100s = 75;
                printf("开启加速。\n");
                Sleep(s500s);
                printf("\x1b[A");
                printf("\r\x1b[K");
            } else {
                s500s = 500;
                s100s = 100;
                printf("关闭加速。\n");
                Sleep(s500s);
                printf("\x1b[A");
                printf("\r\x1b[K");
            }
            continue;
        }
        char ch2 = _getch();  // 使用 _getch 函数捕捉键盘输入
        switch (ch2) {
            case 72:  // 上箭头
                --skilTemp;
                break;
            case 80:  // 下箭头
                ++skilTemp;
                break;
            case 75:  // 左箭头
                --skilTemp;
                break;
            case 77:  // 右箭头
                ++skilTemp;
                break;
            default:
                printf("按键错误。\n", ch);
                flag = 1;
                Sleep(100);
                printf("\x1b[A");
                printf("\r\x1b[K");
                break;
        }

        //使用flag标记按键错误，错误后直接跳出此次循环
        if (flag == 1) {
            continue;
        }
        switch (((skilTemp % 6) + 6) % 6) {
            case 0:
                skilFlag = 1;
                printf("\x1b[A");
                printf("\r\x1b[K");
                printf("\x1b[A");
                printf("\r\x1b[K");
                printf("\\\\[" BLOOD_RED "1攻击" BEIGE "] " YELLOW_BROWN "2防御  " LIGHT_GREEN "3回复  " LIGHT_BLUE "4充能  " BLOOD_RED "5强化  " BLOOD_RED "6蓄力一击 " BEIGE "//\n");
                printf("\t造成一定伤害，50%%概率使对方触发某种状态。\n");
                break;
            case 1:
                skilFlag = 1;
                printf("\x1b[A");
                printf("\r\x1b[K");
                printf("\x1b[A");
                printf("\r\x1b[K");
                printf("\\\\" BLOOD_RED " 1攻击 " BEIGE "[" YELLOW_BROWN "2防御" BEIGE "] " LIGHT_GREEN "3回复  " LIGHT_BLUE "4充能  " BLOOD_RED "5强化  " BLOOD_RED "6蓄力一击 " BEIGE "//\n");
                printf("\t制造一个抵御伤害的护盾。\n");
                break;
            case 2:
                skilFlag = 1;
                printf("\x1b[A");
                printf("\r\x1b[K");
                printf("\x1b[A");
                printf("\r\x1b[K");
                printf("\\\\" BLOOD_RED " 1攻击  " YELLOW_BROWN "2防御 " BEIGE "[" LIGHT_GREEN "3回复" BEIGE "] " LIGHT_BLUE "4充能  " BLOOD_RED "5强化  " BLOOD_RED "6蓄力一击 " BEIGE "//\n");
                printf("\t恢复部分生命。\n");
                break;
            case 3:
                skilFlag = 1;
                printf("\x1b[A");
                printf("\r\x1b[K");
                printf("\x1b[A");
                printf("\r\x1b[K");
                printf("\\\\" BLOOD_RED " 1攻击  " YELLOW_BROWN "2防御  " LIGHT_GREEN "3回复 " BEIGE "[" LIGHT_BLUE "4充能" BEIGE "] " BLOOD_RED "5强化  " BLOOD_RED "6蓄力一击 " BEIGE "//\n");
                printf("\t提高能量。\n");
                break;
            case 4:
                skilFlag = 1;
                printf("\x1b[A");
                printf("\r\x1b[K");
                printf("\x1b[A");
                printf("\r\x1b[K");
                printf("\\\\" BLOOD_RED " 1攻击  " YELLOW_BROWN "2防御  " LIGHT_GREEN "3回复  " LIGHT_BLUE "4充能 " BEIGE "[" BLOOD_RED "5强化" BEIGE "] " BLOOD_RED "6蓄力一击 " BEIGE "//\n");
                printf("\t损失些许生命，提高攻击力。\n");
                break;
            case 5:
                skilFlag = 1;
                printf("\x1b[A");
                printf("\r\x1b[K");
                printf("\x1b[A");
                printf("\r\x1b[K");
                printf("\\\\" BLOOD_RED " 1攻击  " YELLOW_BROWN "2防御  " LIGHT_GREEN "3回复  " LIGHT_BLUE "4充能  " BLOOD_RED "5强化 " BEIGE "[" BLOOD_RED "6蓄力一击" BEIGE "]" BEIGE "//\n");
                printf("\t耗费巨大能量，造成高额伤害。\n");
                break;
            default:
                printf("\t错误。\n");
                break;
        }
    }
}

void attack1(Character p1, Character p2, int healthValue, int energyValue, int attackValue) {
    printf("  %s       \n", p1.name);
    ZHEN
        printf("  %s       \n", p1.name);
    ZHEN
        printf("  %s       \n", p1.name);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED " >      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "->      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "-->     " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED " -->    " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "  -->   " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "   -->  " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "    --> " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "     -->" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "      --%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "       --" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "        -%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "         -" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s          " BLOOD_RED "%s>" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s          %s" BLOOD_RED "->" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s        " BLOOD_RED "%4d-->" BEIGE "\n", p1.name, healthValue);
    ZHEN
        printf(" %s        " BLOOD_RED "%4d -->" BEIGE "\n", p1.name, healthValue);
    ZHEN
        printf(" %s        " BLOOD_RED "%4d  --" BEIGE "\n", p1.name, healthValue);
    ZHEN
        printf(" %s        " BLOOD_RED "%4d   -" BEIGE "\n", p1.name, healthValue);
    ZHEN
        printf(" %s          %s    \n", p1.name, p2.name);
    ZHEN
        printf(" %s          %s    \n", p1.name, p2.name);
    ZHEN
}
void attack2(Character p1, Character p2, int healthValue, int energyValue, int attackValue) {
    printf("  %s       \n", p1.name);
    ZHEN
        printf("  %s       \n", p1.name);
    ZHEN
        printf("  %s       \n", p1.name);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf("  %s      [%s]\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "]" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "]" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "|" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "]" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "|" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "|" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN ":" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "|" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN ":" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN ":" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·    " YELLOW_BROWN "[[" BEIGE "%s" YELLOW_BROWN "·" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN ":" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·    " YELLOW_BROWN "[[" BEIGE "%s" YELLOW_BROWN "·" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "·" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●    " YELLOW_BROWN "[[" BEIGE "%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·    " YELLOW_BROWN "[ " BEIGE "%s" YELLOW_BROWN "·" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·    " YELLOW_BROWN "[[" BEIGE "%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·     " YELLOW_BROWN "[" BEIGE "%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED " >     " YELLOW_BROWN "[" BEIGE "%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "->     " YELLOW_BROWN "[" BEIGE "%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "-->    " YELLOW_BROWN "[" BEIGE "%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED " -->   " YELLOW_BROWN "[" BEIGE "%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "  -->  " YELLOW_BROWN "[" BEIGE "%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "   --> " YELLOW_BROWN "[" BEIGE "%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "    -->" YELLOW_BROWN "[" BEIGE "%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "     --" YELLOW_BROWN "[" BEIGE "%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "      --" YELLOW_BROWN "[" BEIGE "%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "       -" YELLOW_BROWN "[" BEIGE "%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "        -%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s         %s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "         %s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s         %s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "         %s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s         %s" BLOOD_RED ">" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "         %s>" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s       " BLOOD_RED "%4d->" BEIGE "\n", p1.name, healthValue);
    ZHEN
        printf(" %s" BLOOD_RED "       %4d->" BEIGE "\n", p1.name, healthValue);
    ZHEN
        printf(" %s       " BLOOD_RED "%4d ->" BEIGE "\n", p1.name, healthValue);
    ZHEN
        printf(" %s" BLOOD_RED "       %4d ->" BEIGE "\n", p1.name, healthValue);
    ZHEN
        printf(" %s         %s" BLOOD_RED "  -" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s         %s" BLOOD_RED "  -" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s         %s   \n", p1.name, p2.name);
    ZHEN
}
void defense(Character p1, Character p2, int healthValue, int energyValue, int attackValue) {
    printf("  %s       \n", p1.name);
    ZHEN
        printf("  %s       \n", p1.name);
    ZHEN
        printf("  %s       \n", p1.name);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "·" BEIGE "%s" YELLOW_BROWN "·      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "·" BEIGE "%s" YELLOW_BROWN "·      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "·" BEIGE "%s" YELLOW_BROWN "·      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "·" BEIGE "%s" YELLOW_BROWN "·      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN ":" BEIGE "%s" YELLOW_BROWN ":      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "·" BEIGE "%s" YELLOW_BROWN "·      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN ":" BEIGE "%s" YELLOW_BROWN ":      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN ":" BEIGE "%s" YELLOW_BROWN ":      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN ":" BEIGE "%s" YELLOW_BROWN ":      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN ":" BEIGE "%s" YELLOW_BROWN ":      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN ":" BEIGE "%s" YELLOW_BROWN ":      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "|" BEIGE "%s" YELLOW_BROWN "|      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN ":" BEIGE "%s" YELLOW_BROWN ":      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "|" BEIGE "%s" YELLOW_BROWN "|      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "|" BEIGE "%s" YELLOW_BROWN "|      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "|" BEIGE "%s" YELLOW_BROWN "|      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "|" BEIGE "%s" YELLOW_BROWN "|      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "|" BEIGE "%s" YELLOW_BROWN "|      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "|" BEIGE "%s" YELLOW_BROWN "|      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "|" BEIGE "%s" YELLOW_BROWN "]      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "|" BEIGE "%s" YELLOW_BROWN "|      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "|" BEIGE "%s" YELLOW_BROWN "]      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "|" BEIGE "%s" YELLOW_BROWN "]      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "|" BEIGE "%s" YELLOW_BROWN "]      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "|" BEIGE "%s" YELLOW_BROWN "]      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "|" BEIGE "%s" YELLOW_BROWN "]      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "]      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "|" BEIGE "%s" YELLOW_BROWN "]      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "]      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "]      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "]      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "]      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "]      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "]      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "]      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "]      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
}
void heal(Character p1, Character p2, int healthValue, int energyValue, int attackValue) {
    printf("  %s       \n", p1.name);
    ZHEN
        printf("  %s       \n", p1.name);
    ZHEN
        printf("  %s       \n", p1.name);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "." BEIGE "%s" LIGHT_GREEN "." BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "." BEIGE "%s" LIGHT_GREEN "." BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "." BEIGE "%s" LIGHT_GREEN "." BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "." BEIGE "%s" LIGHT_GREEN "." BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "·" BEIGE "%s" LIGHT_GREEN "·" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "·" BEIGE "%s" LIGHT_GREEN "·" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "·" BEIGE "%s" LIGHT_GREEN "·" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "·" BEIGE "%s" LIGHT_GREEN "·" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "○" BEIGE "%s" LIGHT_GREEN "○" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "·" BEIGE "%s" LIGHT_GREEN "·" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "○" BEIGE "%s" LIGHT_GREEN "○" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "○" BEIGE "%s" LIGHT_GREEN "○" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "○" BEIGE "%s" LIGHT_GREEN "○" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "○" BEIGE "%s" LIGHT_GREEN "○" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "○" BEIGE "%s" LIGHT_GREEN "○" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "·" BEIGE "%s" LIGHT_GREEN "·" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "·" BEIGE "%s" LIGHT_GREEN "·" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_GREEN "·" BEIGE "%s" LIGHT_GREEN "·" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf(LIGHT_GREEN "  +%d" BEIGE "\t   \n", healthValue);
    ZHEN
        printf(LIGHT_GREEN "  +%d" BEIGE "\t   \n", healthValue);
    ZHEN
        printf(LIGHT_GREEN "  +%d" BEIGE "\t   \n", healthValue);
    ZHEN
        printf(LIGHT_GREEN "  +%d" BEIGE "\t   \n", healthValue);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
}
void charge(Character p1, Character p2, int healthValue, int energyValue, int attackValue) {
    printf("  %s       \n", p1.name);
    ZHEN
        printf("  %s       \n", p1.name);
    ZHEN
        printf("  %s       \n", p1.name);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "." BEIGE "%s" LIGHT_BLUE "." BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "." BEIGE "%s" LIGHT_BLUE "." BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "." BEIGE "%s" LIGHT_BLUE "." BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "." BEIGE "%s" LIGHT_BLUE "." BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "·" BEIGE "%s" LIGHT_BLUE "·" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "·" BEIGE "%s" LIGHT_BLUE "·" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "·" BEIGE "%s" LIGHT_BLUE "·" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "·" BEIGE "%s" LIGHT_BLUE "·" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "○" BEIGE "%s" LIGHT_BLUE "○" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "·" BEIGE "%s" LIGHT_BLUE "·" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "○" BEIGE "%s" LIGHT_BLUE "○" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "○" BEIGE "%s" LIGHT_BLUE "○" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "○" BEIGE "%s" LIGHT_BLUE "○" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "○" BEIGE "%s" LIGHT_BLUE "○" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "○" BEIGE "%s" LIGHT_BLUE "○" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "·" BEIGE "%s" LIGHT_BLUE "·" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "·" BEIGE "%s" LIGHT_BLUE "·" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " LIGHT_BLUE "·" BEIGE "%s" LIGHT_BLUE "·" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf(LIGHT_BLUE "  +%d" BEIGE "\t   \n", healthValue);
    ZHEN
        printf(LIGHT_BLUE "  +%d" BEIGE "\t   \n", healthValue);
    ZHEN
        printf(LIGHT_BLUE "  +%d" BEIGE "\t   \n", healthValue);
    ZHEN
        printf(LIGHT_BLUE "  +%d" BEIGE "\t   \n", healthValue);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
}
void strengthen(Character p1, Character p2, int healthValue, int energyValue, int attackValue) {
    printf("  %s       \n", p1.name);
    ZHEN
        printf("  %s       \n", p1.name);
    ZHEN
        printf("  %s       \n", p1.name);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "-" BEIGE "%s" BLOOD_RED "-" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "-" BEIGE "%s" BLOOD_RED "-" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "-" BEIGE "%s" BLOOD_RED "-" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "-" BEIGE "%s" BLOOD_RED "-" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "<" BEIGE "%s" BLOOD_RED ">" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "-" BEIGE "%s" BLOOD_RED "-" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "<" BEIGE "%s" BLOOD_RED ">" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "<" BEIGE "%s" BLOOD_RED ">" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "<" BEIGE "%s" BLOOD_RED ">" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "<" BEIGE "%s" BLOOD_RED ">" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf("" BLOOD_RED "< " BEIGE "%s " BLOOD_RED ">" BEIGE "     %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "<" BEIGE "%s" BLOOD_RED ">" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf("" BLOOD_RED "< " BEIGE "%s " BLOOD_RED ">" BEIGE "     %s\n", p1.name, p2.name);
    ZHEN
        printf("" BLOOD_RED "< " BEIGE "%s " BLOOD_RED ">" BEIGE "     %s\n", p1.name, p2.name);
    ZHEN
        printf("" BLOOD_RED "< " BEIGE "%s " BLOOD_RED ">" BEIGE "     %s\n", p1.name, p2.name);
    ZHEN
        printf("" BLOOD_RED "<\\" BEIGE "%s" BLOOD_RED "/>" BEIGE "     %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "\\" BEIGE "%s" BLOOD_RED "/" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "\\" BEIGE "%s" BLOOD_RED "/" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "\\" BEIGE "%s" BLOOD_RED "/" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "\\" BEIGE "%s" BLOOD_RED "/" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "\\" BEIGE "%s" BLOOD_RED "/" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "\\" BEIGE "%s" BLOOD_RED "/" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf("" BLOOD_RED "\\ " BEIGE "%s " BLOOD_RED "/" BEIGE "     %s\n", p1.name, p2.name);
    ZHEN
        printf(" " BLOOD_RED "\\" BEIGE "%s" BLOOD_RED "/" BEIGE "      %s\n", p1.name, p2.name);
    ZHEN
        printf("" BLOOD_RED "\\ " BEIGE "%s " BLOOD_RED "/" BEIGE "     %s\n", p1.name, p2.name);
    ZHEN
        printf("" BLOOD_RED "\\\\" BEIGE "%s" BLOOD_RED "//" BEIGE "     %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf("" BLOOD_RED "\\\\" BEIGE "%s" BLOOD_RED "//" BEIGE "     %s\n", p1.name, p2.name);
    ZHEN
        printf("" BLOOD_RED "\\\\" BEIGE "%s" BLOOD_RED "//" BEIGE "     %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf("" BLOOD_RED "\\\\" BEIGE "%s" BLOOD_RED "//" BEIGE "     %s\n", p1.name, p2.name);
    ZHEN
        printf("" BLOOD_RED "\\\\" BEIGE "%s" BLOOD_RED "//" BEIGE "     %s\n", p1.name, p2.name);
    ZHEN
        printf(BLOOD_RED "  +%d" BEIGE "\t   \n", attackValue);
    ZHEN
        printf(BLOOD_RED "  +%d" BEIGE "\t   \n", attackValue);
    ZHEN
        printf(BLOOD_RED "  +%d" BEIGE "\t   \n", attackValue);
    ZHEN
        printf(BLOOD_RED "  +%d" BEIGE "\t   \n", attackValue);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
}
void chargedAttack1(Character p1, Character p2, int healthValue, int energyValue, int attackValue) {
    printf("  %s       \n", p1.name);
    ZHEN
        printf("  %s       \n", p1.name);
    ZHEN
        printf("  %s       \n", p1.name);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf("  %s       %s\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED " >      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "=>      " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "==>     " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED " ==>    " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "  ==>   " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "   ==>  " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "    ==> " BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "     ==>" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "      ==>" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "       ==%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "        ==" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "         =%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "          =" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s           %s" BLOOD_RED ">" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s            %s" BLOOD_RED ">" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s            " BLOOD_RED "%s=>" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s          " BLOOD_RED "%4d==>" BEIGE "\n", p1.name, healthValue);
    ZHEN
        printf(" %s          " BLOOD_RED "%4d ==>" BEIGE "\n", p1.name, healthValue);
    ZHEN
        printf(" %s          " BLOOD_RED "%4d  ==" BEIGE "\n", p1.name, healthValue);
    ZHEN
        printf(" %s          " BLOOD_RED "%4d   =" BEIGE "\n", p1.name, healthValue);
    ZHEN
        printf(" %s            %s    \n", p1.name, p2.name);
    ZHEN
        printf(" %s            %s    \n", p1.name, p2.name);
    ZHEN
}
void chargedAttack2(Character p1, Character p2, int healthValue, int energyValue, int attackValue) {
    printf("  %s       \n", p1.name);
    ZHEN
        printf("  %s       \n", p1.name);
    ZHEN
        printf("  %s       \n", p1.name);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf(LIGHT_BLUE "  %d" BEIGE "\t   \n", energyValue);
    ZHEN
        printf("  %s      [%s]\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "]" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "]" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "|" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "]" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "|" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "|" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN ":" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "|" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN ":" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN ":" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●    " YELLOW_BROWN "[[" BEIGE "%s" YELLOW_BROWN "·" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN ":" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·    " YELLOW_BROWN "[[" BEIGE "%s" YELLOW_BROWN "·" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●     " YELLOW_BROWN "[" BEIGE "%s" YELLOW_BROWN "·" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●    " YELLOW_BROWN "[[" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "·    " YELLOW_BROWN "[ " BEIGE "%s" YELLOW_BROWN "·" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●    " YELLOW_BROWN "[[" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●     " YELLOW_BROWN "[" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf("  %s" BLOOD_RED "●     " YELLOW_BROWN "[" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED " >     " YELLOW_BROWN "[" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "=>     " YELLOW_BROWN "[" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "==>    " YELLOW_BROWN "[" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED " ==>   " YELLOW_BROWN "[" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "  ==>  " YELLOW_BROWN "[" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "   ==> " YELLOW_BROWN "[" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "    ==>" YELLOW_BROWN "[" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "     ==" YELLOW_BROWN "[" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "      ==" YELLOW_BROWN "[" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "       =" YELLOW_BROWN "[" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "        =" YELLOW_BROWN "[" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "        =" YELLOW_BROWN "[" BEIGE "%s\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "         =%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "         =" BEIGE "%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s" BLOOD_RED "          =%s" BEIGE "\n", p1.name, p2.name);
    ZHEN
        printf(" %s           " BEIGE "%s" BLOOD_RED ">\n", p1.name, p2.name);
    ZHEN
        printf(" %s           %s" BLOOD_RED ">\n", p1.name, p2.name);
    ZHEN
        printf(" %s            " BEIGE "%s" BLOOD_RED ">\n", p1.name, p2.name);
    ZHEN
        printf(" %s            %s" BLOOD_RED ">\n", p1.name, p2.name);
    ZHEN
        printf(" %s          " BLOOD_RED "%4d=>\n", p1.name, healthValue);
    ZHEN
        printf(" %s          " BLOOD_RED "%4d=>\n", p1.name, healthValue);
    ZHEN
        printf(" %s          " BLOOD_RED "%4d =>\n", p1.name, healthValue);
    ZHEN
        printf(" %s          " BLOOD_RED "%4d =>\n", p1.name, healthValue);
    ZHEN
        printf(" %s            %s" BLOOD_RED "  =>\n", p1.name, p2.name);
    ZHEN
        printf(" %s            %s" BLOOD_RED "  =>\n", p1.name, p2.name);
    ZHEN
        printf(" %s            %s" BLOOD_RED "   =\n", p1.name, p2.name);
    ZHEN
        printf(" %s            %s" BLOOD_RED "   =\n", p1.name, p2.name);
    ZHEN
        printf(" %s            %s    \n", p1.name, p2.name);
    ZHEN
        printf(" %s            %s    \n", p1.name, p2.name);
    ZHEN
}
