#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <style_amber.h>

#include <fox_logo.h>
#include <fox_animated.h>

/* ------------------------------- NON-RAYLIB CODES ------------------------------- */
typedef struct node
{
    struct node *next;
    unsigned int index;
    double locX, locY;
    double prevLocX, prevLocY;
    double angularV;
    double rodL;
    double radius;
    Color color;
} node;

node *newNode(unsigned int *index, int X, int Y, double angularV, double radius, Color color, node **lastNode)
{
    node *n = malloc(sizeof(node));

    X = max(10, X);
    Y = max(10, Y);

    n->index = (*index);
    n->locX = X;
    n->locY = Y;
    n->angularV = angularV;
    n->radius = radius;
    n->color = color;

    (*lastNode)->rodL = sqrt(pow((*lastNode)->locX - X, 2) + pow((*lastNode)->locY - Y, 2));
    (*lastNode)->next = n;
    (*index)++;

    return n;
}

node *removeLastNode(unsigned int *index, node *root, node *lastNode)
{
    if (*index == 1)
        return root;

    (*index)--;

    node *newLast = root;
    for (unsigned int i = 1; i < *index; i++)
    {
        newLast = newLast->next;
    }
    newLast->next = NULL;

    free(lastNode);

    return newLast;
}

// not used
void printNodes(unsigned int *index, node *root)
{
    node *curr = root;
    for (unsigned int i = 0; i < (*index); i++)
    {
        printf("%d, %f, %f, %f, %f\n", curr->index, curr->locX, curr->locY, curr->angularV, curr->rodL);
        curr = curr->next;
    }

    printf("\n");
}

void calculate(unsigned int *index, node *root, float deltaTime)
{
    node *prev = root;
    double prevX = root->locX;
    double prevY = root->locY;
    double newX, newY;

    for (unsigned int i = 1; i < (*index); i++)
    {
        node *curr = prev->next;

        double alpha = curr->angularV * deltaTime;

        double centeredX = curr->locX - prevX;
        double centeredY = curr->locY - prevY;

        newX = (centeredX * cos(alpha)) - (centeredY * sin(alpha));
        newY = (centeredX * sin(alpha)) + (centeredY * cos(alpha));

        newX += prev->locX;
        newY += prev->locY;

        prevX = curr->locX;
        prevY = curr->locY;

        curr->locX = newX;
        curr->locY = newY;

        prev = curr;
    }
}
/* ------------------------------- NON-RAYLIB CODES ------------------------------- */

/* --------------------------------- RAYLIB CODES --------------------------------- */

typedef enum
{
    LOGO = 0,
    INTRO,
    MAIN
} e_screen;

void initProgram(int width, int height)
{
    InitWindow(width, height, "bunch of balls");

    SetTargetFPS(120);
}

/* --------------------------------- RAYLIB CODES --------------------------------- */

int main()
{
    srand(time(NULL));

    Color colorList[7] = {LIGHTGRAY, ORANGE, DARKPURPLE, LIME, PINK, GOLD, MAROON};

    unsigned int _index = 0;
    unsigned int *index = &_index;

    node *root = malloc(sizeof(node));
    root->index = _index++;
    root->locX = 800; /*Center of screen*/
    root->locY = 400;
    root->angularV = 0;
    root->rodL = 0; /*Random Value*/
    root->radius = 5;
    root->color = RED;

    node *lastNode = root;

    for (int i = 0; i < 3; i++)
    {
        int X = lastNode->locX + (rand() % 80) * (pow(-1, rand() % 2));
        int Y = lastNode->locY + (rand() % 80) * (pow(-1, rand() % 2));
        unsigned int V = (rand() % 7) + 1;
        double radius = (rand() % 5) + 5;
        Color color = colorList[rand() % 7];

        lastNode = newNode(index, X, Y, V, radius, color, &lastNode);
    }

    int width = 1600;
    int height = 800;
    initProgram(width, height);

    e_screen curr_screen = LOGO;
    unsigned int frames = 0;

    int animFrames = 0;
    int curr_animFrames = 0;
    int frameDelay = 10;
    int frameCounter = 0;
    int nextFrameOffset = 0;
    // Change
    Image imIntroAnim = LoadImageAnimFromMemory(".gif", fox_animated_gif, fox_animated_gif_len, &animFrames);
    Image imFoxLogo = LoadImageFromMemory(".png", fox_logo_png, fox_logo_png_len);
    Texture2D texFoxLogo = LoadTextureFromImage(imFoxLogo);
    Texture2D texIntroAnim = LoadTextureFromImage(imIntroAnim);

    UnloadImage(imFoxLogo);

    char isPaused = 0;
    char isMenuOpen = 0;
    char drawOrbit = 0;
    char selectionCoord = 0;
    unsigned int customX = 1;
    unsigned int customY = 1;
    unsigned int customVelocity = 1;
    unsigned int customRadius = 1;
    Color customColor = RED;
    GuiLoadStyleAmber();
    while (!WindowShouldClose())
    {
        switch (curr_screen)
        {
        case LOGO:
            frames++;

            if (frames > 900 /*900*/)
            {
                curr_screen = INTRO;
            }
            break;

        case INTRO:
            frameCounter++;
            if (frameCounter >= frameDelay)
            {
                curr_animFrames++;

                if (curr_animFrames >= animFrames)
                    curr_animFrames = 0;

                nextFrameOffset = imIntroAnim.width * imIntroAnim.height * 4 * curr_animFrames;

                UpdateTexture(texIntroAnim, ((unsigned char *)imIntroAnim.data) + nextFrameOffset);

                frameCounter = 0;
            }
            if (GetKeyPressed())
            {
                curr_screen = MAIN;
            }
            break;
        case MAIN:

            if (IsKeyPressed(KEY_SPACE))
            {
                isPaused = 1 - isPaused;
            }
            if (IsKeyPressed(KEY_P))
            {
                isMenuOpen = 1 - isMenuOpen;
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) & !isPaused & !isMenuOpen)
            {
                int X = lastNode->locX + (rand() % 80) * (pow(-1, rand() % 2));
                int Y = lastNode->locY + (rand() % 80) * (pow(-1, rand() % 2));
                unsigned int V = (rand() % 7) + 1;
                double radius = (rand() % 5) + 5;
                Color color = colorList[rand() % 7];

                lastNode = newNode(index, X, Y, V, radius, color, &lastNode);
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) & !isPaused & !isMenuOpen)
            {
                lastNode = removeLastNode(index, root, lastNode);
            }

            if (!isPaused)
                calculate(index, root, GetFrameTime());

            if (selectionCoord)
            {
                customX = GetMouseX();
                customY = GetMouseY();

                customX = min(1599, customX);
                customY = min(1599, customY);

                customX = max(1, customX);
                customY = max(1, customY);

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    selectionCoord = 0;
            }

            break;
        default:
            break;
        }

        // ------

        BeginDrawing();
        ClearBackground(WHITE);

        switch (curr_screen)
        {
        case LOGO:
            DrawText("PLAKANKA", min(580, frames * 1.2), min(300, frames / 1.8), 80, RED);

            DrawTexture(texFoxLogo, 570, 400, WHITE);
            break;

        case INTRO:
            DrawText("RAS", 20, 20, 100, BLUE);
            DrawText("Left Mouse -> adds a ball", 20, 140, 40, PURPLE);
            DrawText("Right Mouse -> removes a ball", 20, 200, 40, PURPLE);
            DrawText("Space -> pauses", 20, 260, 40, PURPLE);
            DrawText("P -> opens menu", 20, 320, 40, PURPLE);
            DrawText("Escape -> closes the app", 20, 380, 40, PURPLE);

            DrawText("Click any key to skip...", 900, 700, 60, VIOLET);

            DrawTexture(texIntroAnim, 1100, -50, WHITE);
            break;
        case MAIN:

            if (isMenuOpen)
            {
                GuiPanel((Rectangle){24, 24, 200, 385}, "hi                RAS PLAKANKA");
                GuiValueBox((Rectangle){90, 55, 50, 20}, "X , Y -> ", &customX, 1, 1599, false);
                GuiValueBox((Rectangle){150, 55, 50, 20}, NULL, &customY, 1, 1599, false);

                if (GuiButton((Rectangle){30, 85, 188, 20}, "toggle coordinate selection"))
                    selectionCoord = 1;

                GuiSpinner((Rectangle){100, 115, 118, 20}, "velocity -> ", &customVelocity, 1, 99, false);
                GuiSpinner((Rectangle){90, 145, 128, 20}, "radius -> ", &customRadius, 1, 99, false);
                GuiColorPicker((Rectangle){30, 175, 158, 100}, "test", &customColor);

                if (GuiButton((Rectangle){30, 285, 188, 20}, "add button"))
                    lastNode = newNode(index, customX, customY, customVelocity, customRadius, customColor, &lastNode);

                GuiToggle((Rectangle){30, 315, 188, 20}, "draw orbit toggle", &drawOrbit);

                if (GuiButton((Rectangle){30, 345, 188, 20}, "add 10 random"))
                    for (int _ = 0; _ < 10; _++)
                    {
                        int X = lastNode->locX + (rand() % 80) * (pow(-1, rand() % 2));
                        int Y = lastNode->locY + (rand() % 80) * (pow(-1, rand() % 2));
                        unsigned int V = (rand() % 7) + 1;
                        double radius = (rand() % 5) + 5;
                        Color color = colorList[rand() % 7];

                        lastNode = newNode(index, X, Y, V, radius, color, &lastNode);
                    }

                if (GuiButton((Rectangle){30, 375, 188, 20}, "remove 10"))
                    for (int _ = 0; _ < 10; _++)
                        lastNode = removeLastNode(index, root, lastNode);
            }

            node *curr = root;
            for (int i = 0; i < *index; i++)
            {

                if (drawOrbit)
                    DrawCircleLines(curr->locX, curr->locY, curr->rodL, BLUE);

                if (i != *index - 1)
                {
                    DrawCircle(curr->locX, curr->locY, curr->radius, curr->color);
                    DrawLine(curr->locX, curr->locY, curr->next->locX, curr->next->locY, BLACK);
                    curr = curr->next;
                }
                else
                    DrawCircle(curr->locX, curr->locY, curr->radius, curr->color);
            }
            if (isPaused)
            {
                DrawText("PAUSED", 1360, 30, 40, RED);
                DrawText("| |", 1540, 20, 60, RED);
            }
            break;
        default:
            break;
        }

        EndDrawing();
    }

    UnloadTexture(texFoxLogo);
    UnloadTexture(texIntroAnim);
    UnloadImage(imIntroAnim);

    CloseWindow();

    return 0;
}