# SD2 Ticker - Cryptocurrency Price Display

A stylish cryptocurrency price ticker based on ESP8266 and a 1.54-inch IPS display, showing real-time prices for SOL, ETH, and BTC with cyberpunk-style animated borders.

![Ticker](https://github.com/RainNameless/SD2_Ticker/blob/main/SD2_Ticker%E7%A4%BA%E4%BE%8B.gif)

## 📱 Features

- **Real-time price display**: Supports SOL, ETH, and BTC
- **Cyberpunk style**: Flowing rainbow border animation, pixel fade effects
- **High-resolution display**: 1.54-inch IPS, 240×240 pixels
- **WiFi connectivity**: Supports 2.4G WiFi
- **OTA firmware upgrade**: Wireless firmware updates supported
- **USB programming**: Type-C port, CH340C chip for easy firmware upload

## 🔧 Hardware Specifications

- **Main chip**: ESP-12F (ESP8266)
- **Display**: 1.54-inch IPS, 240×240 resolution
- **Case**: Injection-molded plastic
- **Overall size**: 45×35×39mm
- **Port**: Type-C USB
- **USB chip**: CH340C

### Network Support
- **Supported**: 2.4G WiFi only
- **Not supported**: 5G WiFi, campus secondary authentication WiFi, Asus routers

## 🛠️ Development Environment

### 1. Install PlatformIO

Recommended to use PlatformIO for development:

```bash
# Install PlatformIO Core
pip install platformio

# Or use VS Code + PlatformIO extension
```

### 2. Clone the Project

```bash
git clone https://github.com/RainNameless/SD2_Ticker.git
cd SD2_Ticker
```

### 3. Install Dependencies

Dependencies are listed in `platformio.ini`:
- `TFT_eSPI@^2.5.0` - TFT display driver
- `lv_arduino@^3.0.1` - LVGL graphics library
- `ArduinoJson@^6.20.0` - JSON parser

## 📡 WiFi Configuration

### Default Settings
The project uses the following default WiFi settings (change as needed in `src/main.ino`):

```cpp
const char* ssid = "esp123";           // WiFi SSID
const char* password = "esp123123";    // WiFi password
```

### Network Requirements
- Only supports 2.4G WiFi
- Does NOT support 5G WiFi, campus secondary authentication, or Asus routers

## 🔄 Build & Upload

### Build the Project

```bash
pio run
```

### Upload Firmware

#### Method 1: Specify Port
```bash
pio run -t upload --upload-port COM3
```

#### Method 2: Auto-detect Port
```bash
pio run -t upload
```

### Monitor Serial Output

#### Method 1: PlatformIO Monitor
```bash
pio device monitor --baud 921600
# or
pio run -t monitor
```

#### Method 2: Specify Port
```bash
pio device monitor --port COM3 --baud 921600
```

#### Method 3: Other Serial Tools
- **Arduino IDE**: Tools → Serial Monitor, set baud rate to 921600
- **PuTTY**: Serial mode, baud rate 921600
- **Other serial assistants**: Select correct COM port, baud rate 921600

## 🚀 Usage Instructions

### First Use
1. Connect device to PC via Type-C cable
2. Upload firmware
3. Configure WiFi if needed
4. Re-upload or use OTA for updates
5. Device will auto-connect and display prices

### Normal Operation
- Device auto-connects to WiFi on boot
- Screen displays SOL, ETH, BTC prices
- Animated rainbow border
- Prices update every 13 seconds (animation cycle)

### Troubleshooting
1. **WiFi connection failed**: Check SSID and password
2. **Price display error**: Check network and API status
3. **No display**: Check power and firmware
4. **No serial output**: Check COM port and baud rate

## 📊 Display Layout

- **Top**: SOL price
- **Middle**: ETH price
- **Bottom**: BTC price

### Animation
- **Rainbow border**: Flowing pixel border
- **Pixel fade**: Random pixel fade and recovery
- **Price update**: Updates during black screen
- **Cycle**: 13 seconds (5s fade out + 3s black + 5s fade in)

## 🔧 Technical Details

- **Platform**: ESP8266 (NodeMCU v2)
- **Framework**: Arduino
- **Compiler**: PlatformIO
- **Serial baud rate**: 921600
- **Upload baud rate**: 921600
- **Screen rotation**: 0° (portrait)
- **Price update interval**: 13 seconds
- **API**: 103.244.88.116:4399 (`/sol`, `/eth`, `/btc`)

## 📝 Development Notes

- `src/main.ino`: Main program
- `platformio.ini`: Project config
- `.gitignore`: Git ignore rules

### Main Modules
1. **WiFi management**: Auto-connect/reconnect
2. **Price fetching**: HTTP requests for real-time prices
3. **Display control**: TFT drawing and animation
4. **Animation system**: Pixel fade and recovery

### Customization
To modify display or add features:
1. Edit `src/main.ino`
2. Adjust WiFi/API settings
3. Rebuild and upload firmware

## 📞 Support

For questions or suggestions:
- Open an issue on GitHub
- Email technical support

## 📄 License

This project is licensed under the MIT License. See LICENSE for details.

---

**Note**: Please ensure your WiFi network meets the requirements and configure connection info before use. 