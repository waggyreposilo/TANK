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

## Design Decisions

**Bare-Metal, single-timer game loop**  
A Basic Timer(TIM6) is configured for a fixed period and drives both the logic and render update flags via its  
interrupt callback instead of mail loop polling the HAL_GetTick() directly. This keeps the game updates done by a  
single timer with no RTOS scheduler, since the game loop has no independent concurrent tasks which means  
the game logic only needs predictable timing to work instead of an RTOS.  

**Double-buffered rendering**  
The SH1107 driver keeps two full frames, one for drawing the updated frame(bufferDraw) and another for  
sending the data to the display(bufferTx), that swaps pointers instead of copying the data from one  
frame to another. The 128x128 frame is sent 8 pages at a time by transmitting the data via I2C DMA  
with the callback chaining to the next page automatically. A busy flag for the driver(SH1107_isBusy())  
is checked before the game logic writes the new frame, so the CPU never blocks on I2C which causes tearing  
a frame mid transfer. Game logic and enemy AI computation is done in parallel with the previous frame  
still being pushed out over I2C.

**Downsampled tile grid for BFS pathfinding**  
A tile map is created by downsampling the 128x128 pixel map into a 16x16 (TILE SIZE = 8) by majority-voting  
wall pixels per tile. With this downsampling, BFS runs over 256 tiles instead of the 16384 pixels, a  
64x reduction in search space, which keeps visited/parent/queue arrays small enough to for allocations    
per enemy tank. 

**Finite State Machine(FSM) enemy AI: PATROL <-> SHOOT**  
Each enemy is run by a FSM ('ENEMY_PATROL'/'ENEMY_SHOOT') where the state changes if the player tank is  
in the enemy tank's line of sight(isLineofSight()). In PATROL, the enemy follows a BFS path to a randomly  
chosen walkable tile (picked via a 16-bit Galoi's LFSR, not 'rand()', to avoid pulling in a heavier standard  
library RNG) and re-plans whenever the destination is blocked by a wall. Whenever the line of sight function  
returns true, the enemy switches to SHOOT and fires on a cooldown; loosing sight of the player sets it back to  
PATROL.

**Bresenham-based line-of-sight**   
Instead of reusing the SH1107 display's line drawing function, the Bresenham's line algorithm was reimplemented      
specifically for wall inclusion check between the enemy and player tank (checkWallsH()/checkWallsV()). In this 
implementation, the line samples the map buffer(gameObject->map) at each stepped pixel so it walks with the same  
line against the map buffer directly. Direction axis are also implemented to check if the current position of the   
enemy tank is facing the player tank directly before implementing the line checking algorithm.

**AABB collision**   
Tank-to-tank and bullet-to-tank checks use simple bounding box overlap tests against a fixed 8x8 tank and  
small bullet dimensions. AABB is accurate enough for the gameplay while having fast execution. This logic is  
implemented for each bullet against tank, per logic tick. 
 
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
