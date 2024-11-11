# Half Edge

Qt application that displays triangle meshes backed by half-edge data structure.
The program supports triangle soup (`.tri`) and custom half-edge (`.hds`) files, with samples being provided.
In addition, the mesh can be subdivided using the loop subdivision technique.

## Project Structure

```plaintext
half-edge/
├── src/                   # Source code
├── assets/                # Static assets
    ├── tri                # .tri files
    ├── hds                # .hds files
├── build/                 # Generated build files
├── bin/                   # Generated executable files
├── half-edge.pro          # QMake project
└── README.md              # Project README
```

## Build

```bash
qmake
make
```

## Run

```bash
bin/half-edge <.tri or .hds file>
```

Example `.tri`:

```bash
bin/half-edge assets/tri/cube.tri
```

Example `.hds`:

```bash
bin/half-edge assets/hds/cube.hds
```

## Technologies

* **C++**: `>= C++17`
* **Qt**: `5.12.x`
* **OpenGL**: `>= 4.0`

Newer versions of Qt might work correctly, if no breaking changes that affect the application were introduced.

## Showcase

![half-edge](https://github.com/user-attachments/assets/03991a61-96ee-4e66-b433-41b3744409dc)

## Controls

| Key(s)                   | Action                             |
|--------------------------|------------------------------------|
| `(X, Y, Z)` Sliders      | Adjust the camera position         |
| `Model` ArcBall          | Rotate mesh                        |
| `Light` ArcBall          | Rotate directional light           |
| `Flat Normals` Checkbox  | Toggle per vertex/per face normals |
| `Show Vertices` Checkbox | Render spheres around vertices     |
| `Vertex Size` Slider     | Control size of vertex spheres     |
| `Subdivisions [0, 8]`    | Control current subdivision level  |

Subdivisions are computed lazily, but the computation occurs on the main thread.
If the mesh is sufficiently large, the program stalls.

## TODOs

* [ ] Parallelize subdivision computation
