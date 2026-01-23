<h1 align="center"> Motor 2025 </h1>

<p align="center">
This project is a custom 3D game engine developed in C++ using OpenGL as the main graphics API, made for a subject in Game developement.  
It integrates several external libraries such as Assimp (for 3D model loading), DevIL (for texture management), and ImGui (for the user interface).
</p>

<p align="center">
In latest version, we've developed in more depth the UI and added a Main Menu, with a fade out effect and player name inputbox, Options Menu with diverse toggles such as vsync or colour themes.In the HUD there's a crosshair in the middle of the scene windom that has toggable colours. 
</p>

<p align="center">
🔗 <strong>GitHub Repository:</strong> <a href="https://github.com/bottzo/Motor2025/tree/UI---Ivan%2CBernat%2CMaria">https://github.com/bottzo/Motor2025/tree/UI---Ivan%2CBernat%2CMaria</a>
</p>

---

## 🎏 Team Members

- **Bernat Loza** — [GitHub: BerniF0x](https://github.com/BerniF0x) 
-Work done, Main Menu, Option Menu, testing
<img src="https://github.com/bottzo/Motor2025/blob/UI---Ivan%2CBernat%2CMaria/Engine/Assets/ReadmePhotos/Bernat.png" width="100" alt="Bernat">
- **Iván Álvarez** — [GitHub: Ivalpe](https://github.com/Ivalpe)
-Work done, Vsync, Release, Video, general fixes
<img src="https://github.com/bottzo/Motor2025/blob/UI---Ivan%2CBernat%2CMaria/Engine/Assets/ReadmePhotos/Ivan.jpg" width="100" alt="Ivan">
- **Maria Besora** — [GitHub: mariabeo](https://github.com/mariabeo)
-Work done, Main menu fade, Crosshair, extra toggles.
<img src="https://github.com/bottzo/Motor2025/blob/UI---Ivan%2CBernat%2CMaria/Engine/Assets/ReadmePhotos/Maria.jpg" width="100" alt="Maria">

---

## 🦀 Controls

| Action | Key 1 | Key 2 |
|------------|------------|------------|
| Up | Space | |
| Down | Left Ctrl | |
| Zoom | Mouse wheel | |
| Velocity ×2 | Hold Shift | |
| Free movement | Right Mouse Button | WASD |
| Orbit | Left Alt | Left Mouse Button |
| Focus | F | |
| Select | Left Mouse Button | |
| Multiple select | Shift | Left Mouse Button |
| Delete object | Backspace | |

| Gizmo Action | Key 1 | Key 2 |
|------------|------------|------------|
| Move | W | |
| Rotate | E | |
| Scale | R | |
| Toggle Coordinate System | T | |
|Option Menu | F1 | |
---

## 🐠 User Interface

### **Console**
The console logs all engine events and processes, such as:
- Loading geometry (via **ASSIMP**)
- Loading textures (via **DevIL**)
- Resource management operations
- Initialization of external libraries
- Application flow and error messages

Additionally, it includes several **interactive options**:
- **Clear:** Erases all current console messages  
- **Log filters:** Enable or disable the display of specific types of logs (info, warnings, errors)

### **New UI specifics**
-Main Menu
-Option Menu
-HUD crosshair
---

### **Configuration**
This window is divided into **five tabs**:

1. **FPS:** Displays the current frame rate and performance data.  
2. **Window:** Allows full customization of the application window:  
   - Adjust size and resolution  
   - Toggle **fullscreen** or **borderless** mode  
   - Enable/disable **resizable** window  
3. **Camera:**  
   - Adjust camera settings 
   - Reset camera settings  
   - View current **camera position**
   - Displays a summary of **camera controls**
   - Change current active camera
   - Displays current active camera 
4. **Renderer:**  
   - Enable or disable **face culling** and choose its mode  
   - Toggle **wireframe mode**  
   - Change the **background color** of the scene
   - Toggle debug visualization for AABBs, octree,raycast, zBuffer  
5. **Hardware:**  
   - Displays detailed information about the system hardware in use  

---

### **Assets Window**
A dedicated panel to manage all project resources:
- Browse assets organized
- Import new assets via drag-and-drop
- Delete assets (automatically removes associated files in Library folder)
- Visualize reference
- View asset and import settings

---

### **Hierarchy**
Displays all GameObjects in the current scene, allowing:
- Selection of scene objects
- Reparenting objects (drag to change hierarchy)
- Renaming (double click)

---

### **Inspector**
Provides detailed information and transformation options for the selected GameObject:
- **Gameobject:**
   - Set active camera (only camera)
   - Reparenting objects (list)
   - Creating empty GameObjects
   - Deleting objects
- **Gizmo:**
   - Change gizmo mode
   - World/Local 
- **Transform:** Modify **position**, **rotation**, and **scale** directly.  
  Includes a **reset option** to restore default values.  
- **Mesh:**  
  - Displays mesh data and allows **normal visualization** (per-triangle / per-face)  
  - Select any imported mesh from the Assets window   
- **Material:**  
  - Shows texture path and dimensions  
  - Preview textures with optional checker pattern  
  - Select textures from the Assets window  
- **Camera Component** (when selected):  
  - Active frustum culling
  - Toggle debug visualization for frustum culling

---

### **Toolbar**

Includes the following menu options:

- **File:**
  - Save scene
  - Load scene 
  - Exit the program  
- **View:** Show or hide any of the editor windows
  - Layout (Save, load, auto save)  
- **Cameras:**  
  - Create camera
- **Gameobjects:**
  - Create primitves
  - Add rotate component
- **Help:**
  - *GitHub documentation:* Opens the official documentation  
  - *Report a bug:* Opens `[Link to repo]/issues`  
  - *Download latest:* Opens `[Link to repo]/releases`  
  - *About:* Displays engine name, version, authors, libraries used, and MIT License  

---

## ✨ Extra features 
- **Dark and light theme**
- **Crosshair colour change**

---

## ✨ New Core Features UI Engine

### **Main Menu**
- A menu at the beginig of the load, you can choose a player name then press start, it fades out.
![MMenu](https://github.com/bottzo/Motor2025/blob/UI---Ivan%2CBernat%2CMaria/Engine/Assets/Pruebas/MainMenu.gif)


### **Options Menu**
- F1, option menu that let's you turn on/off the vsync Aand has UI options, colour change the crosshair or change the window theme.
![Menu](https://github.com/bottzo/Motor2025/blob/UI---Ivan%2CBernat%2CMaria/Engine/Assets/Pruebas/Menu.gif)

### **Crosshair**
- Crosshair in the center of scene rendered on top of everything else.
![CRosshair](https://github.com/bottzo/Motor2025/blob/UI---Ivan%2CBernat%2CMaria/Engine/Assets/Pruebas/Crosshair.gif)


## ✨ Video
- **Video** — [Video UI](https://drive.google.com/file/d/1-Fb5xuGHLuT8gNNSD-4yBmbuVm7JjmJm/view?usp=sharing) — [Video Scene](https://drive.google.com/file/d/1rT5xrdZACu9JwDBhv7i_Tp_BFSKC_qNn/view?usp=sharing)

---

<p align="center">
<sub>© Motor 2025  — Developed by Bernat Loza, Iván Álvarez, Maria Besora — MIT License</sub>
</p>
