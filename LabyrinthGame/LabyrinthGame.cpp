#include <iostream>
#include <vector>
#include <Windows.h>
#include <conio.h>
#include <fstream>


enum Color
{
    Black = 0,
    Blue = 1,
    Green = 2,
    Cyan = 3,
    Red = 4,
    Magenta = 5,
    Brown = 6,
    LightGray = 7,
    DarkGray = 8,
    LightBlue = 9,
    LightGreen = 10,
    LightCyan = 11,
    LightRed = 12,
    LightMagenta = 13,
    Yellow = 14,
    White = 15
};
const struct MappedObjects
{
    char bomb = '9';
    char aidkit = '8';
    char wall = '#';
    char finish = 'D';
    char air = ' ';
} mappedObjects;

const struct AidKitSettings
{
    int minRegen = 10;
    int maxRegen = 50;
}aidKitSettings;

const struct BombSettings
{
    int minDamage = 10;
    int maxDamage = 50;
}bombSettings;

struct GameState
{
    bool shouldExit = false;
}gameState;

struct GameSettings
{
    int MapSizeX = 0;
    int MapSizeY = 0;
    int BombsCount = 0;
    int AidCount = 0;
    int PlayerHP = 0;
    int DrillsCount = 0;
    int BackgroundColor = 0;
};

struct Player
{
    int hp = 0;
    int maxhp = 100;
    struct Inventory
    {
        int countOfDrills = 0;
        int countOfAidKits = 0;
    };
    char drawableChar = '%';
    int x = 0;
    int y = 0;
    Inventory inventory;
};

struct Map
{
    int countOfBombs = 0;
    int countOfAidKits = 0;
    const int sizeX = 30;
    const int sizeY = 15;
    char** map = new char* [sizeY];
    Map()
    {
        
        map[0] = new char[30] {"############################"};
        map[1] = new char[30] {"#          #   #     #   # #"};
        map[2] = new char[30] {"#  ##  ##   #   #   #   #  #"};
        map[3] = new char[30] {"##    #  #  #        #  #  #"};
        map[4] = new char[30] {"##    #  #  #  #    ##   # #"};
        map[5] = new char[30] {"##     # #  #     #     #  #"};
        map[6] = new char[30] {"#  #     ###     #  #    # #"};
        map[7] = new char[30] {"#   # #  #    #      #   ###"};
        map[8] = new char[30] {"#  #       #     #  ##     #"};
        map[9] = new char[30] {"#     #  # # #       #  #  #"};
        map[10] = new char[30] {"#  #  #   #      #  #    # #"};
        map[11] = new char[30] {"#   # # #    # # #  #  #  ##"};
        map[12] = new char[30] {"#  #  # ###    #    #     ##"};
        map[13] = new char[30] {"#    ##   #      #     #   #"};
        map[14] = new char[30] {"#########################D##"};
    }
    ~Map()
    {
        for (int i = 0; i < sizeY; i++)
            delete[]map[i];
        delete[]map;
    }
};

void GenerateObject(Map& map, int amount, char object)
{
    int x = 0;
    int y = 0;
    int flag = 0;
    while (flag < amount)
    {
        x = rand() % (map.sizeX - 1) + 1;
        y = rand() % (map.sizeY - 1) + 1;
        if (map.map[y][x] == mappedObjects.air)
        {
            map.map[y][x] = object;
            flag++;
        }
    }
}

const struct Control
{
    char up = 'w';
    char down = 's';
    char right = 'd';
    char left = 'a';
    char useAid = 'q';
    char useDrill = 'e';
} control;

void SetColor(Color text, Color background)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}

void initMap(Map& map, GameSettings settings)
{
    map.countOfBombs = settings.BombsCount;
    map.countOfAidKits = settings.AidCount;
    GenerateObject(map, map.countOfBombs, mappedObjects.bomb);
    GenerateObject(map, map.countOfAidKits, mappedObjects.aidkit);
}

void initPlayer(Player& player, GameSettings &settings, Map& map)
{
    player.hp = settings.PlayerHP;
    player.inventory.countOfDrills = settings.DrillsCount;
    while (map.map[player.y][player.x] != mappedObjects.air)
    {
        player.x = rand() % (map.sizeX - 1) + 1;
        player.y = rand() % (map.sizeY - 1) + 1;
    }
}   

void setCursorPosition(int x, int y) {
    COORD pos;
    pos.X = x;
    pos.Y = y;
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(output, pos);
}

void DrawHUD(const Player &player, const Map &map)
{
    std::cout << "HP:" << player.hp << " AID KITS:" << player.inventory.countOfAidKits << " DRILLS:" << player.inventory.countOfDrills << std::endl;
    std::cout << "BOMBS ON MAP: " << map.countOfBombs << " AID KITS ON MAP: " << map.countOfAidKits << std::endl;
}

void DrawMap(Map& map)
{
    for (int i = 0; i < map.sizeY; ++i)
    {
        for (int j = 0; j < map.sizeX; ++j)
            if (map.map[i][j] == mappedObjects.bomb)
            {
                SetColor(Black, Black);
                std::cout << map.map[i][j];
                SetColor(White, Black);
            }
            else
            {
                std::cout << map.map[i][j];
            }

        std::cout << std::endl;
    }
}

void UseAid(Player& player)
{
    if (player.inventory.countOfAidKits == 0)
    {
        return;
    }

    player.hp += rand() % (aidKitSettings.maxRegen - aidKitSettings.minRegen) + aidKitSettings.minRegen;
    if (player.hp > player.maxhp)
    {
        player.hp = player.maxhp;
    }
    player.inventory.countOfAidKits--;
}

void TryToUseAid(char input, Player& player)
{
    if (input == control.useAid)
    {
        UseAid(player);
    }
}

void TryToUseDrill(char input, Player& player, bool& drill)
{
    if (input == control.useDrill && player.inventory.countOfDrills != 0)
    {
        drill = true;
        player.inventory.countOfDrills--;
    }
}


void TryToMove(char control, char& cell, bool& drill, char input, Map& map, Player& player)
{
    if (input != control || (cell == mappedObjects.wall && drill == false))
    {
        return;
    }

    if (cell == mappedObjects.bomb)
    {
        map.countOfBombs--;
        cell = mappedObjects.air;
        player.hp -= rand() % (bombSettings.maxDamage - bombSettings.minDamage) + bombSettings.minDamage;
    }
    else if (cell == mappedObjects.aidkit)
    {
        player.inventory.countOfAidKits++;
        map.countOfAidKits--;
        cell = mappedObjects.air;
    }
    else if (cell == mappedObjects.finish)
    {
        setCursorPosition(0, map.sizeY + 1);

        std::cout << "CONGRATS, U WON, RETURNING TO MENU..." << std::endl;
        gameState.shouldExit=true;
        
        system("pause");
        std::fstream file("file.txt", std::ios::out);
        for (int i = 0; i < map.sizeY; ++i)
        {
            for (int j = 0; j < map.sizeX; ++j)
                file << map.map[i][j];

            file << std::endl;
        }
        file.close();
        return;
    }
    else if (cell == mappedObjects.wall)
    {
        if (cell + 1 == '#')
            return;

        cell = mappedObjects.air;
        drill = false;
    }

    if (control == ::control.down)
        player.y++;
    if (control == ::control.up)
        player.y--;
    if (control == ::control.right)
        ++player.x;
    if (control == ::control.left)
        --player.x;
}

void game(Player &player, Map &map)
{

    DrawMap(map);
    DrawHUD(player, map);

    setCursorPosition(player.x, player.y);
    std::cout << player.drawableChar;
    bool drill = false;

    while (!gameState.shouldExit)
    {
        char input = 0;
        input = _getch();
        setCursorPosition(player.x, player.y);
        std::cout << mappedObjects.air;

        TryToUseAid(input, player);
        TryToUseDrill(input, player, drill);

        TryToMove(control.down, map.map[player.y + 1][player.x], drill, input, map, player);
        TryToMove(control.up, map.map[player.y - 1][player.x], drill, input, map, player);
        TryToMove(control.right, map.map[player.y][player.x + 1], drill, input, map, player);
        TryToMove(control.left, map.map[player.y][player.x - 1], drill, input, map, player);

        setCursorPosition(0, map.sizeY);
        std::cout << "                                                                                                             "<<std::endl;
        std::cout << "                                                                                                             ";
        setCursorPosition(0, map.sizeY);
        DrawHUD(player, map);
        std::cout << "Time in game = " << static_cast<double>(clock()) / CLOCKS_PER_SEC << std::endl;
        if (player.hp <= 0)
        {
            std::cout << "YOU LOSE, RETURNING TO MENU..." << std::endl;
            Sleep(3000);
            system("cls");
            gameState.shouldExit = true;
        }

        setCursorPosition(player.x, player.y);
        std::cout << player.drawableChar;
    }


}
int main()
{
    srand(time(0));
    Player player;
    GameSettings settings;
    

    int menu = -1;

    while (menu != 0)
    {
        Map map;
        gameState.shouldExit = false;
        std::cout << "Welcome to labyrinth game, choose difficulty level" << std::endl;
        std::cout << "1 for easy" << std::endl;
        std::cout << "2 for medium" << std::endl;
        std::cout << "3 for hard" << std::endl;
        std::cout << "0 to close game" << std::endl;

        std::cin >> menu;
        switch (menu)
        {
        case 1:
            settings.AidCount = 20;
            settings.BombsCount = 10;
            settings.PlayerHP = 100;
            settings.DrillsCount = 2;
            initPlayer(player, settings, map);
            initMap(map, settings);
            system("cls");
            game(player, map);
            system("cls");
            break;
        case 2:
            settings.AidCount = 20;
            settings.BombsCount = 20;
            settings.PlayerHP = 100;
            settings.DrillsCount = 1;
            initPlayer(player, settings, map);
            initMap(map, settings);
            system("cls");
            game(player, map);
            system("cls");
            break;
        case 3:
            settings.AidCount = 10;
            settings.BombsCount = 20;
            settings.PlayerHP = 100;
            settings.DrillsCount = 0;
            initPlayer(player, settings, map);
            initMap(map, settings);
            system("cls");
            game(player, map);
            system("cls");
            break;
        }
      
    }

    return 0;

}


