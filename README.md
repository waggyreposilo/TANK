# TANK
A real time arcade-style game written in C for STM32U585 microcontroller and a SH1107 OLED Display using a bare metal approach **(NO RTOS, just one game loop using timer only)** .

---

# GAME FEATURES
The current version of the game does the following 
- Player movement and shooting
- Working enemy AI that does patrol and shooting
- Simple Animations and effects
- real time hardware input and graphics rendering

---

# IMPLEMENTATIONS
This project is a real-time arcade game in embedded C **(no RTOS)** featuring:

- Implemented a custom I2C display driver for an SH1107 OLED with double-buffering and DMA-driven page transfers for tear-free rendering
- Designed an enemy AI using finite state machines(patrol/shoot) with Breadth-First-Search pathfinding on a downsampled tile grid for real-time obstacle navigation
- Built in line of sight detection using Bresenham's line algorithm for enemy targeting logic
- Integrated ADC based joystick input via DMA and debounced button FSM for responsive controls
- Managed collision detection(bullet-wall, tank-to-wall,tank-to-tank) and shared bullet pool between player and enemy tanks for efficient memory use

More features and fixes to be added in the future

---

# HARDWARE
- **Microcontroller:** STM32U585CI (Cortex-M33, 784 KB RAM, 2MB Flash)
- **Display:** SH1107 (128x128, monochrome, vertical display)
- **Language:** C
- **IDE:** STM32CubeIDE
- **Peripherals Used for the project:**
  - I2C (OLED Display Rendering)
  - DMA (Display Refresh)
  - ADC (Analog Joystick)
  - GPIO (Push Button)
 
---

# TOOLS and REFERENCES
- **Adafruit GFX Library**  
  Design ideas were inspired from this library like how the API is created and their abstraction approach during the creation of the SH1107 display driver:  
  https://github.com/adafruit/Adafruit-GFX-Library

- **Amit Patel's Red Blob Games**  
  A great source for learning the concepts of AI pathfinding algorithms used in games:  
  https://www.redblobgames.com/

- **Eric Richards's FSM blog**  
  Both Blogs helped me learn the concepts of FSM for programming AI. The code in the blog is in C# but you can apply the concepts in the C project:    
  http://richardssoftware.net/Home/Post/67  
  http://richardssoftware.net/Home/Post/68

- **Bitmap Creation tool**  
  Used while preparing sprites and graphics for the OLED display:  
  https://dot2pic.com/

- 
