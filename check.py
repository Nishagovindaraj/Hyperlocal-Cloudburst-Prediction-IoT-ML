from flask import Flask, render_template_string, request
import joblib
import numpy as np

app = Flask(__name__)

# Load the trained model
model = joblib.load('landslide_model.pkl')

# HTML Template (The interface)
HTML_TEMPLATE = """
<!DOCTYPE html>
<html>
<head>
    <title>Landslide Prediction Manual Test</title>
    <style>
        body { font-family: sans-serif; text-align: center; background: #f4f4f4; padding: 50px; }
        .container { background: white; padding: 20px; border-radius: 10px; display: inline-block; box-shadow: 0px 0px 10px rgba(0,0,0,0.1); }
        input { margin: 10px; padding: 8px; width: 60px; }
        button { padding: 10px 20px; background: #28a745; color: white; border: none; border-radius: 5px; cursor: pointer; }
        .result { margin-top: 20px; font-size: 1.5em; font-weight: bold; }
        .danger { color: #dc3545; }
        .safe { color: #28a745; }
    </style>
</head>
<body>
    <div class="container">
        <h2>Manual Landslide Prediction</h2>
        <form method="POST">
            v1 (Rain): <input type="number" name="v1" value="50" step="1">
            v2 (Rain): <input type="number" name="v2" value="50" step="1"><br>
            v3 (Temp): <input type="number" name="v3" value="40" step="1">
            v4 (Moisture): <input type="number" name="v4" value="60" step="1"><br><br>
            <button type="submit">Check Prediction</button>
        </form>

        {% if prediction is not none %}
            <div class="result">
                Result: 
                {% if prediction == 1 %}
                    <span class="danger">DANGER: LANDSLIDE RISK</span>
                {% else %}
                    <span class="safe">SAFE: NO RISK</span>
                {% endif %}
            </div>
        {% endif %}
    </div>
</body>
</html>
"""

@app.route('/', methods=['GET', 'POST'])
def home():
    prediction = None
    if request.method == 'POST':
        # Get values from form
        v1 = int(request.form['v1'])
        v2 = int(request.form['v2'])
        v3 = int(request.form['v3'])
        v4 = int(request.form['v4'])
        
        # Prepare data for model
        input_data = np.array([[v1, v2, v3, v4]])
        prediction = model.predict(input_data)[0]
        
    return render_template_string(HTML_TEMPLATE, prediction=prediction)

if __name__ == '__main__':
    app.run(debug=True, port=5000)