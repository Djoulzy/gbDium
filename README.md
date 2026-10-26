
# gbDium

Prototype de jeu **Game Boy**, écrit en C avec **GBDK-2020**, inspiré du shoot'em up *Uridium* sur Commodore 64. Le jeu produit une ROM `gbDium.gb` pour la Game Boy classique (DMG), également utilisable sur Game Boy Color en mode de compatibilité.

## État du jeu

Le prototype contient un vaisseau animé avec accélération, inertie et retournement, une caméra qui suit le joueur, des tirs, cinq ennemis qui poursuivent le joueur et tirent dans sa direction, ainsi qu'une animation d'explosion des ennemis touchés.

L'écran titre original « GB-DIUM » apparaît au démarrage. Appuyer sur **Start** pour lancer le jeu. L'image source est conservée dans `assets/title.png` et son export en tuiles dans `res/title.c` / `res/title.h`.

La scène mesure **128 × 32 tuiles**, soit **1 024 × 256 pixels**, pour un écran de **160 × 144 pixels**. Il n'y a pas encore de menu, de score, de gestion des vies, de fin de partie ou de son. Les collisions des tirs ennemis avec le joueur sont détectées, mais ne déclenchent pas encore de destruction du vaisseau.

## Démarrage rapide

Prérequis : `make`, `curl`, `tar`, `shasum` ou `sha256sum`, **Python 3.9 ou plus**, et un compilateur C natif (`cc`, Clang ou GCC) pour les tests de logique. Sur macOS, installer les Command Line Tools si nécessaire avec `xcode-select --install`. SameBoy doit être installé pour le test interactif.

Depuis la racine du dépôt :

```sh
make setup       # Télécharge et vérifie GBDK 4.5.0 dans .tools/gbdk
make             # Compile gbDium.gb
make check       # Teste la logique et vérifie la ROM sans émulateur
make test        # Compile, vérifie et ouvre la ROM dans SameBoy
```

L'installation automatique choisit l'archive officielle pour **macOS Intel/Apple Silicon** ou **Linux x86_64/ARM64** et vérifie son SHA-256 avant extraction. La version **4.5.0** est fixée dans le script pour utiliser la même chaîne de compilation en local et en CI. Un second `make setup` réutilise l'installation existante. Le téléchargement initial nécessite un accès à GitHub ; les compilations suivantes fonctionnent hors ligne.

### Configurer les chemins

Le Makefile utilise `.tools/gbdk` par défaut. Pour réutiliser une installation de GBDK ou définir un emplacement particulier de SameBoy :

```sh
cp config.example.mk config.local.mk
```

Adapter ensuite `config.local.mk`, ignoré par Git :

```makefile
GBDK_HOME := /opt/gbdk
SAMEBOY_APP := /Applications/SameBoy.app
```

`GBDK_HOME` doit désigner le dossier contenant `bin/lcc` et `include/`, avec ou sans `/` final. L'ancienne variable d'environnement `GBDKDIR` reste acceptée si `GBDK_HOME` n'est pas définie. Les options peuvent également être passées directement :

```sh
make GBDK_HOME=/opt/gbdk
make test SAMEBOY_APP="$HOME/Desktop/EMU/GameBoy/SameBoy.app"
```

Sur macOS, SameBoy est détecté automatiquement dans `/Applications/SameBoy.app` ou `~/Applications/SameBoy.app`. L'application est ouverte avec `open -a`, sans Wine. Sur Linux, le script cherche `sameboy` ou `SameBoy` dans le `PATH`. Pour un exécutable situé ailleurs :

```makefile
RUNNER := /chemin/vers/sameboy
```

`RUNNER` désigne un exécutable sans arguments et prend priorité sur `SAMEBOY_APP`. Les chemins propres à une machine restent dans `config.local.mk` ; ils ne doivent pas être ajoutés au Makefile partagé. Une configuration locale existe sur la machine de développement pour l'installation actuelle de SameBoy dans `~/Desktop/EMU/GameBoy/`.

## Commandes de développement

| Commande | Effet |
| --- | --- |
| `make setup` | Installe la version fixée de GBDK dans le projet. |
| `make doctor` | Vérifie la présence de GBDK, Python et SameBoy. |
| `make` ou `make default` | Compile les sources C/assembleur de `src/` et `res/`. |
| `make -j2 check` | Compile en parallèle, teste la logique et valide la ROM. |
| `make check-logic` | Compile et exécute les tests de logique avec le compilateur C natif. |
| `make test` ou `make run` | Valide la ROM puis la charge dans SameBoy. |
| `make clean` | Efface les produits de compilation et conserve les sauvegardes. |
| `make help` | Affiche les commandes disponibles. |

Les objets sont placés dans `obj/src/` et `obj/res/`, ce qui évite les conflits entre fichiers de même nom. Les dossiers sont créés automatiquement. Une modification des sources, des en-têtes, du compilateur ou des options provoque la recompilation nécessaire ; `make test` ne fait plus de nettoyage systématique.

Par défaut, `LCCFLAGS=-debug -Wf--max-allocs-per-node50000 -Wf--opt-code-speed` active les informations de débogage et les optimisations SDCC. `gbDium.map` contient la carte mémoire et `gbDium.noi` les symboles. Pour compiler sans débogage en conservant les optimisations : `make LCCFLAGS='-Wf--max-allocs-per-node50000 -Wf--opt-code-speed'`. Le changement d'options déclenche une reconstruction.

### VS Code

`Cmd+Shift+B` lance la compilation. Le menu **Terminal → Exécuter la tâche** propose également l'installation de GBDK, la vérification de la ROM et le lancement dans SameBoy. Les tâches utilisent le même Makefile que le terminal et la CI.

Les chemins IntelliSense utilisent `.tools/gbdk/include` et les variables d'environnement `GBDK_HOME` / `GBDKDIR`. Si GBDK est configuré uniquement dans `config.local.mk`, définir aussi sa variable d'environnement pour IntelliSense, puis relancer VS Code. Clang sert à l'analyse dans l'éditeur ; **la ROM est compilée par GBDK/SDCC**.

## Contrôles et test dans SameBoy

| Bouton Game Boy | Action |
| --- | --- |
| Start | Passer l'écran titre et démarrer le jeu. |
| Croix directionnelle | Accélérer dans la direction choisie ; gauche/droite déclenchent aussi le retournement. |
| A | Tirer, sauf pendant le retournement ; six projectiles simultanés maximum. |
| Select | Remettre les vitesses à zéro ; relâcher les directions pour arrêter le vaisseau. |

Le clavier correspondant dépend de la configuration de SameBoy. Pendant la partie, B et Start n'ont actuellement pas d'action.

Après `make test`, vérifier manuellement :

1. L'affichage de l'écran titre, puis le démarrage de la scène avec Start et l'affichage du vaisseau et des ennemis.
2. Les déplacements dans les quatre directions, l'inertie et le retournement.
3. Le suivi de la caméra, le chargement du décor et les limites de la scène.
4. Les tirs maintenus avec A, les explosions et la disparition des ennemis touchés.
5. L'arrêt avec Select et une nouvelle session après réinitialisation de la ROM.

Pour déboguer, activer le mode développeur dans SameBoy et ouvrir sa console. Les commandes du débogueur sont décrites dans la [documentation officielle](https://sameboy.github.io/debugger/).

## Vérifications automatiques et CI

`make check` compile la ROM et exécute les tests du véritable code de jeu avec une simulation de la tilemap et de la mémoire OAM. Ils couvrent les collisions simultanées, les boîtes de collision, l'épuisement et la réutilisation des tirs, le masquage hors écran, les limites de déplacement, les retournements et les explosions. Le décor visible est comparé à la carte source pendant un parcours complet aller-retour et pendant **4 096 frames** de jeu simulées. Les transferts de décor sont limités à deux appels et 40 tuiles par frame, après `vsync()`.

Le contrôle de cartouche vérifie ensuite la taille de **32 Kio**, le logo Nintendo, le titre `GBDIUM`, la compatibilité DMG, le type **ROM ONLY** sans RAM cartouche, ainsi que les checksums de l'en-tête et du fichier complet. Python n'utilise aucune dépendance externe.

Le workflow [`.github/workflows/build.yml`](.github/workflows/build.yml) exécute l'installation, la compilation, les tests de logique et ces contrôles sur **Ubuntu 24.04 et macOS 15**, à chaque push, pull request ou lancement manuel. La ROM, la carte mémoire et les symboles sont disponibles comme artefacts dans GitHub Actions après une exécution réussie.

Les tests natifs exécutent la logique du jeu, mais ne reproduisent pas le processeur SM83 ni le timing du PPU. Les essais dans SameBoy restent nécessaires pour valider l'affichage et les performances sur le matériel émulé. Il n'y a pas encore de test automatisé d'émulation ou de régression visuelle.

Pour rechercher les accès mémoire invalides et les comportements indéfinis avec Clang/GCC :

```sh
make check-logic HOST_CFLAGS='-O1 -g -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer'
```

## Organisation du projet

| Chemin | Rôle |
| --- | --- |
| `src/main.c` | Initialisation graphique et boucle principale. |
| `src/player.c`, `src/player.h` | Commandes, déplacement, animations et tirs du joueur. |
| `src/mobs.c`, `src/mobs.h` | Ennemis, poursuite, tirs et explosions. |
| `src/GraphLib.c`, `src/GraphLib.h` | Caméra, défilement, entités, projectiles et collisions. |
| `src/settings.h` | Dimensions d'écran, position initiale et paramètres de défilement. |
| `res/` | Tuiles, carte et métasprites en C, compilés dans le jeu. |
| `assets/` | Sources graphiques GBTD (`.gbr`) et GBMB (`.gbm`). |
| `scripts/` | Installation GBDK, lancement SameBoy et validation de ROM. |
| `tests/` | Tests du code de jeu et simulation native des API graphiques. |
| `.vscode/` | Configuration IntelliSense et tâches de développement. |

Les ressources C sont déjà exportées : **GBTD, GBMB et Wine ne sont pas nécessaires pour compiler ou jouer**. Pour modifier les fichiers sources historiques, utiliser les éditeurs compatibles puis réexporter vers `res/`. Les anciennes cibles `make sprite` / `make map`, liées à des exécutables Windows et à des chemins locaux, ont été retirées.

## Optimisations pour la Game Boy

La boucle conserve quatre étapes par frame : joueur, ennemis, projectiles, puis synchronisation vidéo. Les fonctions qui correspondent à ces étapes restent séparées. Seuls les petits tests de visibilité, de collision et le masquage des métasprites sont intégrés avec `inline` ; l'assembleur généré a été vérifié pour confirmer l'absence d'appels à ces helpers. Les routines graphiques de GBDK restent utilisées pour les métasprites et les copies VRAM.

- **Ressources en ROM** : la carte et les tuiles sont `const`, ce qui évite leur copie dans la RAM au démarrage. Conserver ce qualificatif lors des prochains exports GBTD/GBMB.
- **Mémoire bornée** : un joueur, cinq ennemis, six tirs joueur et cinq tirs ennemis sont stockés dans des structures et tableaux statiques compacts. Les listes chaînées, les allocations dynamiques et les structures de coordonnées dupliquées ont été retirées.
- **Calculs simples** : lecture unique de `joypad()` par frame, compteurs et vitesses sur 8 bits, positions du monde et positions fixes sur 16 bits. Les animations du vaisseau utilisent des décalages au lieu de tables et de l'appel à `abs()`.
- **Boucles directes** : les tableaux sont parcourus avec des pointeurs qui avancent. Le joueur est accessible directement, sans traverser une liste puis un pointeur d'entité.
- **Collisions cohérentes** : les coordonnées du monde sont conservées jusqu'au test de visibilité ; la conversion en coordonnées OAM sur 8 bits intervient ensuite. Chaque tir parcourt ses propres cibles. Les sprites hors écran sont masqués par leur coordonnée Y.
- **Défilement borné** : la scène initiale comprend la rangée et la colonne partiellement visibles. Une colonne de 19 tuiles maximum est préparée avant `vsync()` ; seules les nouvelles lignes/colonnes sont envoyées en VRAM, puis les registres de défilement sont mis à jour. La vitesse maximale garantit au plus un changement de tuile par axe et par frame.

Ces choix suivent les [recommandations GBDK pour le code et les optimisations SDCC](https://gbdk.org/docs/api/docs_coding_guidelines.html). Le budget réserve **35 des 40 sprites matériels**. La limite de **10 sprites par ligne** s'applique toujours lorsque plusieurs objets se superposent ; aucun système de répartition/flicker n'est encore ajouté. Voir [Pan Docs : OAM](https://gbdev.io/pandocs/OAM.html).

Mesures avec GBDK 4.5.0 et les options du Makefile, avant/après la passe d'optimisation (avant restauration de l'écran titre) :

| Mesure | Avant | Après |
| --- | ---: | ---: |
| RAM statique (`_DATA` + `_INITIALIZED`, hors OAM et pile) | 5 495 octets | 182 octets |
| Réserve du tas d'allocation | 2 048 octets | 0 octet |
| ROM occupée, selon `romusage` | 15 648 octets | 11 680 octets |

Le fichier de cartouche conserve sa taille de 32 Kio. Pour consulter l'occupation actuelle après compilation :

```sh
.tools/gbdk/bin/romusage gbDium.noi -g
```

Ces mesures quantifient la mémoire libérée. Un profilage dans l'émulateur reste nécessaire pour chiffrer le temps CPU par frame. La destruction du joueur, les vies, le score, le son et la progression restent à implémenter.

## Références

- [GBDK-2020 4.5.0 et archives officielles](https://github.com/gbdk-2020/gbdk-2020/releases/tag/4.5.0)
- [Installation et premiers pas avec GBDK](https://gbdk.org/docs/api/docs_getting_started.html)
- [Documentation de la chaîne de compilation](https://gbdk.org/docs/api/docs_toolchain.html)
- [SameBoy](https://sameboy.github.io/)
