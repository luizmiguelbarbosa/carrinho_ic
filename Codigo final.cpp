#include <AFMotor.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <MPU9250_asukiaaa.h>
#include <IRremote.hpp>


#define IR_RECEIVE_PIN 53
// Codigos Controle IR com repeticao de entradas
#define KEY_1   0x0C  // modo 1
#define KEY_2   0x18  // modo 2
#define KEY_3   0x5E  // modo 3
#define KEY_4   0x08  // frente
#define KEY_5   0x1C  // ré
#define KEY_6   0x5A  // direita
#define KEY_7   0x42  // esquerda
const unsigned long IR_IDLE_MS = 200; // sem repeat => parar
uint16_t UltimoCmd = 0;
unsigned long UltimoIRms = 0;
/* ======================== */

MPU9250_asukiaaa imu;

/* ---------- Pinos dos sensores --------- */
const int SensorIR_ESQ = 28;
const int SensorIR_DIR = 29;
const int SensorIR_FRE = 26;

/* ---------- LCD ------------------------ */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* ---------- Motores (Shield Adafruit) -- */
AF_DCMotor mEtraseiro(1);
AF_DCMotor mDtraseiro(2);
AF_DCMotor mDfrontal(3);
AF_DCMotor mEfrontal(4);

/* ---------- Constantes gerais ---------- */
const float g_cm       = 980.0;
const uint16_t FREQ_HZ = 200;
const float LIM_PARADO = 20.0;

/* ---------- Parâmetros de movimento ---- */
const unsigned char velAndar = 130;   // velocidade reta
const unsigned char velAndar1 = 255;   // velocidade reta no modo 2
const unsigned char velGirar = 150;   // velocidade curva
const unsigned long tPulso   = 30;    // ms ligados por pulso
const unsigned long T_IGNORE = 200;   // ms ignorando frontal pós-curva

/* ---------- Estado de controle --------- */
unsigned char modo = 1;               // 1 = seguidor, 2 = experimento, 3 = Controle remoto
bool modoInicialMostrado = false;
unsigned long tUltimaCurva = 0;
bool ultima_esquerda = false;
bool ultima_direita  = false;
bool modo3Mostrado = false;

/* ---------- MPU offset ----------------- */
float offsetY = 0;

bool lerSensorFiltrado(int pino);
void ligarMotores(char modo, unsigned char vel);
void desligarMotores();
void pulsarFrente();
void pulsarCurva(char lado);
void calibrarMPU();
void experimentoAtritoMPU();

/* --- contínuo p/ modo 3 --- */
void setParado();
void setFrente();
void setRe();
void setEsq();
void setDir();
void LidarIR();

/* ==============  SETUP  ================ */
void setup() {
  Wire.begin();
  imu.setWire(&Wire);
  imu.beginAccel();
  calibrarMPU();

  pinMode(SensorIR_ESQ, INPUT_PULLUP);
  pinMode(SensorIR_DIR, INPUT_PULLUP);
  pinMode(SensorIR_FRE, INPUT_PULLUP);

  Serial.begin(9600);
  lcd.init(); lcd.backlight();
  lcd.print("Iniciando...");
  delay(2000);

  // IR
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  setParado();
}

/* ==============  LOOP  ================= */
void loop() {
  LidarIR(); // lê controle e troca/aciona conforme modo

  /* ---------- 1. SEGUIDOR -------------- */
  if (modo == 1) {

    if (!modoInicialMostrado) {
      lcd.clear(); lcd.print("Modo: Seguidor");
      delay(2000); modoInicialMostrado = true;
    }

    bool dir  = lerSensorFiltrado(SensorIR_DIR);
    bool esq  = lerSensorFiltrado(SensorIR_ESQ);
    bool fre  = lerSensorFiltrado(SensorIR_FRE);
    unsigned long agora = millis();

    /* ---- Início da curva esquerda ---- */
    if (!esq && !fre && dir) {
      pulsarCurva('L');
      tUltimaCurva = agora; ultima_esquerda = true; ultima_direita = false;
      return;
    }

    /* ---- Início da curva direita ----- */
    if (esq && !fre && !dir) {
      pulsarCurva('R');
      tUltimaCurva = agora; ultima_direita = true; ultima_esquerda = false;
      return;
    }

    /* ---- Dentro da janela de exclusão */
    if (agora - tUltimaCurva < T_IGNORE) return;

    /* ---- Seguir em frente ------------ */
    if (!esq && fre && !dir) {
      pulsarFrente();
      ultima_direita = ultima_esquerda = false;
      return;
    }

    /* ---- Linha perdida --------------- */
    if (!ultima_direita && !ultima_esquerda) {
      lcd.clear(); lcd.print("PERDIDO -> FRENTE");
      pulsarFrente();
    } else if (ultima_esquerda) {
      pulsarCurva('L');
    } else {
      pulsarCurva('R');
    }

    delay(5);  // repouso mínimo
  }

  /* ---------- 2. EXPERIMENTO ATRITO ---- */
  else if (modo == 2) {
    lcd.clear(); lcd.print("Modo: Atrito");
    experimentoAtritoMPU();
    modo = 1; modoInicialMostrado = false;
  }

  /* ---------- 3. CONTROLE IR CONTINUO -- */
  else if (modo == 3) {
    if (!modo3Mostrado) {
      lcd.clear(); lcd.print("Modo: IR ");
      delay(1200); modo3Mostrado = true;
    }
    // Se parar de receber repeats por IR_IDLE_MS, freia
    if (millis() - UltimoIRms > IR_IDLE_MS) setParado();
    delay(2);
  }
}

/* ============ FILTRO SENSOR ============ */
bool lerSensorFiltrado(int pino) {
  int ok = 0;
  for (int i = 0; i < 6; i++) { ok += digitalRead(pino); delay(1); }
  return ok >= 3;
}

/* ============ MOVIMENTO ================ */
void ligarMotores(char modoMov, unsigned char vel) {
  mEfrontal.setSpeed(vel);  mEtraseiro.setSpeed(vel);
  mDfrontal.setSpeed(vel);  mDtraseiro.setSpeed(vel);

  switch (modoMov) {
    case 'F': // frente
      mEfrontal.run(FORWARD); mEtraseiro.run(FORWARD);
      mDfrontal.run(FORWARD); mDtraseiro.run(FORWARD); break;
    case 'B': // ré (adicionado p/ modo 3)
      mEfrontal.run(BACKWARD); mEtraseiro.run(BACKWARD);
      mDfrontal.run(BACKWARD); mDtraseiro.run(BACKWARD); break;
    case 'L': // curva esquerda
      mEfrontal.run(BACKWARD); mEtraseiro.run(BACKWARD);
      mDfrontal.run(FORWARD);  mDtraseiro.run(FORWARD); break;
    case 'R': // curva direita
      mDfrontal.run(BACKWARD); mDtraseiro.run(BACKWARD);
      mEfrontal.run(FORWARD);  mEtraseiro.run(FORWARD); break;
  }
}
void desligarMotores() {
  mEfrontal.run(RELEASE); mEtraseiro.run(RELEASE);
  mDfrontal.run(RELEASE); mDtraseiro.run(RELEASE);
}

/* -- passo reto -- */
void pulsarFrente() {
  ligarMotores('F', velAndar); delay(tPulso);
  desligarMotores();           delay(5);
}

void pulsarFrente1() {
  ligarMotores('F', velAndar1);  
}

/* -- passo de curva -- */
void pulsarCurva(char lado) {          // 'L' ou 'R'
  ligarMotores(lado, velGirar); delay(tPulso);
  desligarMotores();            delay(5);
}

/* ============ CONTÍNUO (modo 3) ======== */
void setParado() { desligarMotores(); }
void setFrente() { ligarMotores('F', velAndar); lcd.clear(); lcd.print("FRENTE "); }
void setRe()     { ligarMotores('B', velAndar); lcd.clear(); lcd.print("RE "); }
void setEsq()    { ligarMotores('L', velGirar); lcd.clear(); lcd.print("ESQ "); }
void setDir()    { ligarMotores('R', velGirar); lcd.clear(); lcd.print("DIR "); }

/* ============ IR =============== */ //trecho feito com ajuda da IA pois nao sabiamos como fazer a repeticao de entrada dar um movimento continuo
void LidarIR() {
  if (!IrReceiver.decode()) return;

  UltimoIRms = millis();

  // Se for repeat NEC, a lib marca flag; reaproveita o último comando
  uint16_t cmd = IrReceiver.decodedIRData.command;
  if ((IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) && UltimoCmd != 0) {
    cmd = UltimoCmd;
  } else {
    UltimoCmd = cmd;
  }

  // Mudar modos
  if (cmd == KEY_1) { modo = 1; modoInicialMostrado = false; modo3Mostrado = false; setParado(); }
  else if (cmd == KEY_2) { modo = 2; modo3Mostrado = false; setParado(); }
  else if (cmd == KEY_3) { modo = 3; modo3Mostrado = false; /* setParado opcional */ }
  else if (modo == 3) {
    // Controles no modo 3 (contínuos enquanto houver repeat)
    if (cmd == KEY_4)       setFrente();
    else if (cmd == KEY_5)  setRe();
    else if (cmd == KEY_6)  setDir();
    else if (cmd == KEY_7)  setEsq();
  }

  IrReceiver.resume();
}.       // fim do trecho feito por IA

/* ============ MPU / ATRITO ============= */
void calibrarMPU() {
  long soma = 0; const int N = 500;
  for (int i = 0; i < N; i++) { imu.accelUpdate();
    soma += imu.accelY() * 100; delay(2); }
  offsetY = (float)soma / N;
}

void experimentoAtritoMPU() {
  lcd.clear(); lcd.print("Acelerando...");
  pulsarFrente1(); delay(1000); desligarMotores(); delay(50);

  lcd.clear(); lcd.print("Medindo atrito");
  unsigned long dt = 1000 / FREQ_HZ;
  float somaAbsA = 0; unsigned long amostras = 0, tParado = 0;

  while (true) {
    imu.accelUpdate();
    float aY = imu.accelY() * 100 - offsetY;
    somaAbsA += fabs(aY); amostras++;

    if (fabs(aY) < LIM_PARADO) {
      if (!tParado) tParado = millis();
      if (millis() - tParado > 500) break;
    } else tParado = 0;

    delay(dt);
  }
  float mu = (somaAbsA / amostras) / g_cm;

  lcd.clear(); lcd.print("Atrito estimado:");
  lcd.setCursor(0, 1); lcd.print("u = "); lcd.print(mu, 3);
  delay(5000);
}