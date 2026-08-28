#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <math.h>

constexpr int TFT_SCLK=12,TFT_MOSI=11,TFT_CS=10,TFT_DC=9,TFT_RST=8,TFT_BL=7;
constexpr int JOY_X=1,JOY_Y=2,JOY_SW=4;
constexpr bool INVERT_X=false,INVERT_Y=false;
Adafruit_ST7735 tft(&SPI,TFT_CS,TFT_DC,TFT_RST);

constexpr int SCREEN_W=160,SCREEN_H=128,VIEW_H=108,MAP_W=16,MAP_H=16,RAY_STEP=2;
const char WORLD[MAP_H][MAP_W+1]={
 "1111111111111111","1000000000000001","1022200011100001","1020000010000301",
 "1020111010000301","1000100010000001","1110100011110101","1000100000000101",
 "1011110111100101","1000000100000101","1044400101111101","1000400100000001",
 "1100400111101101","10000000000000X1","1000011110000001","1111111111111111"};

struct Enemy{float x,y;int hp;uint32_t lastAttack;bool alive;};
constexpr int ENEMY_COUNT=7;
const float ENEMY_START[ENEMY_COUNT][2]={{6.5f,2.5f},{12.5f,3.5f},{6.5f,7.5f},{2.5f,9.5f},{12.5f,9.5f},{4.5f,13.5f},{12.5f,13.5f}};
Enemy enemies[ENEMY_COUNT];
float px=2.5f,py=1.8f,dirX=0,dirY=1,planeX=-.66f,planeY=0,zBuffer[SCREEN_W];
int joyCenterX=2048,joyCenterY=2048,health=100,kills=0,muzzleFrames=0;
bool won=false,dead=false,oldButton=false; uint32_t lastFrame=0,lastShot=0;

uint16_t darken(uint16_t c){return ((((c>>11)&31)>>1)<<11)|((((c>>5)&63)>>1)<<5)|((c&31)>>1);}
uint16_t wallColor(char c){
 if(c=='2')return tft.color565(38,115,160);if(c=='3')return tft.color565(46,145,67);
 if(c=='4')return tft.color565(135,76,38);if(c=='X')return tft.color565(20,220,90);
 return tft.color565(150,42,35);
}
bool isWall(float x,float y){int a=(int)x,b=(int)y;if(a<0||a>=MAP_W||b<0||b>=MAP_H)return true;char c=WORLD[b][a];return c!='0'&&c!='X';}
bool canStand(float x,float y){constexpr float r=.18f;return !isWall(x-r,y-r)&&!isWall(x+r,y-r)&&!isWall(x-r,y+r)&&!isWall(x+r,y+r);}

void resetGame(){
 px=2.5f;py=1.8f;dirX=0;dirY=1;planeX=-.66f;planeY=0;health=100;kills=0;won=dead=false;
 for(int i=0;i<ENEMY_COUNT;i++)enemies[i]={ENEMY_START[i][0],ENEMY_START[i][1],2,0,true};
}
void calibrateJoystick(){
 tft.fillScreen(ST77XX_BLACK);tft.setTextColor(ST77XX_WHITE);tft.setTextSize(1);tft.setTextWrap(false);
 tft.setCursor(18,47);tft.print("CENTRA EL JOYSTICK");tft.setCursor(35,62);tft.print("Calibrando...");
 long sx=0,sy=0;for(int i=0;i<80;i++){sx+=analogRead(JOY_X);sy+=analogRead(JOY_Y);delay(10);}joyCenterX=sx/80;joyCenterY=sy/80;
}
float axis(int pin,int center,bool invert){
 int d=analogRead(pin)-center;constexpr int deadZone=350;if(abs(d)<deadZone)return 0;
 float v=d>0?(float)(d-deadZone)/(4095-center-deadZone):(float)(d+deadZone)/(center-deadZone);
 v=constrain(v,-1.0f,1.0f);return invert?-v:v;
}
void rotatePlayer(float a){float o=dirX;dirX=dirX*cosf(a)-dirY*sinf(a);dirY=o*sinf(a)+dirY*cosf(a);o=planeX;planeX=planeX*cosf(a)-planeY*sinf(a);planeY=o*sinf(a)+planeY*cosf(a);}
void updatePlayer(float dt){
 float turn=axis(JOY_X,joyCenterX,INVERT_X),move=-axis(JOY_Y,joyCenterY,INVERT_Y);rotatePlayer(turn*2.25f*dt);
 float s=move*2*dt,nx=px+dirX*s,ny=py+dirY*s;if(canStand(nx,py))px=nx;if(canStand(px,ny))py=ny;
 if(WORLD[(int)py][(int)px]=='X'&&kills==ENEMY_COUNT)won=true;
}
void updateEnemies(uint32_t now,float dt){
 for(auto&e:enemies){if(!e.alive)continue;float dx=px-e.x,dy=py-e.y,d=sqrtf(dx*dx+dy*dy);
  if(d<.62f){if(now-e.lastAttack>650){health-=8;e.lastAttack=now;if(health<=0){health=0;dead=true;}}}
  else if(d<5.5f){float s=.48f*dt,nx=e.x+dx/d*s,ny=e.y+dy/d*s;if(canStand(nx,e.y))e.x=nx;if(canStand(e.x,ny))e.y=ny;}}
}
void shoot(){
 uint32_t now=millis();if(now-lastShot<250||dead||won)return;lastShot=now;muzzleFrames=3;int best=-1;float bestD=1000;
 for(int i=0;i<ENEMY_COUNT;i++){Enemy&e=enemies[i];if(!e.alive)continue;float ex=e.x-px,ey=e.y-py,f=ex*dirX+ey*dirY,s=fabsf(ex*dirY-ey*dirX);
  if(f>0&&f<bestD&&s<.16f+f*.045f){bool clear=true;for(float d=.15f;d<f;d+=.12f)if(isWall(px+dirX*d,py+dirY*d)){clear=false;break;}if(clear){best=i;bestD=f;}}}
 if(best>=0&&--enemies[best].hp<=0){enemies[best].alive=false;kills++;}
}

void renderWorld(){
 // writeFillRect no abre otra transaccion SPI. Usar fillRect aqui bloquearia
 // el ESP32 porque startWrite ya mantiene la transaccion abierta.
 tft.startWrite();tft.writeFillRect(0,0,SCREEN_W,VIEW_H/2,tft.color565(34,35,48));tft.writeFillRect(0,VIEW_H/2,SCREEN_W,VIEW_H/2,tft.color565(45,39,35));
 for(int x=0;x<SCREEN_W;x+=RAY_STEP){float camera=2.0f*x/SCREEN_W-1,rx=dirX+planeX*camera,ry=dirY+planeY*camera;
  int mx=(int)px,my=(int)py,stx,sty,side=0;float dx=rx==0?1e30f:fabsf(1/rx),dy=ry==0?1e30f:fabsf(1/ry),sx,sy;
  if(rx<0){stx=-1;sx=(px-mx)*dx;}else{stx=1;sx=(mx+1-px)*dx;}if(ry<0){sty=-1;sy=(py-my)*dy;}else{sty=1;sy=(my+1-py)*dy;}
  char hit='1';for(int n=0;n<40;n++){if(sx<sy){sx+=dx;mx+=stx;side=0;}else{sy+=dy;my+=sty;side=1;}hit=WORLD[my][mx];if(hit!='0')break;}
  float d=max(side==0?sx-dx:sy-dy,.08f);for(int i=x;i<min(x+RAY_STEP,SCREEN_W);i++)zBuffer[i]=d;
  int h=(int)(VIEW_H/d),y0=max(0,VIEW_H/2-h/2),y1=min(VIEW_H-1,VIEW_H/2+h/2);uint16_t c=wallColor(hit);if(side)c=darken(c);
  float wh=side==0?py+d*ry:px+d*rx;if(((int)(wh*8))&1)c=darken(c);tft.writeFillRect(x,y0,RAY_STEP,y1-y0+1,c);
 }tft.endWrite();
}
void renderEnemies(){
 int order[ENEMY_COUNT];float ds[ENEMY_COUNT];for(int i=0;i<ENEMY_COUNT;i++){order[i]=i;float x=enemies[i].x-px,y=enemies[i].y-py;ds[i]=x*x+y*y;}
 for(int i=0;i<ENEMY_COUNT-1;i++)for(int j=i+1;j<ENEMY_COUNT;j++)if(ds[order[i]]<ds[order[j]]){int q=order[i];order[i]=order[j];order[j]=q;}
 for(int oi=0;oi<ENEMY_COUNT;oi++){Enemy&e=enemies[order[oi]];if(!e.alive)continue;float sx=e.x-px,sy=e.y-py,id=1/(planeX*dirY-dirX*planeY);
  float tx=id*(dirY*sx-dirX*sy),ty=id*(-planeY*sx+planeX*sy);if(ty<=.1f)continue;int center=(SCREEN_W/2)*(1+tx/ty),h=min(abs((int)(VIEW_H/ty)),VIEW_H*2),w=h/2;
  int x0=max(0,center-w/2),x1=min(SCREEN_W-1,center+w/2),y0=max(0,VIEW_H/2-h/2),y1=min(VIEW_H-1,VIEW_H/2+h/2);
  for(int x=x0;x<=x1;x++){if(ty>=zBuffer[x])continue;float u=(float)(x-(center-w/2))/max(1,w);for(int y=y0;y<=y1;y++){float v=(float)(y-(VIEW_H/2-h/2))/max(1,h);
   bool body=(v<.27f&&fabsf(u-.5f)<.28f)||(v>=.27f&&v<.75f&&fabsf(u-.5f)<.46f)||(v>=.75f&&(fabsf(u-.28f)<.18f||fabsf(u-.72f)<.18f));if(!body)continue;
   uint16_t c=v<.27f?tft.color565(188,63,42):tft.color565(115,20,24);if(v<.2f&&((u>.34f&&u<.43f)||(u>.57f&&u<.66f)))c=ST77XX_YELLOW;tft.drawPixel(x,y,c);}}
 }
}
void renderWeapon(){int c=SCREEN_W/2;tft.fillTriangle(c-24,VIEW_H,c-12,91,c-5,VIEW_H,tft.color565(55,55,58));tft.fillTriangle(c+24,VIEW_H,c+12,91,c+5,VIEW_H,tft.color565(55,55,58));tft.fillRect(c-12,88,24,20,tft.color565(75,76,82));tft.fillRect(c-6,78,12,22,tft.color565(105,108,115));if(muzzleFrames>0){tft.fillTriangle(c,70,c-8,82,c+8,82,ST77XX_YELLOW);muzzleFrames--;}tft.drawFastHLine(c-4,VIEW_H/2,9,ST77XX_WHITE);tft.drawFastVLine(c,VIEW_H/2-4,9,ST77XX_WHITE);}
void renderHud(){
 tft.fillRect(0,VIEW_H,SCREEN_W,SCREEN_H-VIEW_H,tft.color565(18,18,20));tft.drawFastHLine(0,VIEW_H,SCREEN_W,tft.color565(120,25,22));tft.setTextSize(1);tft.setTextWrap(false);
 tft.setCursor(3,113);tft.setTextColor(health>25?ST77XX_GREEN:ST77XX_RED);tft.print("VIDA ");tft.print(health);tft.setCursor(70,113);tft.setTextColor(ST77XX_WHITE);tft.print("BAJAS ");tft.print(kills);tft.print('/');tft.print(ENEMY_COUNT);
 if(kills==ENEMY_COUNT){tft.setCursor(3,121);tft.setTextColor(ST77XX_YELLOW);tft.print("BUSCA LA SALIDA VERDE");}
}
void renderEnd(){tft.fillScreen(dead?tft.color565(55,0,0):tft.color565(0,45,20));tft.setTextColor(ST77XX_WHITE);tft.setTextSize(2);tft.setCursor(dead?41:35,35);tft.print(dead?"MUERTO":"GANASTE");tft.setTextSize(1);tft.setCursor(31,72);tft.print("Pulsa para reiniciar");}

void setup(){
 Serial.begin(115200);pinMode(TFT_BL,OUTPUT);digitalWrite(TFT_BL,HIGH);pinMode(JOY_SW,INPUT_PULLUP);
 analogReadResolution(12);analogSetPinAttenuation(JOY_X,ADC_11db);analogSetPinAttenuation(JOY_Y,ADC_11db);
 SPI.begin(TFT_SCLK,-1,TFT_MOSI,TFT_CS);tft.initR(INITR_BLACKTAB);tft.setRotation(1);calibrateJoystick();resetGame();lastFrame=millis();Serial.println("DOOM-like listo");
}
void loop(){
 uint32_t now=millis();if(now-lastFrame<33)return;float dt=min((now-lastFrame)/1000.0f,.08f);lastFrame=now;bool button=digitalRead(JOY_SW)==LOW;
 if(dead||won){renderEnd();if(button&&!oldButton){resetGame();delay(120);}oldButton=button;return;}
 if(button&&!oldButton)shoot();oldButton=button;updatePlayer(dt);updateEnemies(now,dt);renderWorld();renderEnemies();renderWeapon();renderHud();
}
