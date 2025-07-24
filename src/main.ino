/*
 * SD2小电视 SOL/ETH/BTC 价格显示器
 * 基于 RouterMonitor 的 TFT_eSPI 配置
 * 1.54寸 IPS屏 240*240分辨率 赛博朋克风格显示实时币价
 */

#include <TFT_eSPI.h>
#include <ESP8266WiFi.h>

// ========================
// 1. 硬件初始化
// ========================
TFT_eSPI tft = TFT_eSPI(); // 创建TFT对象

// ========================
// 2. WiFi配置
// ========================
const char* ssid = "zysjaq2.4";       // WiFi名称
const char* password = "@zysjaq123123123";       // WiFi密码
bool wifiConnected = false;

// ========================
// 3. 价格数据存储
// ========================
String solPrice = " ";     // SOL价格
String ethPrice = " ";     // ETH价格
String btcPrice = " ";     // BTC价格

// ========================
// 5. 显示动画相关变量
// ========================
unsigned long lastPixelUpdate = 0;
const unsigned long pixelInterval = 50; // 50ms动画间隔
bool pixelDisappearing = true;
bool pixelRecovering = false; 
bool pixelPause = false;
unsigned long pixelPauseStart = 0;
int pixelPhase = 0;
float currentPixelAlpha = 1.0;
int borderBase = 0;
int colorBase = 0;

// ========================
// 5. WiFi连接函数
// ========================
void connectWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 15) {
        delay(1000);
        attempts++;
    }
    wifiConnected = (WiFi.status() == WL_CONNECTED);
}

// ========================
// 6. 快速价格获取函数
// ========================
String getQuickPrice(const char* path) {
    if (!wifiConnected) {
        Serial.print("WiFi未连接，跳过请求: ");
        Serial.println(path);
        return "";
    }
    
    Serial.print("请求价格: ");
    Serial.println(path);
    
    WiFiClient client;
    if (!client.connect("103.244.88.116", 4399)) {
        Serial.print("连接失败: ");
        Serial.println(path);
        return "";
    }
    
    Serial.print("连接成功: ");
    Serial.println(path);
    
    client.setTimeout(1000); // 增加超时时间
    
    // 发送HTTP请求
    client.print(String("GET ") + path + " HTTP/1.1\r\n");
    client.print("Host: 103.244.88.116\r\n");
    client.print("Connection: close\r\n");
    client.print("\r\n");
    
    // 等待响应数据可用
    unsigned long timeout = millis() + 1000;
    while (client.available() == 0) {
        if (millis() > timeout) {
            Serial.print("等待响应超时: ");
            Serial.println(path);
            client.stop();
            return "";
        }
        delay(10);
    }
    
    // 读取所有响应数据 - 改进版本
    String response = "";
    String line = "";
    while (client.available() || client.connected()) {
        if (client.available()) {
            char c = client.read();
            line += c;
            if (c == '\n') {
                response += line;
                line = "";
            }
        } else {
            delay(1); // 短暂等待更多数据
        }
        
        // 防止无限等待
        if (response.length() > 1000) break;
    }
    
    // 添加最后一行（可能没有换行符）
    if (line.length() > 0) {
        response += line;
    }
    
    client.stop();
    
    Serial.print("完整响应: ");
    Serial.println(response);
    
    // 查找响应体（空行后的内容）
    int bodyStart = response.indexOf("\r\n\r\n");
    if (bodyStart == -1) {
        bodyStart = response.indexOf("\n\n");
        if (bodyStart != -1) bodyStart += 2;
    } else {
        bodyStart += 4;
    }
    
    if (bodyStart > 0 && bodyStart < response.length()) {
        String body = response.substring(bodyStart);
        body.trim();
        
        Serial.print("响应体: ");
        Serial.println(body);
        
        if (body.length() > 0 && body != "error") {
            Serial.print("成功获取价格: ");
            Serial.print(path);
            Serial.print(" = ");
            Serial.println(body);
            return body;
        }
    }
    
    Serial.print("解析失败: ");
    Serial.println(path);
    return "";
}

// ========================
// 7. 在特定时机更新价格
// ========================
void updatePricesAtKey() {
    Serial.println("开始价格更新...");
    wifiConnected = (WiFi.status() == WL_CONNECTED);
    if (!wifiConnected) {
        Serial.println("WiFi未连接，跳过价格更新");
        return;
    }
    
    Serial.println("WiFi已连接，开始获取价格");
    
    // 快速获取三个价格
    String newSol = getQuickPrice("/sol");
    if (newSol.length() > 0) {
        solPrice = newSol;
        Serial.print("SOL价格更新: ");
        Serial.println(solPrice);
    } else {
        Serial.println("SOL价格获取失败");
    }
    
    String newEth = getQuickPrice("/eth");
    if (newEth.length() > 0) {
        ethPrice = newEth;
        Serial.print("ETH价格更新: ");
        Serial.println(ethPrice);
    } else {
        Serial.println("ETH价格获取失败");
    }
    
    String newBtc = getQuickPrice("/btc");
    if (newBtc.length() > 0) {
        btcPrice = newBtc;
        Serial.print("BTC价格更新: ");
        Serial.println(btcPrice);
    } else {
        Serial.println("BTC价格获取失败");
    }
    
    colorBase = (colorBase + 1) % 256; // 颜色变换
    updatePricesOnlyText();
    
    Serial.println("价格更新完成，当前价格:");
    Serial.print("SOL: ");
    Serial.println(solPrice);
    Serial.print("ETH: ");
    Serial.println(ethPrice);
    Serial.print("BTC: ");
    Serial.println(btcPrice);
}

// ========================
// 8. 屏幕绘制与显示相关 (240*240分辨率适配)
// ========================
#define BLACK 0x0000
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define GREEN 0x07E0
unsigned long lastFullRedraw = 0; // 上次全屏重绘时间

// HSV转RGB565彩虹色
uint16_t hsv2rgb565(float h, float s, float v) {
    float r, g, b;
    int i = int(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);
    switch(i % 6) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }
    return tft.color565(int(r*255), int(g*255), int(b*255));
}

// 生成彩虹流动色，pixelAlpha控制像素显示概率
uint16_t getRainbowColor(int base, int offset) {
    float h = fmod((base + offset) / 256.0, 1.0); // 0~1
    return hsv2rgb565(h, 1.0, 1.0); // 彩虹色，满亮度
}

// 像素点消失边框绘制函数 (适配240*240) - 性能优化版本
void drawPixelBorder(int sectionY, int sectionHeight, int base, float pixelAlpha) {
    int alphaInt = (int)(pixelAlpha * 100); // 预计算整数值
    
    // 上边框 - 优化版本
    for (int x = 8; x < 232; x += 2) { // 减少绘制频率
        if (random(100) < alphaInt) {
            uint16_t color = getRainbowColor(base, 0);
            tft.drawPixel(x, sectionY + 4, color);
            if (x + 1 < 232) tft.drawPixel(x + 1, sectionY + 4, color); // 连续绘制
        } else {
            tft.drawPixel(x, sectionY + 4, BLACK);
            if (x + 1 < 232) tft.drawPixel(x + 1, sectionY + 4, BLACK);
        }
    }
    
    // 下边框 - 优化版本
    for (int x = 8; x < 232; x += 2) {
        if (random(100) < alphaInt) {
            uint16_t color = getRainbowColor(base, 64);
            tft.drawPixel(x, sectionY + sectionHeight - 5, color);
            if (x + 1 < 232) tft.drawPixel(x + 1, sectionY + sectionHeight - 5, color);
        } else {
            tft.drawPixel(x, sectionY + sectionHeight - 5, BLACK);
            if (x + 1 < 232) tft.drawPixel(x + 1, sectionY + sectionHeight - 5, BLACK);
        }
    }
    
    // 左边框 - 优化版本
    for (int y = sectionY + 4; y < sectionY + sectionHeight - 4; y += 2) {
        if (random(100) < alphaInt) {
            uint16_t color = getRainbowColor(base, 128);
            tft.drawPixel(8, y, color);
            if (y + 1 < sectionY + sectionHeight - 4) tft.drawPixel(8, y + 1, color);
        } else {
            tft.drawPixel(8, y, BLACK);
            if (y + 1 < sectionY + sectionHeight - 4) tft.drawPixel(8, y + 1, BLACK);
        }
    }
    
    // 右边框 - 优化版本
    for (int y = sectionY + 4; y < sectionY + sectionHeight - 4; y += 2) {
        if (random(100) < alphaInt) {
            uint16_t color = getRainbowColor(base, 192);
            tft.drawPixel(231, y, color);
            if (y + 1 < sectionY + sectionHeight - 4) tft.drawPixel(231, y + 1, color);
        } else {
            tft.drawPixel(231, y, BLACK);
            if (y + 1 < sectionY + sectionHeight - 4) tft.drawPixel(231, y + 1, BLACK);
        }
    }
}

// 只绘制币名和价格（不画边框）- 适配240*240屏幕
void drawPricesOnly(float pixelAlpha) {
    int sectionHeight = 80; // 240/3=80 三等分
    // --- SOL ---
    tft.setTextSize(2);
    tft.setTextColor(CYAN, BLACK);
    tft.setCursor(20, 15); tft.print("S");
    tft.setCursor(20, 35); tft.print("O");
    tft.setCursor(20, 55); tft.print("L");
    tft.setTextSize(3);
    
    // $符号像素消失效果 - SOL
    if (random(100) < (pixelAlpha * 100)) {
        tft.setTextColor(getRainbowColor(colorBase, 0), BLACK);
        tft.setCursor(60, 30);
        tft.print("$");
    } else {
        tft.setTextColor(BLACK, BLACK);
        tft.setCursor(60, 30);
        tft.print("$");
    }
    
    // SOL价格每个字符不同彩虹色
    int charOffset = 10;
    for (int i = 0; i < solPrice.length(); i++) {
        tft.setTextColor(getRainbowColor(colorBase, charOffset + i * 15), BLACK);
        tft.print(solPrice.charAt(i));
    }
    
    // --- ETH ---
    tft.setTextSize(2);
    tft.setTextColor(YELLOW, BLACK);
    tft.setCursor(20, sectionHeight + 15); tft.print("E");
    tft.setCursor(20, sectionHeight + 35); tft.print("T");
    tft.setCursor(20, sectionHeight + 55); tft.print("H");
    tft.setTextSize(3);
    
    // $符号像素消失效果 - ETH
    if (random(100) < (pixelAlpha * 100)) {
        tft.setTextColor(getRainbowColor(colorBase, 100), BLACK);
        tft.setCursor(60, sectionHeight + 30);
        tft.print("$");
    } else {
        tft.setTextColor(BLACK, BLACK);
        tft.setCursor(60, sectionHeight + 30);
        tft.print("$");
    }
    
    // ETH价格处理和彩虹色绘制
    String ethShow = ethPrice;
    int dotIdx = ethShow.indexOf('.');
    if (dotIdx > 0 && ethShow.length() > dotIdx + 1)
        ethShow = ethShow.substring(0, dotIdx + 2); // 只保留一位小数
    
    charOffset = 110;
    for (int i = 0; i < ethShow.length(); i++) {
        tft.setTextColor(getRainbowColor(colorBase, charOffset + i * 15), BLACK);
        tft.print(ethShow.charAt(i));
    }
    
    // --- BTC ---
    tft.setTextSize(2);
    tft.setTextColor(GREEN, BLACK);
    tft.setCursor(20, sectionHeight * 2 + 15); tft.print("B");
    tft.setCursor(20, sectionHeight * 2 + 35); tft.print("T");
    tft.setCursor(20, sectionHeight * 2 + 55); tft.print("C");
    tft.setTextSize(3);
    
    // $符号像素消失效果 - BTC
    if (random(100) < (pixelAlpha * 100)) {
        tft.setTextColor(getRainbowColor(colorBase, 200), BLACK);
        tft.setCursor(60, sectionHeight * 2 + 30);
        tft.print("$");
    } else {
        tft.setTextColor(BLACK, BLACK);
        tft.setCursor(60, sectionHeight * 2 + 30);
        tft.print("$");
    }
    
    // BTC价格处理和彩虹色绘制
    String btcInt = btcPrice;
    dotIdx = btcInt.indexOf('.');
    if (dotIdx > 0) btcInt = btcInt.substring(0, dotIdx); // 只保留整数
    
    charOffset = 210;
    for (int i = 0; i < btcInt.length(); i++) {
        tft.setTextColor(getRainbowColor(colorBase, charOffset + i * 15), BLACK);
        tft.print(btcInt.charAt(i));
    }
}

// 刷新所有内容（币名、价格、边框）
void updateAllScreen(float pixelAlpha) {
    int sectionHeight = 80; // 240/3=80
    // 清空所有区域
    tft.fillRect(0, 0, 240, sectionHeight, BLACK);
    tft.fillRect(0, sectionHeight, 240, sectionHeight, BLACK);
    tft.fillRect(0, sectionHeight * 2, 240, sectionHeight, BLACK);
    // 画三段像素边框
    drawPixelBorder(0, sectionHeight, borderBase, pixelAlpha);
    drawPixelBorder(sectionHeight, sectionHeight, borderBase + 32, pixelAlpha);
    drawPixelBorder(sectionHeight * 2, sectionHeight, borderBase + 64, pixelAlpha);
    // 绘制币名和价格
    drawPricesOnly(pixelAlpha);
}

// 只刷新边框和$符号（不动内容）
void updateBorderAndDollar(float pixelAlpha) {
    int sectionHeight = 80;
    // 只重画边框，不清空内容
    drawPixelBorder(0, sectionHeight, borderBase, pixelAlpha);
    drawPixelBorder(sectionHeight, sectionHeight, borderBase + 32, pixelAlpha);
    drawPixelBorder(sectionHeight * 2, sectionHeight, borderBase + 64, pixelAlpha);
    // $符号像素消失效果重绘 - 每个$符号不同彩虹色
    tft.setTextSize(3);
    if (random(100) < (pixelAlpha * 100)) {
        // SOL的$符号
        tft.setTextColor(getRainbowColor(colorBase, 0), BLACK); 
        tft.setCursor(60, 30); 
        tft.print("$");
        // ETH的$符号
        tft.setTextColor(getRainbowColor(colorBase, 100), BLACK); 
        tft.setCursor(60, sectionHeight + 30); 
        tft.print("$");
        // BTC的$符号
        tft.setTextColor(getRainbowColor(colorBase, 200), BLACK); 
        tft.setCursor(60, sectionHeight * 2 + 30); 
        tft.print("$");
    } else {
        tft.setTextColor(BLACK, BLACK); tft.setCursor(60, 30); tft.print("$");
        tft.setTextColor(BLACK, BLACK); tft.setCursor(60, sectionHeight + 30); tft.print("$");
        tft.setTextColor(BLACK, BLACK); tft.setCursor(60, sectionHeight * 2 + 30); tft.print("$");
    }
}

// 只更新价格文字，不触碰边框（避免破坏像素动画）
void updatePricesOnlyText() {
    int sectionHeight = 80;
    
    // 清空价格文字区域（不清空边框）
    tft.fillRect(85, 25, 150, 30, BLACK);  // SOL价格区域
    tft.fillRect(85, sectionHeight + 25, 150, 30, BLACK);  // ETH价格区域
    tft.fillRect(85, sectionHeight * 2 + 25, 150, 30, BLACK);  // BTC价格区域
    
    tft.setTextSize(3);
    
    // SOL价格每个字符不同彩虹色
    tft.setCursor(85, 30);
    int charOffset = 10;
    for (int i = 0; i < solPrice.length(); i++) {
        tft.setTextColor(getRainbowColor(colorBase, charOffset + i * 15), BLACK);
        tft.print(solPrice.charAt(i));
    }
    
    // ETH价格处理和彩虹色绘制
    String ethShow = ethPrice;
    int dotIdx = ethShow.indexOf('.');
    if (dotIdx > 0 && ethShow.length() > dotIdx + 1)
        ethShow = ethShow.substring(0, dotIdx + 2); // 只保留一位小数
    
    tft.setCursor(85, sectionHeight + 30);
    charOffset = 110;
    for (int i = 0; i < ethShow.length(); i++) {
        tft.setTextColor(getRainbowColor(colorBase, charOffset + i * 15), BLACK);
        tft.print(ethShow.charAt(i));
    }
    
    // BTC价格处理和彩虹色绘制
    String btcInt = btcPrice;
    dotIdx = btcInt.indexOf('.');
    if (dotIdx > 0) btcInt = btcInt.substring(0, dotIdx); // 只保留整数
    
    tft.setCursor(85, sectionHeight * 2 + 30);
    charOffset = 210;
    for (int i = 0; i < btcInt.length(); i++) {
        tft.setTextColor(getRainbowColor(colorBase, charOffset + i * 15), BLACK);
        tft.print(btcInt.charAt(i));
    }
}

// ========================
// 9. 主流程
// ========================
// 初始化显示屏并连接WiFi
void setup() {
    Serial.begin(921600); // 添加串口调试
    Serial.println("RouterMonitor启动 - 简化版本");
    
    tft.init();
    tft.setRotation(0); // 竖屏显示
    tft.fillScreen(TFT_BLACK);
    
    Serial.println("连接WiFi...");
    connectWiFi();
    
    // 初始显示完整界面（币名、边框、价格）
    borderBase = 0;
    updateAllScreen(1.0);
    
    // 初始化价格数据
    if (wifiConnected) {
        solPrice = getQuickPrice("/sol");
        ethPrice = getQuickPrice("/eth"); 
        btcPrice = getQuickPrice("/btc");
        updatePricesOnlyText();
        Serial.println("初始价格数据已加载");
    } else {
        Serial.println("WiFi未连接，无法获取价格");
    }
    
    Serial.println("初始化完成 - 价格将在动画节点更新");
}

// 主循环：专注UI动画，只在特定节点更新价格
void loop() {
    unsigned long now = millis();
    
    // 1. 边框像素动画 - 永不停止的流畅循环
    if (now - lastPixelUpdate >= pixelInterval) {
        if (pixelPause) {
            // 检查暂停时间是否超过3秒（给黑屏留3秒更新时间）
            if (now - pixelPauseStart >= 3000) {
                pixelPause = false;
                pixelRecovering = true;
                pixelPhase = 0; // 重新开始恢复
            }
        } else if (pixelRecovering) {
            // 像素点逐渐恢复到90%
            pixelPhase++;
            currentPixelAlpha = (pixelPhase / 100.0) * 0.9; // 从0恢复到90%
            
            // 检查是否恢复到90%
            if (pixelPhase >= 100) {
                pixelRecovering = false;
                pixelDisappearing = true;
                pixelPhase = 0; // 重新开始消失
                currentPixelAlpha = 0.9; // 设置为90%
                
                // 恢复到100%时不更新价格，保证动画永远流动
            }
            
            borderBase = (borderBase + 1) % 256;
            updateBorderAndDollar(currentPixelAlpha);
        } else if (pixelDisappearing) {
            // 像素点逐渐消失
            pixelPhase++;
            currentPixelAlpha = 0.9 - (pixelPhase / 100.0) * 0.9; // 从90%消失到0%
            
            // 检查是否完全消失
            if (pixelPhase >= 100) {
                pixelDisappearing = false;
                pixelPause = true;
                pixelPauseStart = now;
                pixelPhase = 0;
                currentPixelAlpha = 0.0; // 完全消失
                
                // 进入黑屏状态，立即更新价格
                Serial.println("进入黑屏状态，立即更新价格");
                Serial.println("动画周期: 消失5秒 + 黑屏3秒 + 恢复5秒 = 13秒一轮");
                updatePricesAtKey();
            }
            
            borderBase = (borderBase + 1) % 256;
            updateBorderAndDollar(currentPixelAlpha);
        }
        lastPixelUpdate = now;
    }
    
    // 极短延时，保持最高流畅性
    delay(2);
}
