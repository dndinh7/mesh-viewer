# mesh-viewer

Implements a simple PLY viewer

https://user-images.githubusercontent.com/72237791/227417948-c5874ee1-584c-4307-bf00-78d41482576f.mp4


## How to build

*Windows*

Open git bash to the directory containing this repository.

```
mesh-viewer $ mkdir build
mesh-viewer $ cd build
mesh-viewer/build $ cmake ..
mesh-viewer/build $ start mesh-viewer.sln
```

Your solution file should contain two projects: `mesh-viewer` and `test-ply-mesh`.
To run from the git bash command shell, 

```
mesh-viewer/build $ ../bin/Debug/test-ply-mesh.exe
mesh-viewer/build $ ../bin/Debug/mesh-viewer.exe
```

*macOS*

Open terminal to the directory containing this repository.

```
mesh-viewer $ mkdir build
mesh-viewer $ cd build
mesh-viewer/build $ cmake ..
mesh-viewer/build $ make
```

To run each program from build, you would type

```
mesh-viewer/build $ ../bin/mesh-viewer
```

## Demo of basic features

Normal Shading
![normals](https://user-images.githubusercontent.com/72237791/227417854-d678e4f1-69e8-43a8-b309-eac2dbef75df.png)

Phong-Vertex Shading
![phong-vertex](https://user-images.githubusercontent.com/72237791/227417866-2bac79c3-471b-4dd8-8e9a-f1f412c750ed.png)

Phong-Pixel Shading
![phong-pixel](https://user-images.githubusercontent.com/72237791/227417862-aadf6097-905b-4c7a-9845-7d7e49c53ffa.png)

1. Orbit around the model by dragging left click
2. Zoom in and out using the scroll wheel, or by shift + left click drag
3. Change the model by press 'n' for next and 'p' for previous models
4. Change the shader by pressing 's'
5. Change the texture by pressing 't'
6. Make the light move/stop by pressing 'm'

## Unique features

Spotlight
![spotlight_apple](https://user-images.githubusercontent.com/72237791/227418252-b3291633-623d-428d-949c-e9adfe197b0d.png)

Toon Shading
![toon_apple](https://user-images.githubusercontent.com/72237791/227418292-e30afe44-acc8-4fb6-88e5-4218fcbfeba9.png)

Textures
![textured_flashlight](https://user-images.githubusercontent.com/72237791/227418352-0f04dcc4-1eae-4c16-ad4f-302f06cf1ac3.png)

Fog
![textured_toon_sphere](https://user-images.githubusercontent.com/72237791/227418317-ee432361-39c6-4493-8c47-7ecd71663c78.png)

