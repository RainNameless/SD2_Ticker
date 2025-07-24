from flask import Flask, Response
import requests

app = Flask(__name__)

# 通用函数：获取指定币种的价格
def get_price(symbol):
    url = f"https://www.okx.com/api/v5/market/ticker?instId={symbol}-USDT"
    try:
        resp = requests.get(url, timeout=10)
        resp.raise_for_status()
        data = resp.json()
        return data["data"][0]["last"]
    except Exception:
        return "error"

@app.route('/sol')
def sol():
    return Response(get_price("SOL"), mimetype="text/plain")

@app.route('/eth')
def eth():
    return Response(get_price("ETH"), mimetype="text/plain")

@app.route('/btc')
def btc():
    return Response(get_price("BTC"), mimetype="text/plain")

@app.route('/doge')
def doge():
    return Response(get_price("DOGE"), mimetype="text/plain")

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=4399)

