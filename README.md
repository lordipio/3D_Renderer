# 🔷 3D Renderer in C (with SDL2)

A 3D rendering engine written in pure C, using the SDL2 library for display. This project demonstrates how to render 3D models from scratch without relying on hardware acceleration (OpenGL, DirectX, Vulkan).
## ✨ Features

- ✅ **3D Model Loading**
  - Supports `.obj` file format
  - Texture mapping via `.png`

- 🧱 **Rendering Modes**
  - Wireframe
  - Solid color fill
  - Textured mapping
  - Vertex & triangle overlays
  - Configurable via keyboard at runtime

- 📦 **Custom Math Library**
  - Vector & Matrix operations
  - Perspective projection matrix
  - View matrix (look-at camera)
  - Frustum culling and clipping (6 planes)

- 💡 **Lighting**
  - Directional light

- 🎮 **Camera System**
  - Fly-through style movement
  - Yaw & pitch rotation
  - WASD + Arrow keys for full 6DOF control

- 🎨 **Pixel-Based Rendering**
  - Custom depth buffer (z-buffer)
  - Texture interpolation with UV mapping
  - Manual rasterization for triangle filling

- 🌌 **Extras**
  - Grid overlay for spatial reference
  - Adjustable pixelation factor for retro effect

---


## 🖼️ Rendering Modes Preview

| Rendering Mode                  | Screenshot |
|--------------------------------|------------|
| 🧱 **Wireframe**                | <img src="https://github.com/user-attachments/assets/bedda6eb-b9d6-4ca1-a485-790466c609a4" width="500"/> |
| 🎨 **Flat Shaded**              | <img src="https://github.com/user-attachments/assets/de160102-655c-4bc1-ba43-ae98db21e63e" width="500"/> |
| 🧊 **Textured**                 | <img src="https://github.com/user-attachments/assets/6d32063d-003e-4ef0-a61e-5a0c65c5ecd8" width="500"/> |
| 🧵 **Textured + Wireframe**     | <img src="https://github.com/user-attachments/assets/76925cd8-d15b-4d3e-a0a8-71e13f2fe184" width="500"/> |
| 🔘 **Wireframe + Vertex Dots** | <img src="https://github.com/user-attachments/assets/921f9676-11c1-48c3-b355-42d4985ad33e" width="500"/> |

## 🎬 Demo Preview

### 🔧 Full Program Overview
> A real-time rendering demo showcasing model loading, transformations, shading modes, and camera controls.

<p align="center">
  <img src="https://github.com/user-attachments/assets/9e58baa6-f729-4d20-957c-b74989b7a697" width="640"/>
</p>

### 🕹️ Pixelated Retro Mode
> Inspired by old-school software renderers, this mode simulates low-res output with a nostalgic feel.

<p align="center">
  <img src="https://github.com/user-attachments/assets/b544a6a1-dc71-436a-aa4a-7fda469aee42" width="640"/>
</p>

### 🎥 Watch Demo on YouTube

<p align="center">
  <a href="https://www.youtube.com/watch?v=sM3g3XrXK98" target="_blank">
    <img src="https://www.iconpacks.net/icons/2/free-youtube-logo-icon-2431-thumb.png" alt="Watch on YouTube" width="100"/>
  </a>
  <br/>
  <a href="https://www.youtube.com/watch?v=sM3g3XrXK98" target="_blank"><b>Watch on YouTube</b></a>
</p>

---

<br>

## 🧭 Controls

| Key | Action |
|-----|--------|
| `W` / `S` | Move Forward / Backward |
| `A` / `D` | Strafe Left / Right |
| `Q` / `E` | Move Up / Down |
| `←` / `→` | Rotate Yaw |
| `↑` / `↓` | Rotate Pitch |
| `1-6` | Switch Render Mode |
| `C` / `X` | Toggle Backface Culling |
| `ESC` | Exit |

---

## 🛠️ Build & Run

### Requirements:
- [SDL2](https://www.libsdl.org/)
- C Compiler (GCC / MSVC)
- `upng` (for PNG loading)

### Build (Windows + Visual Studio):
1. Open the `.sln` project file
2. Build in `Release` or `Debug` mode
3. Run the executable
