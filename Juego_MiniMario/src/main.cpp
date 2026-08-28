/*
 * =====================================================================
 *  MINI MARIO - Juego tipo "Mario Bros" (endless runner) para
 *  ESP32-S3 N16R8 + Pantalla ST7735S (SPI) + Joystick analógico
 * =====================================================================
 *
 *  CONEXIONES:
 *
 *  Pantalla ST7735S            Joystick
 *   VCC -> 3V3                  VCC -> 3V3 (¡NO 5V!)
 *   GND -> GND                  GND -> GND
 *   SCL -> GPIO12 (SCK)         VRx -> GPIO1  (ADC)
 *   SDA -> GPIO11 (MOSI)        VRy -> GPIO2  (ADC)
 *   CS  -> GPIO10               SW  -> GPIO4  (digital, pull-up)
 *   DC  -> GPIO9
 *   RST -> GPIO8
 *   BL  -> GPIO7   (retroiluminación)
 *
 *  Controles del juego:
 *   - Botón del joystick (SW)  -> Saltar / Iniciar / Reiniciar
 *   - Joystick hacia abajo (VRy) -> Agacharse (esquivar enemigos voladores)
 *   - Joystick izquierda/derecha (VRx) -> Reservado (no usado en este modo)
 *
 *  Librerías necesarias (ya declaradas en platformio.ini):
 *   - Adafruit GFX Library
 *   - Adafruit ST7735 and ST7789 Library
 *   - Adafruit BusIO
 * =====================================================================
 */

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Preferences.h>   // Para guardar el puntaje máximo en memoria NVS

// ----------------------- PINES DE LA PANTALLA -----------------------
#define TFT_SCLK  12
#define TFT_MOSI  11
#define TFT_CS    10
#define TFT_DC     9
#define TFT_RST    8
#define TFT_BL     7

// ----------------------- PINES DEL JOYSTICK --------------------------
#define PIN_VRX    1
#define PIN_VRY    2
#define PIN_SW     4

// Si tu módulo ST7735 es de otro "tab color", cambia INITR_BLACKTAB
// por INITR_GREENTAB, INITR_REDTAB o INITR_144GREENTAB según el caso.
#define INIT_TAB   INITR_BLACKTAB

// --------------------------------------------------------------------
//  Objeto de pantalla usando SPI por hardware con pines personalizados
// --------------------------------------------------------------------
SPIClass tftSPI = SPIClass(FSPI);
Adafruit_ST7735 tft = Adafruit_ST7735(&tftSPI, TFT_CS, TFT_DC, TFT_RST);

// Tras rotation(1) la pantalla queda en modo horizontal (apaisada)
int SCREEN_W = 160;
int SCREEN_H = 128;

// --------------------------------------------------------------------
//  Colores (RGB565)
// --------------------------------------------------------------------
#define COLOR_SKY      ST77XX_CYAN
#define COLOR_GROUND   0x8A22   // marrón tierra
#define COLOR_GROUND2  0x4A20   // marrón oscuro (borde del piso)
#define COLOR_PIPE     ST77XX_GREEN
#define COLOR_PIPE_DK  0x0400
#define COLOR_COIN     ST77XX_YELLOW
#define COLOR_WHITE    ST77XX_WHITE
#define COLOR_BLACK    ST77XX_BLACK
#define COLOR_RED      ST77XX_RED
#define COLOR_SKIN     0xFCC0
#define COLOR_BLUE     ST77XX_BLUE
#define COLOR_BIRD     0x6B4D

// --------------------------------------------------------------------
//  Parámetros del juego
// --------------------------------------------------------------------
const int GROUND_Y      = 106;   // línea del piso (coordenada Y)
const int PLAYER_X      = 18;    // posición X fija del jugador
const int PLAYER_W      = 12;
const int PLAYER_H_NORM = 16;
const int PLAYER_H_DUCK = 9;

const float GRAVITY       = 0.9f;
const float JUMP_VELOCITY = -9.6f;

const int JOY_CENTER   = 2048;   // ADC 12 bits en ESP32-S3 (0-4095)
const int JOY_DEADZONE = 700;

// --------------------------------------------------------------------
//  Estados del juego
// --------------------------------------------------------------------
enum GameState { START_SCREEN, PLAYING, GAME_OVER };
GameState gameState = START_SCREEN;

// --------------------------------------------------------------------
//  Jugador
// --------------------------------------------------------------------
struct Player {
  float y;
  float vy;
  bool  onGround;
  bool  ducking;
  int   animFrame;
} player;

// --------------------------------------------------------------------
//  Obstáculos (tuberías en el piso y "pájaros" voladores)
// --------------------------------------------------------------------
enum ObstacleType { OBST_PIPE, OBST_BIRD };

struct Obstacle {
  float x;
  int   w, h;
  int   flyY;        // altura de vuelo (solo para OBST_BIRD)
  ObstacleType type;
  bool  active;
  bool  scored;
};

const int MAX_OBSTACLES = 3;
Obstacle obstacles[MAX_OBSTACLES];

// --------------------------------------------------------------------
//  Monedas
// --------------------------------------------------------------------
struct Coin {
  float x, y;
  bool  active;
};
const int MAX_COINS = 4;
Coin coins[MAX_COINS];

// --------------------------------------------------------------------
//  Variables de juego
// --------------------------------------------------------------------
float gameSpeed        = 2.4f;
unsigned long score    = 0;
unsigned long highScore = 0;
unsigned long lastFrameTime = 0;
unsigned long lastSpawnTime = 0;
unsigned long spawnInterval = 1400;
int groundScrollX = 0;

bool prevButtonState = HIGH;

Preferences prefs;

// --------------------------------------------------------------------
//  Prototipos
// --------------------------------------------------------------------
void resetGame();
void readInputs(bool &jumpPressed, bool &ducking);
void updatePlaying(float dt, bool jumpPressed, bool ducking);
void spawnObstacle();
void spawnCoinMaybe(float x, int h);
bool checkCollision();
void drawGame();
void drawPlayer();
void drawObstacle(Obstacle &o);
void drawCoin(Coin &c);
void drawGround();
void drawStartScreen();
void drawGameOverScreen();
void drawHUD();

// =====================================================================
//  SETUP
// =====================================================================
void setup() {
  Serial.begin(115200);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);      // Enciende la retroiluminación

  pinMode(PIN_SW, INPUT_PULLUP);   // Botón del joystick con pull-up interno
  analogReadResolution(12);        // ADC de 12 bits (0-4095) en ESP32-S3

  // Inicializa el bus SPI con los pines personalizados
  tftSPI.begin(TFT_SCLK, -1 /* MISO no usado */, TFT_MOSI, TFT_CS);

  tft.initR(INIT_TAB);
  tft.setRotation(1);              // Pantalla horizontal (160x128)
  tft.fillScreen(COLOR_BLACK);

  prefs.begin("minimario", false);
  highScore = prefs.getULong("highscore", 0);

  randomSeed(analogRead(PIN_VRX) + analogRead(PIN_VRY) + millis());

  resetGame();
  drawStartScreen();

  lastFrameTime = millis();
}

// =====================================================================
//  LOOP PRINCIPAL
// =====================================================================
void loop() {
  unsigned long now = millis();
  float dt = (now - lastFrameTime) / 16.0f;   // dt normalizado a ~60 FPS
  if (dt > 3.0f) dt = 3.0f;                   // evita saltos grandes
  lastFrameTime = now;

  bool jumpPressed, ducking;
  readInputs(jumpPressed, ducking);

  switch (gameState) {

    case START_SCREEN:
      if (jumpPressed) {
        resetGame();
        gameState = PLAYING;
        tft.fillScreen(COLOR_SKY);
      }
      break;

    case PLAYING:
      updatePlaying(dt, jumpPressed, ducking);
      drawGame();
      if (checkCollision()) {
        gameState = GAME_OVER;
        if (score > highScore) {
          highScore = score;
          prefs.putULong("highscore", highScore);
        }
        drawGameOverScreen();
      }
      break;

    case GAME_OVER:
      if (jumpPressed) {
        resetGame();
        gameState = PLAYING;
        tft.fillScreen(COLOR_SKY);
      }
      break;
  }

  delay(16);   // ~60 FPS aprox.
}

// =====================================================================
//  LECTURA DE ENTRADAS (joystick)
// =====================================================================
void readInputs(bool &jumpPressed, bool &ducking) {
  jumpPressed = false;

  bool currentButtonState = digitalRead(PIN_SW);
  // Flanco de bajada = botón recién presionado
  if (prevButtonState == HIGH && currentButtonState == LOW) {
    jumpPressed = true;
  }
  prevButtonState = currentButtonState;

  int vy = analogRead(PIN_VRY);
  // Ajusta el signo si tu joystick está montado al revés
  ducking = (vy > (JOY_CENTER + JOY_DEADZONE));
}

// =====================================================================
//  REINICIAR JUEGO
// =====================================================================
void resetGame() {
  player.y = GROUND_Y - PLAYER_H_NORM;
  player.vy = 0;
  player.onGround = true;
  player.ducking = false;
  player.animFrame = 0;

  for (int i = 0; i < MAX_OBSTACLES; i++) obstacles[i].active = false;
  for (int i = 0; i < MAX_COINS; i++) coins[i].active = false;

  score = 0;
  gameSpeed = 2.4f;
  spawnInterval = 1400;
  lastSpawnTime = millis();
  groundScrollX = 0;
}

// =====================================================================
//  ACTUALIZAR LÓGICA DEL JUEGO (mientras se juega)
// =====================================================================
void updatePlaying(float dt, bool jumpPressed, bool ducking) {

  // --- Jugador: salto ---
  if (jumpPressed && player.onGround) {
    player.vy = JUMP_VELOCITY;
    player.onGround = false;
  }

  player.ducking = ducking && player.onGround;

  player.vy += GRAVITY * dt;
  player.y  += player.vy * dt;

  int playerH = player.ducking ? PLAYER_H_DUCK : PLAYER_H_NORM;
  float floorY = GROUND_Y - playerH;

  if (player.y >= floorY) {
    player.y = floorY;
    player.vy = 0;
    player.onGround = true;
  }

  // --- Dificultad progresiva ---
  gameSpeed += 0.0015f * dt;
  if (spawnInterval > 750) spawnInterval -= (unsigned long)(0.4f * dt);

  // --- Scroll del piso (efecto visual) ---
  groundScrollX -= (int)(gameSpeed * dt);
  if (groundScrollX <= -8) groundScrollX += 8;

  // --- Generar obstáculos ---
  if (millis() - lastSpawnTime > spawnInterval) {
    spawnObstacle();
    lastSpawnTime = millis();
  }

  // --- Mover obstáculos ---
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (!obstacles[i].active) continue;
    obstacles[i].x -= gameSpeed * dt;

    if (!obstacles[i].scored && obstacles[i].x + obstacles[i].w < PLAYER_X) {
      obstacles[i].scored = true;
      score += 10;
    }
    if (obstacles[i].x < -30) obstacles[i].active = false;
  }

  // --- Mover monedas ---
  for (int i = 0; i < MAX_COINS; i++) {
    if (!coins[i].active) continue;
    coins[i].x -= gameSpeed * dt;
    if (coins[i].x < -10) coins[i].active = false;

    // Colisión jugador-moneda (recolección)
    float px = PLAYER_X, py = player.y;
    if (coins[i].x + 6 > px && coins[i].x - 6 < px + PLAYER_W &&
        coins[i].y + 6 > py && coins[i].y - 6 < py + playerH) {
      coins[i].active = false;
      score += 25;
    }
  }

  // El puntaje también sube con la distancia recorrida
  static float distAccum = 0;
  distAccum += gameSpeed * dt;
  if (distAccum >= 20.0f) {
    distAccum = 0;
    score += 1;
  }
}

// =====================================================================
//  GENERAR OBSTÁCULOS
// =====================================================================
void spawnObstacle() {
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (obstacles[i].active) continue;

    obstacles[i].active = true;
    obstacles[i].scored = false;
    obstacles[i].x = SCREEN_W + 10;

    // 30% de probabilidad de que sea un pájaro volador (hay que agacharse)
    if (random(0, 100) < 30) {
      obstacles[i].type = OBST_BIRD;
      obstacles[i].w = 14;
      obstacles[i].h = 10;
      obstacles[i].flyY = GROUND_Y - PLAYER_H_NORM - random(0, 8);
    } else {
      obstacles[i].type = OBST_PIPE;
      obstacles[i].w = 14;
      obstacles[i].h = random(14, 26);
    }

    spawnCoinMaybe(obstacles[i].x + 20, obstacles[i].h);
    break;
  }
}

void spawnCoinMaybe(float x, int h) {
  if (random(0, 100) < 55) {
    for (int i = 0; i < MAX_COINS; i++) {
      if (coins[i].active) continue;
      coins[i].active = true;
      coins[i].x = x;
      coins[i].y = GROUND_Y - 26 - random(0, 20);
      break;
    }
  }
}

// =====================================================================
//  COLISIONES (AABB simple)
// =====================================================================
bool checkCollision() {
  int playerH = player.ducking ? PLAYER_H_DUCK : PLAYER_H_NORM;
  float px1 = PLAYER_X, py1 = player.y;
  float px2 = PLAYER_X + PLAYER_W, py2 = player.y + playerH;

  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (!obstacles[i].active) continue;

    float ox1, oy1, ox2, oy2;
    if (obstacles[i].type == OBST_PIPE) {
      ox1 = obstacles[i].x;
      oy1 = GROUND_Y - obstacles[i].h;
      ox2 = obstacles[i].x + obstacles[i].w;
      oy2 = GROUND_Y;
    } else {
      ox1 = obstacles[i].x;
      oy1 = obstacles[i].flyY;
      ox2 = obstacles[i].x + obstacles[i].w;
      oy2 = obstacles[i].flyY + obstacles[i].h;
    }

    if (px1 < ox2 && px2 > ox1 && py1 < oy2 && py2 > oy1) {
      return true;
    }
  }
  return false;
}

// =====================================================================
//  DIBUJO
// =====================================================================
void drawGame() {
  // Redibujo simple con "borrado" del fondo cada frame (pantalla pequeña,
  // suficientemente rápido con SPI a alta velocidad en ESP32-S3).
  tft.fillRect(0, 0, SCREEN_W, GROUND_Y, COLOR_SKY);
  drawGround();

  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (obstacles[i].active) drawObstacle(obstacles[i]);
  }
  for (int i = 0; i < MAX_COINS; i++) {
    if (coins[i].active) drawCoin(coins[i]);
  }

  drawPlayer();
  drawHUD();
}

void drawGround() {
  tft.fillRect(0, GROUND_Y, SCREEN_W, SCREEN_H - GROUND_Y, COLOR_GROUND);
  for (int x = groundScrollX; x < SCREEN_W; x += 8) {
    tft.drawFastVLine(x, GROUND_Y, 3, COLOR_GROUND2);
  }
}

void drawPlayer() {
  int h = player.ducking ? PLAYER_H_DUCK : PLAYER_H_NORM;
  int x = PLAYER_X;
  int y = (int)player.y;

  if (player.ducking) {
    // Personaje agachado (más ancho, más bajo)
    tft.fillRect(x - 1, y + 2, PLAYER_W + 2, h - 2, COLOR_BLUE);
    tft.fillRect(x, y, PLAYER_W, 4, COLOR_RED);
  } else {
    // Gorra
    tft.fillRect(x, y, PLAYER_W, 4, COLOR_RED);
    // Cara
    tft.fillRect(x + 1, y + 4, PLAYER_W - 2, 4, COLOR_SKIN);
    // Overol
    tft.fillRect(x, y + 8, PLAYER_W, h - 8, COLOR_BLUE);
    // Botones
    tft.drawPixel(x + 3, y + 10, COLOR_WHITE);
    tft.drawPixel(x + PLAYER_W - 4, y + 10, COLOR_WHITE);
  }
}

void drawObstacle(Obstacle &o) {
  int x = (int)o.x;
  if (o.type == OBST_PIPE) {
    tft.fillRect(x, GROUND_Y - o.h, o.w, o.h, COLOR_PIPE);
    tft.fillRect(x - 1, GROUND_Y - o.h, o.w + 2, 4, COLOR_PIPE_DK);
  } else {
    // Pájaro simple: cuerpo + alas en "V"
    int y = o.flyY;
    tft.fillCircle(x + o.w / 2, y + o.h / 2, 4, COLOR_BIRD);
    tft.drawLine(x, y, x + o.w / 2, y + o.h / 2, COLOR_BLACK);
    tft.drawLine(x + o.w, y, x + o.w / 2, y + o.h / 2, COLOR_BLACK);
  }
}

void drawCoin(Coin &c) {
  tft.fillCircle((int)c.x, (int)c.y, 4, COLOR_COIN);
  tft.drawCircle((int)c.x, (int)c.y, 4, COLOR_GROUND2);
}

void drawHUD() {
  tft.fillRect(0, 0, SCREEN_W, 12, COLOR_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_WHITE);
  tft.setCursor(2, 2);
  tft.print("Puntos: ");
  tft.print(score);

  tft.setCursor(SCREEN_W - 60, 2);
  tft.print("Max: ");
  tft.print(highScore);
}

// =====================================================================
//  PANTALLAS DE INICIO / FIN DE JUEGO
// =====================================================================
void drawStartScreen() {
  tft.fillScreen(COLOR_SKY);
  drawGround();

  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(2);
  tft.setCursor(18, 30);
  tft.print("MINI MARIO");

  tft.setTextSize(1);
  tft.setCursor(20, 60);
  tft.print("Presiona el boton");
  tft.setCursor(30, 72);
  tft.print("del joystick");

  tft.setCursor(10, 95);
  tft.setTextColor(COLOR_BLACK);
  tft.print("Abajo = agacharse");

  // Personajito de portada
  player.y = GROUND_Y - PLAYER_H_NORM;
  player.ducking = false;
  drawPlayer();
}

void drawGameOverScreen() {
  tft.fillRect(10, 30, SCREEN_W - 20, 60, COLOR_BLACK);
  tft.drawRect(10, 30, SCREEN_W - 20, 60, COLOR_WHITE);

  tft.setTextColor(COLOR_RED);
  tft.setTextSize(2);
  tft.setCursor(24, 38);
  tft.print("GAME OVER");

  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(1);
  tft.setCursor(24, 62);
  tft.print("Puntos: ");
  tft.print(score);

  tft.setCursor(24, 74);
  tft.print("Maximo: ");
  tft.print(highScore);

  tft.setCursor(18, 100);
  tft.print("Boton = reintentar");
}
