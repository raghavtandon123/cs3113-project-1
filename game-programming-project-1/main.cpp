/**

* Author: Raghav Tandon

* Assignment: Project 1

* Date due: 10/05/2026

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/

#include "CS3113/cs3113.h"

enum BallState {SHOOTING, RETURNING};

// Global Constants
constexpr int SCREEN_WIDTH  = 1200,
              SCREEN_HEIGHT = 675,
              FPS           = 60;

constexpr char DAY_COLOUR[] = "#87CEEB";
constexpr char NIGHT_COLOUR[] = "#0F1432";
constexpr Vector2 HAND_OFFSET = { 62.0f, -100.0f };

constexpr float SKY_SPEED = 0.5f;
float gSkyTime = 0.0f;
float gSkyBlend = 0.0f;

constexpr float BALL_SPIN_SPEED = 360.0f;
constexpr float BALL_GROW_AMOUNT = 25.0f;

constexpr char BG_COLOUR[] = "#B2AAC6";

float gPreviousTicks = 0.0f;

constexpr char PLAYER_FP[] = "assets/player.png";
constexpr char BALL_FP[] = "assets/ball.png";
constexpr char HOOP_FP[] = "assets/hoop.png";

constexpr float PLAYER_SIZE = 250.0f;
constexpr float HOOP_SIZE = 200.0f;
constexpr float BALL_SIZE = 50.0f;

constexpr Vector2 HOOP_CENTER = {SCREEN_WIDTH * 0.8f, SCREEN_HEIGHT * 0.37f};
constexpr float HOOP_RADIUS = 60.0f;
constexpr float HOOP_SPEED = 1.0f;

constexpr Vector2 PLAYER_CENTER = {SCREEN_WIDTH * 0.2f, SCREEN_HEIGHT * 0.67f};
constexpr float PLAYER_SWAY_X = 30.0f;
constexpr float PLAYER_SWAY_Y = 10.0f;
constexpr float PLAYER_SWAY_SPEED = 1.2f; 
// the sway is kinda slow but im trying to make it like a sway when youre shooting a ball like how players fidget and stuff
// also the player is moving for the moving hoop as well if you get what i mean

constexpr float SHOT_SPEED = 0.6f;
constexpr float SHOT_HEIGHT = 250.0f;
constexpr Vector2 RIM_OFFSET = {-15.0f, -5.0f};

// Global Variables
AppStatus gAppStatus = RUNNING;

float gPlayerSwayTime = 0.0f;

Texture2D gPlayerTexture;
Texture2D gBallTexture;
Texture2D gHoopTexture;

Vector2 gPlayerPosition = {SCREEN_WIDTH * 0.2f, SCREEN_HEIGHT * 0.67f};
Vector2 gPlayerScale    = {PLAYER_SIZE, PLAYER_SIZE };

Vector2 gHoopPosition   = {SCREEN_WIDTH * 0.8f, SCREEN_HEIGHT * 0.37f};
Vector2 gHoopScale      = {HOOP_SIZE, HOOP_SIZE};

Vector2 gBallPosition   = {0.0f, 0.0f};
Vector2 gBallScale      = {BALL_SIZE, BALL_SIZE};

float gHoopAngle = 0.0f;

BallState gBallState = SHOOTING;
float gShotProgress = 0.0f;
float gBallAngle = 0.0f;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Textures & Delta Time");

    gPlayerTexture = LoadTexture(PLAYER_FP);
    gBallTexture   = LoadTexture(BALL_FP);
    gHoopTexture   = LoadTexture(HOOP_FP);

    SetTargetFPS(FPS);
}

void processInput()
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() {
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    gSkyTime += SKY_SPEED * deltaTime;
    gSkyBlend = (sinf(gSkyTime) + 1.0f) / 2.0f;

    gHoopAngle += HOOP_SPEED * deltaTime;
    gHoopPosition = {
        HOOP_CENTER.x + HOOP_RADIUS * cosf(gHoopAngle),
        HOOP_CENTER.y + HOOP_RADIUS * sinf(gHoopAngle)
    };

    gPlayerSwayTime += PLAYER_SWAY_SPEED * deltaTime;
    gPlayerPosition = {
        PLAYER_CENTER.x + PLAYER_SWAY_X * cosf(gPlayerSwayTime),
        PLAYER_CENTER.y - PLAYER_SWAY_Y * sinf(gPlayerSwayTime)
    };
    Vector2 handPosition = {
        gPlayerPosition.x + HAND_OFFSET.x,
        gPlayerPosition.y + HAND_OFFSET.y
    };

    Vector2 rimPosition = {
        gHoopPosition.x + RIM_OFFSET.x,
        gHoopPosition.y + RIM_OFFSET.y
    };
    if (gBallState == SHOOTING) {
        gShotProgress += SHOT_SPEED * deltaTime;
        if (gShotProgress >= 1.0f) {
            gShotProgress = 0.0f;
        }
    }
    float shotCenter = (handPosition.x + rimPosition.x) / 2.0f;
    float shotRadius = (rimPosition.x - handPosition.x) / 2.0f;
    float shotAngle = PI * (1.0f - gShotProgress);

    gBallPosition = {
        shotCenter + shotRadius * cosf(shotAngle),
        handPosition.y + (rimPosition.y - handPosition.y) * gShotProgress - SHOT_HEIGHT * sinf(shotAngle)
    };
        gBallAngle += BALL_SPIN_SPEED * deltaTime;

    float ballSize = BALL_SIZE + BALL_GROW_AMOUNT * sinf(PI * gShotProgress);
    gBallScale = {ballSize, ballSize};
}


void render()
{
    BeginDrawing();

    if (gSkyBlend < 0.5f) {
        ClearBackground(ColorFromHex(DAY_COLOUR));
    }
    else {
        ClearBackground(ColorFromHex(NIGHT_COLOUR));
    }

    Rectangle playerTextureArea = {
        0.0f, 0.0f,
        static_cast<float>(gPlayerTexture.width),
        static_cast<float>(gPlayerTexture.height)
    };
    Rectangle playerDestinationArea = {
        gPlayerPosition.x,
        gPlayerPosition.y,
        gPlayerScale.x,
        gPlayerScale.y
    };
    Vector2 playerOrigin = {
        gPlayerScale.x / 2.0f,
        gPlayerScale.y / 2.0f
    };
    DrawTexturePro(gPlayerTexture, playerTextureArea, playerDestinationArea, playerOrigin, 0.0f, WHITE);

    Rectangle hoopTextureArea = {
        0.0f, 0.0f,
        static_cast<float>(gHoopTexture.width),
        static_cast<float>(gHoopTexture.height)
    };
    Rectangle hoopDestinationArea = {
        gHoopPosition.x,
        gHoopPosition.y,
        gHoopScale.x,
        gHoopScale.y
    };
    Vector2 hoopOrigin = {
        gHoopScale.x / 2.0f,
        gHoopScale.y / 2.0f
    };
    DrawTexturePro(gHoopTexture, hoopTextureArea, hoopDestinationArea, hoopOrigin, 0.0f, WHITE);
    //basetketball!!!!!!!
    Rectangle ballTextureArea = {
        0.0f, 0.0f,
        static_cast<float>(gBallTexture.width),
        static_cast<float>(gBallTexture.height)
    };
    Rectangle ballDestinationArea = {
        gBallPosition.x,
        gBallPosition.y,
        gBallScale.x,
        gBallScale.y
    };
    Vector2 ballOrigin = {
        gBallScale.x / 2.0f,
        gBallScale.y / 2.0f
    };
    DrawTexturePro(gBallTexture, ballTextureArea, ballDestinationArea, ballOrigin, 0.0f, WHITE);
    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gPlayerTexture);
    UnloadTexture(gBallTexture);
    UnloadTexture(gHoopTexture);
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}
