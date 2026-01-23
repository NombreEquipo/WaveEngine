# [Nombre de Vuestro Motor] 🎮

![Language](https://img.shields.io/badge/Language-C%2B%2B-blue) ![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey) ![License](https://img.shields.io/badge/License-MIT-green)

> *Proyecto Universitario:* Asignatura de Motores de Videojuegos.

## 📖 Sobre el Proyecto
Este motor es el resultado del trabajo realizado durante el curso, partiendo de una arquitectura base proporcionada por el profesor. El objetivo principal ha sido comprender el funcionamiento interno de un Game Engine, trabajando en la gestión de recursos, renderizado y herramientas de edición.

Para esta entrega final, el equipo se ha especializado en la *implementación de un sistema completo de Interfaz de Usuario (UI)*, mejorando la interacción y la navegabilidad tanto en el editor como en el juego.

---

## 👥 El Equipo

| Miembro | Rol / Contribución Principal |
| :--- | :--- |
| *David Subirats Bonet* | Implementación del Main Menu y gestión de estados|
| *Mario Torrents Rodríguez* | Crosshair y lógica de ratón|

---

## 🔧 Core Engine Features

Aunque nuestro foco ha sido la UI, el motor cuenta con un conjunto robusto de herramientas base que hemos integrado y optimizado:

* *⚡ Configuration Panel:* Control total sobre el rendimiento (FPS), modo de ventana (Fullscreen/Borderless/Resizable) y visualización de datos de Hardware.
* *🎥 Camera & Rendering:* Sistema de cámaras flexible con soporte para Frustum Culling, Face Culling, Wireframe y depuración de Bounding Boxes (AABB, Octree).
* *📦 Asset Management:* Ventana de Assets dedicada con funcionalidad Drag & Drop, gestión de recursos y eliminación automática de librerías vinculadas.
* *🏗️ Scene Workflow:*
    * *Hierarchy:* Organización visual de GameObjects con sistema de reparenting.
    * *Inspector:* Manipulación detallada de componentes (Transform, Mesh, Material) y Gizmos (Local/World).
* *🎨 Customization:* Soporte para Temas (Dark/Light) y personalización del color de fondo.

---

## 🌟 Feature Destacada: UI System

Nuestro trabajo principal en este hito ha sido desarrollar un sistema de interfaz de usuario de alto nivel, intuitivo y funcional.

### 1. Main Menu
Un menú principal completamente interactivo que gestiona el flujo de entrada a la aplicación.
* Navegación fluida entre secciones.
* Gestión de estados (Start, Options, Exit).

### 2. Option Menu
Sistema de configuración integrado que permite modificar variables del motor en tiempo real desde la UI.
* Ajustes de Video y Pantalla.
* Configuración de controles y audio.

### 3. HUD & Mouse UI
Mejoras en la experiencia de usuario dentro de la ventana de juego (Game View).
* *Crosshair Dinámico:* Posibilidad de cambiar el color de la mira.
* *Gestión del Ratón:* Lógica para ocultar/mostrar y bloquear el cursor según el contexto.


---

## 📥 Instalación y Uso

### Descarga
* Descarga la ultima release
* Extrae el .zip
* Ejecuta el .exe

### Repositorio
Para ver el código fuente de esta entrega específica:
👉 https://github.com/bottzo/Motor2025/tree/UI_EngineEnjoyers

---

## 📄 Licencia
Este proyecto está bajo la licencia *MIT*. Consulta el archivo [LICENSE](LICENSE) para más detalles.