# SD2 Ticker - 加密货币价格显示器

一个基于ESP8266的1.54寸IPS屏幕加密货币价格显示器，实时显示SOL、ETH、BTC价格，具有赛博朋克风格的动态边框动画效果。

TG ：t.me/SD2_Ticker

![示例图片](https://github.com/RainNameless/SD2_Ticker/blob/main/SD2_Ticker%E7%A4%BA%E4%BE%8B.gif)



## 📱 产品特性

- **实时价格显示**: 目前支持SOL、ETH、BTC三种主流加密货币价格
- **赛博朋克风格**: 彩虹色流动边框动画，像素消失效果
- **高分辨率显示**: 1.54寸IPS屏幕，240×240分辨率
- **WiFi连接**: 支持2.4G WiFi网络连接
- **USB烧录**: Type-C接口，CH340C芯片支持USB固件烧录

## 🔧 硬件规格

### 核心配置
- **主控芯片**: ESP-12F (ESP8266)
- **显示屏**: 1.54寸IPS屏幕
- **分辨率**: 240×240像素
- **接口**: Type-C USB接口
- **USB芯片**: CH340C

### 物理尺寸
- **整体尺寸**: 45×35×39mm
- **外壳材质**: 注塑外壳

### 接口目前用的是okx 
api.py 是okx的api接口，可以获取到sol、eth、btc的价格
目前放在我的个人服务器上，所以需要修改api.py中的ip地址为你的服务器ip地址
也可以用我的 就不需要你做什么更改 连上wifi就可以了 

## 🛠️ 开发环境搭建

### 1. 安装PlatformIO

推荐使用PlatformIO作为开发环境：

```bash
# 安装PlatformIO Core
pip install platformio

# 或者使用VS Code + PlatformIO插件
# 在VS Code中安装PlatformIO IDE插件
```

### 2. 克隆项目

```bash
git clone [项目地址]
cd SD2_Ticker
```

### 3. 安装依赖库

项目依赖以下库（已在platformio.ini中配置）：
- `TFT_eSPI@^2.5.0` - TFT显示屏驱动
- `lv_arduino@^3.0.1` - LVGL图形库
- `ArduinoJson@^6.20.0` - JSON解析库

## 📡 WiFi配置

### 默认配置
项目默认配置了WiFi热点，您可以根据需要修改：

```cpp
// 在 src/main.ino 中修改以下配置
const char* ssid = "esp123";           // WiFi名称
const char* password = "esp123123";    // WiFi密码
```



## 🔄 编译与烧录

### 编译项目

```bash
# 编译项目
pio run

# 编译并生成固件文件
pio run --target build
```

### 烧录固件

#### 方法1: 指定端口烧录
```bash
# 烧录到指定端口（如COM3）
pio run -t upload --upload-port COM3
```

#### 方法2: 自动检测端口
```bash
# 自动检测并烧录
pio run -t upload
```

### 监听串口输出

#### 方法1: 使用PlatformIO监听
```bash
# 监听串口输出（波特率921600）
pio device monitor --baud 921600

# 或者使用简写
pio run -t monitor
```

#### 方法2: 指定端口监听
```bash
# 监听指定端口
pio device monitor --port COM3 --baud 921600
```

#### 方法3: 使用其他串口工具
- **Arduino IDE**: 工具 → 串口监视器，设置波特率为921600
- **PuTTY**: 连接类型选择Serial，设置波特率为921600
- **串口调试助手**: 选择对应COM端口，波特率921600

## 🚀 使用说明

### 首次使用
1. 将设备通过Type-C线连接到电脑
2. 烧录固件到设备
3. 配置WiFi连接信息
4. 重新烧录或使用OTA升级
5. 设备将自动连接WiFi并开始显示价格

### 正常使用
- 设备启动后会自动连接WiFi
- 屏幕显示SOL、ETH、BTC三种加密货币价格
- 边框具有彩虹色流动动画效果
- 价格每13秒更新一次（动画周期）

### 故障排除
1. **WiFi连接失败**: 检查WiFi名称和密码是否正确
2. **价格显示异常**: 检查网络连接和API服务状态
3. **屏幕无显示**: 检查电源连接和固件是否正确烧录
4. **串口无输出**: 检查COM端口设置和波特率

## 📊 显示效果

### 屏幕布局
- **顶部区域**: SOL价格显示
- **中部区域**: ETH价格显示  
- **底部区域**: BTC价格显示

### 动画特性
- **彩虹边框**: 流动的彩虹色像素边框
- **像素消失**: 边框像素随机消失和恢复效果
- **价格更新**: 在黑屏状态下更新价格数据
- **动画周期**: 13秒完整循环（消失5秒 + 黑屏3秒 + 恢复5秒）

## 🔧 技术细节

### 开发框架
- **平台**: ESP8266 (NodeMCU v2)
- **框架**: Arduino
- **编译器**: PlatformIO

### 关键参数
- **串口波特率**: 921600
- **烧录波特率**: 921600
- **屏幕旋转**: 0度（竖屏显示）
- **价格更新间隔**: 13秒

### 依赖服务
- **价格API**: 103.244.88.116:4399
- **支持接口**: /sol, /eth, /btc

## 📝 开发说明

### 代码结构
- `src/main.ino`: 主程序文件
- `platformio.ini`: 项目配置文件
- `.gitignore`: Git忽略文件配置

### 主要功能模块
1. **WiFi连接管理**: 自动连接和重连机制
2. **价格数据获取**: HTTP请求获取实时价格
3. **屏幕显示控制**: TFT屏幕绘制和动画
4. **动画系统**: 像素消失和恢复动画

### 自定义开发
如需修改显示内容或添加新功能：
1. 修改`src/main.ino`中的相关代码
2. 调整WiFi配置和API接口
3. 重新编译和烧录固件



## 📄 许可证

本项目采用MIT许可证，详见LICENSE文件。

---

**注意**: 使用前请确保您的WiFi网络满足要求，并正确配置网络连接信息。 
