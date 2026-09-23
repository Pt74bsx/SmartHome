/******************************************************************************
** PROGRAMME  sketch_jan22a.ino                                              **
**                                                                           **
** Lieu      : ETML - section informatique                                   **
** Auteur    : Romain Augusto                                                **
** Date      : 22.01.2026                                                    **
**                                                                           **
** Modifications                                                             **
**   Auteur  :                                                               **
**   Version : X.X                                                           **
**   Date    :                                                               **
**   Raisons :                                                               **
**                                                                           **
**                                                                           **
******************************************************************************/

/******************************************************************************
** DESCRIPTION                                                               **
** C'est un programme qui permet de gérer automatiquement certains           ** 
** équipements d'une SmartHome en bois.           							 **   
** 	                                                           				 **
**                                                                           **
******************************************************************************/

// ============================
// Bibliothèques
// ============================
#include <Servo.h>                        				// Moteur servo
#include <LiquidCrystal_I2C.h>            				// Bibliothèque pour écran LCD

// ============================
// Déclaration des constantes
// ============================
// Pins des boutons
const int BTN_LEFT_PIN  = 4;              				// Premier bouton
const int BTN_RIGHT_PIN = 8;              				// Second bouton

// Pins des composants
const int LED_PIN = 5;                    				// Lumière interrieur 
const int LED_EXT_PIN = 13;               				// Lumière éxterieur
const int BUZZER_PIN = 3;                 				// Buzzer (song)
const int PIR_PIN = 2;                    				// Capteur de mouvement
const int FAN_PIN = 6;                    				// Ventilateur

// Scénarios 
const int SCENARIO_MAX = 3;               				// Nombre de scénarios (0,1,2,3)

// Seuils pour le capteur de lumière
const int HIGH_LIGHT_THRESHOLD = 800;     				// Lumière à 750 pour Ouverture
const int LOW_LIGHT_THRESHOLD = 700;      				// Lumière à 650 pour fermeture 

// Seuils pour le ventilateur
const int FAN_HIGH_THRESHOLD = 800;       				// Lumière haute pour ventillo rapide
const int FAN_LOW_THRESHOLD = 400;        				// Lumière basse pour ventillo lent

// Configuration générale 
const bool DEBUGMODE = true;			  				// Affiche des print de debug
const unsigned long MODE_MESSAGE_DURATION = 2000; 		// Durée en millisecondes

// ============================
// Déclaration des variables
// ============================
// États des boutons 
bool btnLeftLastState  = HIGH;           // Dernier état du bouton gauche
bool btnRightLastState = HIGH;           // Dernier état du bouton droit

// Gestion de l'affichage du mode 
bool showModeMessage       = false;      // Indique si le message de mode est affiché
unsigned long modeMessageStart = 0;      // Horodatage du début d'affichage

// Capteur de mouvement
bool lastMotionState = LOW;              // Dernier état du capteur PIR

// Scénario et capteurs
int intScenario    = 0;                  // Scénario actif (0 à 3)
int intLightSensor = 0;                  // Valeur lue sur le capteur de lumière

// Positions des ouvrants
int intWindowPosition = 0;               // Position actuelle de la fenêtre (0 = fermée, 120 = ouverte)
int intDoorPosition   = 0;               // Position actuelle de la porte (0 = fermée, 120 = ouverte)

// Ventilateur 
int currentFanSpeed      = 0;            // Vitesse actuelle du ventilateur (valeur logique)
unsigned long lastFanUpdate = 0;         // Horodatage de la dernière mise à jour des km
float fanKm              = 0.0;          // Distance cumulée simulée du ventilateur

// ============================
// Déclaration des objets
// ============================
Servo window;                             // Servo de la fenêtre 
Servo door;                               // Servo de la porte 
LiquidCrystal_I2C lcd(0x27, 16, 2);       // Écran LCD I2C (adresse 0x27, 16 colonnes, 2 lignes)

// ============================
// Initialisation
// ============================
void setup() {
  // Boutons en entrée
  pinMode(BTN_LEFT_PIN,  INPUT_PULLUP);
  pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);

  // Sorties numériques 
  pinMode(LED_PIN,     OUTPUT);          // LED intérieure
  pinMode(LED_EXT_PIN, OUTPUT);          // LED extérieure
  pinMode(BUZZER_PIN,  OUTPUT);          // Buzzer

  // Entrée numérique
  pinMode(PIR_PIN, INPUT);               // Capteur de mouvement PIR

  // État initial des sorties 
  digitalWrite(LED_PIN, HIGH);           // LED intérieure allumée par défaut
  analogWrite(FAN_PIN, 255);             // Ventilateur arrêté (PWM inversé)

  // Initialisation des servomoteurs
  window.attach(10);                     // Servo fenêtre sur pin 10
  door.attach(9);                        // Servo porte sur pin 9
  window.write(0);                       // Fenêtre fermée au démarrage
  door.write(0);                         // Porte fermée au démarrage

  // Communication série
  Serial.begin(9600);                    // Débit série avec le PC

  // Initialisation de l'écran LCD
  lcd.init();                            // Initialisation du contrôleur LCD
  lcd.backlight();                       // Activation du rétroéclairage
  lcd.setCursor(0, 0);
  lcd.print("MODE 1");                   // Affichage du mode initial
}

// ============================
// Gestion des boutons
// ============================
void checkButtons() {
  bool btnLeftState  = digitalRead(BTN_LEFT_PIN);
  bool btnRightState = digitalRead(BTN_RIGHT_PIN);

  // Passage au scénario précédent
  if (btnLeftLastState == HIGH && btnLeftState == LOW) {
    if (intScenario > 0) {
      intScenario--;
      resetActuators();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MODE ");
      lcd.print(intScenario + 1);
      showModeMessage  = true;
      modeMessageStart = millis();
    }
  }

  // Passage au scénario suivant 
  if (btnRightLastState == HIGH && btnRightState == LOW) {
    if (intScenario < SCENARIO_MAX) {
      intScenario++;
      resetActuators();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MODE ");
      lcd.print(intScenario + 1);
      showModeMessage  = true;
      modeMessageStart = millis();
    }
  }

  // Mémorisation des états pour l'antirebond
  btnLeftLastState  = btnLeftState;
  btnRightLastState = btnRightState;
  delay(50);                             // Délai de stabilisation
}

// ============================
// Boucle principale
// ============================
void loop() {
  checkButtons();                        // Lecture des boutons et changement de scénario

  // Débogage : vitesse du ventilateur
  if (DEBUGMODE) {
    Serial.print("Fan speed : ");
    Serial.println(currentFanSpeed);
  }

  // Masquage automatique du message de mode après délai
  if (showModeMessage) {
    if (millis() - modeMessageStart > MODE_MESSAGE_DURATION) {
      showModeMessage = false;           // Fin d'affichage du message
    }
  }

  // Sélection du scénario actif
  switch (intScenario) {
    case 0: scenario1(); break;
    case 1: scenario2(); break;
    case 2: scenario3(); break;
    case 3: scenario4(); break;
  }
}

// ==============================================================
// Scénario 1 — Ouverture/fermeture automatique par luminosité
// ==============================================================
void scenario1() {
  // Ventilateur arrêté par sécurité 
  analogWrite(FAN_PIN, 255);

  // Lecture du capteur de lumière
  intLightSensor = analogRead(A1);

  // Débogage : valeur du capteur 
  if (DEBUGMODE) {
    Serial.print("Scenario 1 - Capteur : ");
    Serial.println(intLightSensor);
  }

  // Ouverture si luminosité élevée 
  if (intLightSensor > HIGH_LIGHT_THRESHOLD && intWindowPosition != 120) {
    moveServo(window, intWindowPosition, 120); 									// Ouvre la fenêtre
    moveServo(door,   intDoorPosition,   120); 									// Ouvre la porte
    digitalWrite(LED_PIN, LOW);               									// Éteint la LED intérieure
    lcd.setCursor(0, 0);
    lcd.print("DZ HOUSE OUVERTE");
  }

  // Fermeture si luminosité faible 
  else if (intLightSensor < LOW_LIGHT_THRESHOLD && intWindowPosition != 0) {
    moveServo(window, intWindowPosition, 0);   									// Ferme la fenêtre
    moveServo(door,   intDoorPosition,   0);   									// Ferme la porte
    digitalWrite(LED_PIN, HIGH);              									// Allume la LED intérieure
    lcd.setCursor(0, 0);
    lcd.print("DZ HOUSE FERMEE ");
  }
}

// ===================================================
// Scénario 2 — Sécurité par détection de mouvement
// ===================================================
void scenario2() {
  // Ventilateur arrêté par sécurité
  analogWrite(FAN_PIN, 255);

  // Lecture du capteur 
  bool motionState = digitalRead(PIR_PIN); 

  // Débogage : état du capteur 
  if (DEBUGMODE) {
    Serial.print("Scenario 2 - PIR : ");
    Serial.println(motionState);
  }

  // Affichage temporaire du nom du mode
  if (showModeMessage) {
    lcd.setCursor(0, 0);
    lcd.print("MODE 2       ");
    if (millis() - modeMessageStart > MODE_MESSAGE_DURATION) {
      showModeMessage = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("DzHouse");
    }
  } else {
    lcd.setCursor(0, 0);
    lcd.print("DzHouse   ");
  }

  // Intrusion détectée
  if (motionState == HIGH) {
    if (lastMotionState == LOW) {
      lcd.setCursor(0, 1);
      lcd.print("Intrusion  ");                  // Affichage du statut
    }
    blinkLight();                                // Clignotement des LED
    softIntrusionAlarm();                        // Déclenchement de l'alarme sonore
    moveServo(window, intWindowPosition, 0);     // Ferme la fenêtre
    moveServo(door,   intDoorPosition,   0);     // Ferme la porte
  }

  // Aucun mouvement détecté 
  else {
    if (lastMotionState == HIGH) {
      lcd.setCursor(0, 1);
      lcd.print("Securisee  ");                  // Affichage du statut
    }
    noTone(BUZZER_PIN);                          // Arrêt du buzzer
    digitalWrite(LED_PIN,     LOW);              // LED intérieure éteinte
    digitalWrite(LED_EXT_PIN, LOW);              // LED extérieure éteinte
    moveServo(window, intWindowPosition, 120);   // Ouvre la fenêtre
    moveServo(door,   intDoorPosition,   120);   // Ouvre la porte
  }

  lastMotionState = motionState;                 // Mémorisation de l'état PIR
}

// ============================
// Scénario 3
// ============================
void scenario3() {
  // Seuils de luminosité pour les trois modes de ventilation 
  const int LIGHT_HIGH = 800;            // Forte luminosité
  const int LIGHT_MED  = 500;            // Luminosité moyenne
  const int LIGHT_LOW  = 200;            // Faible luminosité

  // Lecture du capteur de lumière
  intLightSensor = analogRead(A1);

  // Débogage : valeur du capteur 
  if (DEBUGMODE) {
    Serial.print("Scenario 3 - Capteur : ");
    Serial.println(intLightSensor);
  }

  int fanPWM = 255;                      // Valeur PWM (255 = arrêt, 0 = max)
  int Speed  = 0;                        // Vitesse logique pour affichage

  lcd.setCursor(0, 0);

  // Forte luminosité 
  if (intLightSensor >= LIGHT_HIGH) {
    fanPWM = 0;   											// 0 = vitesse max 
    Speed = 255;  											// Vitesse logique max
    lcd.print("Mode perf      ");
  }

  // Luminosité moyenne
  else if (intLightSensor >= LIGHT_MED) {
    fanPWM = 128;          									// Valeur intermédiaire 
    Speed = 255 - 128;     									// Vitesse logique
    lcd.print("Mode basic      ");
  }

  // Faible luminosité
  else {
    fanPWM = 200;          									// Presque arrêté
    Speed = 255 - 200;     									// Vitesse
    lcd.print("Mode echo      ");
  }
  
  // Application de la vitesse au ventilateur
  analogWrite(FAN_PIN, fanPWM);
  currentFanSpeed = Speed; 									// Met à jour la vitesse 

  // Gestion de l'éclairage selon la luminosité 
  if (intLightSensor >= LIGHT_MED) {
    digitalWrite(LED_PIN,     LOW);      					// Éteint la LED intérieure
    digitalWrite(LED_EXT_PIN, LOW);      					// Éteint la LED extérieure
  } else {
    digitalWrite(LED_PIN,     HIGH);     					// Allume la LED intérieure
    digitalWrite(LED_EXT_PIN, HIGH);     					// Allume la LED extérieure
  }

  // Affichage de la vitesse sur l'écran
  lcd.setCursor(0, 1);
  lcd.print("Speed: ");
  lcd.print(Speed);
  lcd.print("   "); 										// Espace pour écraser l'ancien texte
}

// ==========================================
// Scénario 4 — Affichage du crédit auteur
// ==========================================
void scenario4() {
  // Ventilateur arrêté par sécurité
  analogWrite(FAN_PIN, 255);
  
  // Débogage : confirmation d'entrée dans le scénario
  if (DEBUGMODE) {
    Serial.println("Scenario 4 - Mode credits actif");
  }

  // Affichage sur l'écran
  lcd.setCursor(0, 0);
  lcd.print("CREAT BY ROMAIN");
}

// ========================================================
// Fonction : Clignotement lumiere exterieure et interrieur
// ========================================================
void blinkLight() {
  static unsigned long lastTime = 0;     					// Temps precedent
  static bool ledState = LOW;            					// État LED

  if (millis() - lastTime >= 500) {      					// Toutes les 500 ms
    lastTime = millis();                 					// Mise à jour du temps
    ledState = !ledState;                					// Inverse l'état
    digitalWrite(LED_EXT_PIN, ledState); 					// Applique à la LED
    digitalWrite(LED_PIN, ledState);     					// Applique à la LED
  }
}

// ============================
// Fonction : Alarme
// ============================
void softIntrusionAlarm() {
  static unsigned long lastTime = 0;
  static int step = 0;

  unsigned long currentTime = millis();

  if (currentTime - lastTime >= 150) {
    lastTime = currentTime;

    const int notes[]  = {400, 500, 600, 500}; 				// Fréquences des notes (Hz)
    const int pauses[] = {50,  50,  50,  50};  				// Pauses entre les notes (ms)
    const int count    = 4;

    if (step % 2 == 0) {
      tone(BUZZER_PIN, notes[step / 2]);        			// Joue la note
    } else {
      noTone(BUZZER_PIN);                       			// Pause entre deux notes
    }

    step++;
    if (step >= count * 2) step = 0;           				// Reprend le motif depuis le début
  }
}

// ===========================================
// Fonction : Réinitialisation des sorties
// ===========================================
void resetActuators() {
  digitalWrite(LED_PIN, LOW);                               // LED intérieur éteinte
  digitalWrite(LED_EXT_PIN, LOW);                           // LED extérieur éteinte
  noTone(BUZZER_PIN);                                       // Arrêt du buzzer

  analogWrite(FAN_PIN, 255);      							// Ventillo
  currentFanSpeed = 0;            						    // Reset vitesse
  fanKm = 0;                      							// Reset KM

  window.write(0);                                          // Fenêtre fermée
  door.write(0);                                            // Porte fermée
}

// ===================================
// Fonction : Protection des servo 
// ===================================
void moveServo(Servo &s, int &currentPos, int targetPos) {
  if(currentPos != targetPos) {                             // On ne bouge que si nécessaire
    s.write(targetPos);
    currentPos = targetPos;
  }
}

// ================================================
//  Fonction : Calcule la vitesse du ventillo
// ================================================
void updateFanKm() {
  if (intScenario != 2) return;                				// Exécution réservée au scénario 3

  unsigned long currentTime = millis();

  if (currentTime - lastFanUpdate >= 1000) {   				// Mise à jour toutes les secondes
    lastFanUpdate = currentTime;
    fanKm += (currentFanSpeed / 255.0) * 0.05; 				// Calcul des km selon la vitesse
  }
}
