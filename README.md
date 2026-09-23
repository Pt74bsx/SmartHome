<div align="center">

# 🏠 SmartHome

### Système domotique Arduino pour une maquette de maison connectée

![Arduino](https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![IoT](https://img.shields.io/badge/IoT-Smart_Home-orange?style=for-the-badge)

</div>

## 📖 Présentation

**SmartHome** pilote automatiquement plusieurs équipements d'une maquette de maison en bois. Le programme Arduino réunit des capteurs, des actionneurs et un écran LCD afin de simuler différents comportements domotiques.

Le projet a été réalisé dans le cadre du module **P216** à l'ETML. Il met en pratique la programmation embarquée, la lecture de capteurs, le pilotage de sorties numériques et PWM, ainsi que la création de scénarios interactifs.

## ✨ Fonctionnalités

- sélection de plusieurs scénarios à l'aide de deux boutons ;
- affichage du mode actif sur un écran LCD I2C ;
- commande d'une porte et d'une fenêtre avec des servomoteurs ;
- éclairage intérieur et extérieur avec des LED ;
- détection de mouvement par capteur PIR ;
- génération de signaux sonores avec un buzzer ;
- contrôle d'un ventilateur et simulation de sa distance parcourue ;
- réaction à la luminosité grâce à un capteur analogique ;
- communication série pour le suivi et le débogage ;
- gestion non bloquante de certains délais avec `millis()`.

## 🧩 Composants utilisés

| Composant | Rôle |
|---|---|
| Arduino | Exécute la logique principale |
| 2 boutons | Navigation entre les scénarios |
| Écran LCD I2C 16×2 | Affiche le mode et les informations |
| 2 servomoteurs | Actionnent la porte et la fenêtre |
| Capteur PIR | Détecte une présence ou un mouvement |
| Capteur de lumière | Mesure la luminosité ambiante |
| LED intérieure et extérieure | Simulent l'éclairage |
| Ventilateur | Simule la ventilation |
| Buzzer | Produit des alertes ou mélodies |

## 🔌 Affectation principale des broches

| Élément | Broche |
|---|---:|
| Capteur PIR | 2 |
| Buzzer | 3 |
| Bouton gauche | 4 |
| LED intérieure | 5 |
| Ventilateur PWM | 6 |
| Bouton droit | 8 |
| Servo de porte | 9 |
| Servo de fenêtre | 10 |
| LED extérieure | 13 |

L'écran LCD utilise le bus I2C à l'adresse `0x27`.

## 🎛️ Scénarios

Le programme gère plusieurs scénarios numérotés de 0 à 3. Chaque scénario combine différemment l'éclairage, les ouvrants, la ventilation, le capteur de présence et l'affichage LCD.

Les boutons permettent de changer de scénario. Un message temporaire confirme le mode choisi avant le retour à l'affichage principal.

## 🛠️ Technologies

- Arduino / langage C++ ;
- bibliothèque `Servo` ;
- bibliothèque `LiquidCrystal_I2C` ;
- entrées et sorties numériques ;
- lecture analogique ;
- commande PWM ;
- communication série.

## 🗂️ Structure du dépôt

```text
SmartHome/
├── Romain_Augusto_SmartHome_p216/
│   └── Romain_Augusto_SmartHome_p216.ino
├── doc/
│   ├── rapport du projet
│   ├── journal de travail
│   └── sauvegarde du simulateur
└── README.md
```

## 🚀 Installation

### Prérequis

- Arduino IDE ;
- carte Arduino compatible ;
- bibliothèques `Servo` et `LiquidCrystal_I2C` ;
- composants câblés conformément au montage du projet.

### Étapes

1. Clone le dépôt :

   ```bash
   git clone https://github.com/Pt74bsx/SmartHome.git
   ```

2. Ouvre le fichier `Romain_Augusto_SmartHome_p216/Romain_Augusto_SmartHome_p216.ino`.
3. Installe les bibliothèques manquantes depuis le gestionnaire de bibliothèques.
4. Sélectionne la carte et le port série.
5. Vérifie le câblage des broches.
6. Compile puis téléverse le programme.

## ▶️ Utilisation

1. Alimente la maquette.
2. Utilise les boutons gauche et droit pour parcourir les scénarios.
3. Consulte l'écran LCD pour connaître le mode actif.
4. Modifie la luminosité ou déclenche le capteur PIR pour observer les réactions.
5. Ouvre le moniteur série pour afficher les informations de débogage.

## 🧠 Compétences travaillées

- programmation événementielle sur microcontrôleur ;
- intégration de capteurs et d'actionneurs ;
- gestion d'états ;
- temporisation sans bloquer entièrement la boucle ;
- calibration de seuils ;
- conception et test d'un système embarqué ;
- documentation d'un montage physique.

## ⚠️ Sécurité et précautions

- coupe l'alimentation avant de modifier le câblage ;
- vérifie la tension admissible de chaque composant ;
- utilise une alimentation appropriée pour les moteurs ;
- adapte les seuils des capteurs à ton propre montage ;
- contrôle l'adresse I2C de l'écran avant le téléversement.

## 🔭 Améliorations possibles

- déplacer le câblage dans un schéma clair ;
- enregistrer les mesures dans le temps ;
- ajouter une interface Web ou mobile ;
- permettre la configuration des seuils ;
- séparer chaque équipement dans une classe dédiée ;
- intégrer des tests sur simulateur ;
- désactiver automatiquement le mode de débogage en production.

## 🎓 Contexte

Projet scolaire **P216** réalisé à l'ETML.

## 📄 Licence

Le code source original est distribué sous licence MIT. Consulte le fichier [LICENSE](LICENSE).

---

<div align="center">
Développé par <a href="https://github.com/Pt74bsx">Romain-Augusto</a>.
</div>
