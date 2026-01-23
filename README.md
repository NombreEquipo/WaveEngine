# Wave Engine 🌊

**Wave Engine** es un motor de videojuegos en desarrollo creado para la asignatura de **Motores de Videojuegos** en el **CITM - UPC (Terrassa)**. Este proyecto se centra en la integración de un sistema de físicas robusto y herramientas de edición que facilitan la creación de entornos interactivos.

---

## 👥 El Equipo

![Foto del equipo](https://via.placeholder.com/800x400.png?text=FOTO+DEL+EQUIPO+WAVE+ENGINE) 
*Miembros del Grupo 3*

| Miembro | Contribuciones principales |
| :--- | :--- |
| **Toni Llovera Roca** | Implementación del **Vehicle Controller** y sistema de asignación automática de **RigidBodies y Colliders** para primitivas y modelos 3D importados. |
| **Javier Gómez González** | Desarrollo del sistema de **Cámara**, arquitectura de clases de física, **Module Physics** y el componente base **Collider**. |
| **Oscar Alonso Camenforte** | Integración de la librería **Bullet Physics** y desarrollo del sistema de **Point-to-Point Constraints** configurable desde el editor. |

---

## 🛠️ Core Systems (Sistemas Base)

El motor utiliza una arquitectura modular diseñada para el desarrollo eficiente de escenas:

* **Render Engine:** Basado en **OpenGL**, encargado de la rasterización de mallas y primitivas.
* **Physics Core (Bullet):** Integración profunda de la librería Bullet para gestionar el mundo físico y las colisiones.
* **Input System (SDL):** Gestión de teclado y ratón para el control del vehículo y la navegación del editor.
* **Editor UI (ImGui):** Interfaz completa con Inspector de objetos, consola de logs y gestión de componentes en tiempo real.
* **Resource Manager:** Sistema de carga de assets (FBX/Texturas) con soporte para drag-and-drop.

---

## 🚀 High-Level System: Advanced Physics & Vehicles

Nuestro sistema de alto nivel se centra en la simulación física avanzada mediante **Bullet Physics**, permitiendo que el gameplay dependa directamente de la interacción entre cuerpos rígidos.

### Características destacadas:
1.  **Vehicle Physics:** Un controlador de vehículo que simula suspensión, tracción y dirección, permitiendo interactuar con el entorno mediante un modelo físico real.
2.  **Point-to-Point Constraints:** Sistema de restricciones que permite enlazar dos objetos (primitivas o mallas) mediante un punto de anclaje, configurable visualmente desde el Inspector.
3.  **Dynamic Colliders:** Generación automática de colisionadores (Box, Sphere, Capsule) que se ajustan al volumen del objeto al ser importado, con opción de edición manual de dimensiones.

### Demostración de funcionalidad
| Editor (Configuración) | En Juego (Simulación) |
| :--- | :--- |
| ![GIF Editor](AQUÍ_VA_EL_LINK_AL_GIF_1) | ![GIF Juego](AQUÍ_VA_EL_LINK_AL_GIF_2) |

---

## 📽️ Vídeo de Creación de Escena (Timelapse)

En el siguiente vídeo se muestra el proceso de creación de una escena, configurando el vehículo y los sistemas de constraints en el editor:

[![Watch the video](https://img.youtube.com/vi/ID_DEL_VIDEO/0.jpg)](PONER_LINK_AL_VIDEO_AQUÍ)

---

## 🔗 Enlaces del Proyecto

* **Repositorio:** [Wave Engine - Physics Group 3](https://github.com/bottzo/Motor2025/tree/Physics_Group3)
* **Última Release:** [Descargar Wave Engine v1.0](PONER_LINK_A_RELEASE_AQUÍ)
