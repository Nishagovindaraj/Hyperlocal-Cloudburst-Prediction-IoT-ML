import socket
import mysql.connector
import re
import joblib
import numpy as np
import threading
from flask import Flask, render_template

app = Flask(__name__)
model = joblib.load('landslide_model.pkl')

db_config = {'host': 'localhost', 'user': 'root', 'password': 'Karthi@211277', 'database': 'newschema'}

# UDP Listener remains the same
def udp_listener():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", 4210))
    
    try:
        conn = mysql.connector.connect(**db_config, autocommit=True)
        cursor = conn.cursor()
        print(" Database and UDP Socket Ready.")
    except Exception as e:
        print(f" Initialization Error: {e}")
        return

    while True:
        try:
            # 1. Receive data and get the sender's address (addr)
            data, addr = sock.recvfrom(1024)
            message = data.decode('utf-8')
            values = re.findall(r':(\d+)', message)

            if len(values) == 4:
                int_vals = [int(v) for v in values]
                
                # 2. ML Prediction
                features = np.array([int_vals])
                pred = int(model.predict(features)[0])
                
                # 3. Save to Database
                sql = "INSERT INTO esp32_logs (v1, v2, v3, v4, prediction) VALUES (%s, %s, %s, %s, %s)"
                cursor.execute(sql, (*int_vals, pred))
                
                # 4. Determine result character
                # D = Danger, S = Safe
                result_char = "D" if pred == 1 else "S"
                
                # 5. Send result back to ESP32
                sock.sendto(result_char.encode(), addr)
                
                # 6. Print to CMD
                print(f" Received: {int_vals} |  Prediction: {result_char} |  Sent back to {addr[0]}")
                
            else:
                print(f" Malformed packet: {message}")
                
        except Exception as e:
            print(f" Listener Loop Error: {e}")

@app.route('/')
def index():
    conn = mysql.connector.connect(**db_config)
    cursor = conn.cursor(dictionary=True)
    cursor.execute("SELECT * FROM esp32_logs ORDER BY timestamp DESC LIMIT 1")
    latest = cursor.fetchone()
    conn.close()
    return render_template('index.html', data=latest) # Points to templates/index.html

if __name__ == '__main__':
    threading.Thread(target=udp_listener, daemon=True).start()
    app.run(host='0.0.0.0', port=5000)