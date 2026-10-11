# Novara Sample

Novara ist eine kleine Beispielsanwendung für die Rhea Engine. Das Sample zeigt das Zusammenspiel von Rendering, Physik, Szenenverwaltung, Controllern und Übergangsbereichen in einer zusammenhängenden, begehbaren Umgebung.

## Inhalt

Das Sample besteht derzeit aus drei verbundenen Bereichen:

- **Harbour** – Hafenszene mit Strand, Pier und Wasser
- **Harbour Gate** – dauerhaft geladener Übergangsbereich
- **Town** – einfache Stadtszene

Harbour und Town werden jeweils als primäre Szene verwendet. Harbour Gate bleibt als Partition aktiv und verbindet beide Bereiche über gemeinsame Weltkoordinaten.

## Demonstrierte Funktionen

Novara demonstriert insbesondere:

- mehrere gleichzeitig aktive Szenen
- Wechsel der primären Szene
- persistente Environment- und Partition-Szenen
- Heightmap-Terrain
- Blendmaps mit vier Terrain-Texturen
- animierten Wassershader mit Normalmap
- Skybox und direktionales Licht
- Bullet-Heightmap-Collision
- kinematische Character-Steuerung
- Capsule Sweeps für Gravitation und Gelände
- Controller-Übergabe zwischen Szenen
- Areas und Transition Areas
- animierte beziehungsweise verdeckte Szenenübergänge
- GUI-Overlay und Debug-Anzeigen

## Szenenstruktur

```text
Environment
    dauerhaft aktiv
    ├── Skybox
    └── Beleuchtung

Harbour
    PrimaryScene
    ├── Terrain
    ├── Strand
    ├── Pier
    ├── Wasser
    └── Hafenobjekte

Harbour Gate
    dauerhaft aktive Partition
    ├── flacher Boden
    ├── Mauern und Gebäude
    ├── Sichtschutz
    └── TransitionArea

Town
    PrimaryScene
    ├── Terrain
    └── Stadtobjekte
```

Harbour Gate bleibt während des Wechsels zwischen Harbour und Town sichtbar und aktiv. Mauern und Gebäude verdecken den eigentlichen Szenenwechsel.

## Weltkoordinaten

Alle begehbaren Szenen verwenden gemeinsame Weltkoordinaten. Dadurch können Harbour, Harbour Gate und Town ohne lokale Koordinatenumrechnung aneinandergefügt werden.

Die Terrain- und Bodenhöhen an den Übergängen müssen übereinstimmen oder sich leicht überlappen, damit keine sichtbaren oder physikalischen Lücken entstehen.

## Szenenübergang

Die `TransitionArea` befindet sich im sichtgeschützten Teil von Harbour Gate.

Beim Betreten wird abhängig von der aktuellen PrimaryScene zwischen Harbour und Town gewechselt:

```text
Harbour → Harbour Gate → Town
Town → Harbour Gate → Harbour
```

Der Ablauf ist:

1. Der Character betritt die TransitionArea.
2. Der Übergang wird gegen erneutes Auslösen gesperrt.
3. Die Übergangsanimation beziehungsweise Bildschirmblende beginnt.
4. Nach vollständig gerendertem Übergangsbild wird die Zielszene geladen oder aktiviert.
5. Der Controller wird mit dem Character der Zielszene verbunden.
6. Position und Orientierung werden übertragen.
7. Die Zielszene wird zur PrimaryScene.
8. Die Übergangsanimation wird beendet.
9. Nach dem Verlassen der Area wird der Trigger wieder freigegeben.

## Area-System

Das Area-System besteht aus:

- `AreaService`
- `Area`
- `AreaComponent`
- `ColliderComponent`
- Novaras `TransitionArea`

`AreaComponent` markiert Items, die von Areas erkannt werden können. Der `AreaService` verwaltet aktive Areas und Targets und erkennt:

- `Enter`
- `Stay`
- `Exit`

Die räumliche Erkennung verwendet derzeit achsenparallele Boxen. Sie ist unabhängig von Bullet und erzeugt keine physische Kontaktreaktion.

## Terrain

Die Terrains verwenden 8-Bit-Graustufenbilder als Heightmaps.

Die Höhe wird berechnet als:

```text
worldHeight = grayValue / 255 × terrainHeight
```

Blendmaps verwenden die RGB-Kanäle für zusätzliche Terrain-Texturen:

```text
Schwarz → Basistextur
Rot     → Textur 2
Grün    → Textur 3
Blau    → Textur 4
```

Übergänge zwischen den Texturen entstehen durch Zwischenwerte in der Blendmap.

## Wasser

Die Wasseroberfläche verwendet ein PlaneMesh mit einem eigenen Shader.

Der Shader demonstriert:

- animierte UV-Koordinaten
- zwei bewegte Normalmap-Abtastungen
- Tangent-Space-Normalen
- diffuse Beleuchtung
- reduzierte Specular-Beleuchtung
- Alpha Blending
- deaktiviertes Schreiben in den Depth Buffer

Die Skybox wird vor dem transparenten Wasser gerendert.

## Physik

Novara verwendet Bullet als Physics-Backend.

Das Sample enthält:

- Heightfield-Collision für Terrain
- Box- und Mesh-Collider
- Capsule-Collider für den Character
- Capsule Sweeps für Gravitation
- Erkennung begehbarer und zu steiler Flächen
- Bewegung entlang von Hängen
- Collider-Übergänge zwischen mehreren Szenen

Collision Shapes und Triangle Meshes bleiben während der gesamten Lebensdauer ihres Bullet-Bodys erhalten.

## Steuerung

Der Controller wird vom `ControllerService` verwaltet und beim Wechsel der PrimaryScene mit dem Character der Zielszene verbunden.

Kontinuierliche Eingaben sollten möglichst aus dem aktuellen Tastenzustand bestimmt werden. Ein rein eventbasierter Bewegungszustand kann bei synchronen Szenenwechseln Press- oder Release-Ereignisse verlieren.

## Debugging

Die Physik-Debugdarstellung kann mit der Taste `0` ein- und ausgeschaltet werden.

Die GUI zeigt unter anderem:

- aktuellen Controller
- Weltposition
- Orientierung

## Bekannte Einschränkungen

- Szenenaktivierung kann derzeit synchron erfolgen und kurz blockieren.
- Der Übergang wird deshalb durch eine Animation oder Bildschirmblende verdeckt.
- Areas unterstützen momentan nur achsenparallele Boxen.
- Town und Harbour verwenden noch getrennte Character-Instanzen.
- Echtes Partition-Streaming ist noch nicht implementiert.
- Transparente Objekte werden noch nicht allgemein nach Kameradistanz sortiert.

## Zweck des Samples

Novara ist kein vollständiges Spiel. Es dient als vertikaler Testschnitt für die wichtigsten Systeme der Rhea Engine.

Das Sample soll zeigen, dass Rendering, Terrain, Physik, Controller, GUI und Szenenverwaltung gemeinsam in einer kleinen, interaktiven Welt verwendet werden können.