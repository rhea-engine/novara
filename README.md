# Novara Sample

Novara is a small sample application for the Rhea Engine. It demonstrates how rendering, physics, scene management, controllers, and transition areas work together in a connected, walkable environment.

## Content

The sample currently consists of three connected areas:

- **Harbour** – a harbour scene with a beach, pier, and water
- **Harbour Gate** – a permanently loaded transition area
- **Town** – a simple town scene

Harbour and Town are used as primary scenes. Harbour Gate remains active as a partition and connects both areas using shared world coordinates.

## Demonstrated Features

Novara demonstrates the following features:

- multiple simultaneously active scenes
- primary scene transitions
- persistent environment and partition scenes
- heightmap terrain
- blend maps with four terrain textures
- an animated water shader using a normal map
- a skybox and directional lighting
- Bullet heightmap collision
- kinematic character movement
- capsule sweeps for gravity and terrain collision
- controller handover between scenes
- areas and transition areas
- animated or concealed scene transitions
- a GUI overlay and debug information

## Scene Structure

```text
Environment
    permanently active
    ├── Skybox
    └── Lighting

Harbour
    PrimaryScene
    ├── Terrain
    ├── Beach
    ├── Pier
    ├── Water
    └── Harbour objects

Harbour Gate
    permanently active partition
    ├── Flat ground
    ├── Walls and buildings
    ├── Visual occlusion
    └── TransitionArea

Town
    PrimaryScene
    ├── Terrain
    └── Town objects
```

Harbour Gate remains visible and active while switching between Harbour and Town. Walls and buildings conceal the actual scene transition.

## World Coordinates

All walkable scenes use shared world coordinates. This allows Harbour, Harbour Gate, and Town to connect without converting between local coordinate systems.

Terrain and ground elevations must match or overlap slightly at their boundaries to avoid visible or physical gaps.

## Scene Transition

The `TransitionArea` is located in the visually concealed part of Harbour Gate.

Entering it switches between Harbour and Town depending on the current primary scene:

```text
Harbour → Harbour Gate → Town
Town → Harbour Gate → Harbour
```

The transition proceeds as follows:

1. The character enters the TransitionArea.
2. The transition is locked to prevent repeated activation.
3. The transition animation or screen fade begins.
4. Once the transition image has been rendered completely, the target scene is loaded or activated.
5. The controller is connected to the character in the target scene.
6. Position and orientation are transferred.
7. The target scene becomes the primary scene.
8. The transition animation ends.
9. The trigger is unlocked after leaving the area.

## Area System

The area system consists of:

- `AreaService`
- `Area`
- `AreaComponent`
- `ColliderComponent`
- Novara's `TransitionArea`

`AreaComponent` marks items that can be detected by areas. `AreaService` manages active areas and targets and detects:

- `Enter`
- `Stay`
- `Exit`

Spatial detection currently uses axis-aligned boxes. It is independent of Bullet and does not produce a physical collision response.

## Terrain

Terrains use 8-bit grayscale images as heightmaps.

Height is calculated as:

```text
worldHeight = grayValue / 255 × terrainHeight
```

Blend maps use the RGB channels for additional terrain textures:

```text
Black → Base texture
Red   → Texture 2
Green → Texture 3
Blue  → Texture 4
```

Intermediate blend-map values create smooth transitions between textures.

## Water

The water surface uses a PlaneMesh with a custom shader.

The shader demonstrates:

- animated UV coordinates
- two moving normal-map samples
- tangent-space normals
- diffuse lighting
- reduced specular lighting
- alpha blending
- disabled depth-buffer writes

The skybox is rendered before the transparent water.

## Physics

Novara uses Bullet as its physics backend.

The sample includes:

- heightfield terrain collision
- box and mesh colliders
- a capsule collider for the character
- capsule sweeps for gravity
- detection of walkable and excessively steep surfaces
- movement along slopes
- collider transitions between multiple scenes

Collision shapes and triangle meshes remain alive for the entire lifetime of their Bullet body.

## Controls

The controller is managed by `ControllerService`. When the primary scene changes, it is connected to the character in the target scene.

Continuous input should preferably be determined from the current keyboard state. A purely event-based movement state may lose press or release events during synchronous scene transitions.

## Debugging

The physics debug visualization can be toggled with the `0` key.

The GUI displays information including:

- the current controller
- world position
- orientation

## Known Limitations

- Scene activation may currently happen synchronously and briefly block execution.
- Transitions are therefore concealed with an animation or screen fade.
- Areas currently support only axis-aligned boxes.
- Town and Harbour still use separate character instances.
- True partition streaming is not yet implemented.
- Transparent objects are not yet generally sorted by camera distance.

## Purpose of the Sample

Novara is not a complete game. It is a vertical test slice for the main systems of the Rhea Engine.

The sample demonstrates that rendering, terrain, physics, controllers, GUI, and scene management can work together in a small interactive world.