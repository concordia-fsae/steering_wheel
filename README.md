### Setup Instructions
  1. Setup PlatformIO in IDE of choice (i.e. VSCode)
  2. Navigate to PlatformIO install directory (usually User_folder/.platformio)
  3. Enter `packages` directory and rename `framework-arduinoststm32-maple` to `framework-arduinoststm32-maple-bak`
  4. Clone Roger Clark's repo into the packages directory (`git clone https://github.com/rogerclarkmelbourne/Arduino_STM32.git framework-arduinoststm32-maple`)
  5. Copy `platformio-build-stm32f4.py` and `platformio-build-stm32f1.py` from `framework-arduinoststm32-maple-bak/tools` to `framework-arduinoststm32-maple/tools`
  6. Compile successfully (hopefully)
