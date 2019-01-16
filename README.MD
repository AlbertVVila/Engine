# RealEngine
Game Engine developed by Albert Val during the Master Degree in Video Game Design and Development at UPC School. 
Currently under development.

![alt text](https://imgur.com/cOOPIAO.png)

## Assets
## Window
### Scene & Game
In the scene window you can visualize the editor camera, and you can visualize the geometry loaded.
You can select different gameobjects by picking them with the mouse.
To move the camera, you can use the following controls:
* **WASD** fps-like movement while **Right-clicking** and free look around 
* In and out zoom with **mouse wheel** 
* **Alt+Left** click for orbiting the model 
* **F** for focus the camera on model 
* Hold **shift** to duplicate **WASD** movement

Our engine uses ImGuizmo, so you can move selected gameobjects using the gizmo. You can select which transformation apply
by clicking on the different transformations buttons on screen. You can also use *snap* button which allows to transform an object using a regular scale customizable.
Our engine allows changing the transformation mode from world space to local space in order to ease the placement of gameobjects in the scene.

In the Game window you can visualize the main camera view without all the gizmos and the interactivity of the editor. If there is no main camera then a default not found camera texture is shown.

### Hierarchy

The hierarchy allows to visualize the structure of the different gameobjects presents in the scene. In this window you can select a gameobject to view its information. With **right-clicking** you can perform multiple operations like creating a primitive, a light, deleting the object or duplicating it.
Be aware that a primitive is created according to the current scale of the scene!!
You can also drag and drop gameobjects to another parent to change the structure of the scene.

### Inspector
The inspector shows info about the selected gameobject. You can see its name, properties and you can modify them. Besides you can add more components. For example if you want to create a camera, you can add a component camera.

### Configuration
Here you have different sections that lets you modify different visualization options. Be aware that using some options may affect performance!! The configuration is saved when exiting the game.

### Console
In the console you we'll see a list of relevant logs about importing, loading or saving files.

## More information
[Last Release Notes](https://github.com/AlbertVVila/Engine/releases),
[Github](https://github.com/AlbertVVila/Engine) and
[License](https://github.com/AlbertVVila/Engine/blob/master/LICENSE)

