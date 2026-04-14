import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, classification_report
import joblib

# 1. Load the dataset
try:
    df = pd.read_csv('landslide_data.csv')
    print("Dataset loaded successfully.")
except FileNotFoundError:
    print("Error: landslide_data.csv not found! Run the generator script first.")
    exit()

# 2. Separate Features (X) and Target Label (y)
X = df[['v1', 'v2', 'v3', 'v4']]
y = df['label']

# 3. Split data (80% for training, 20% for testing)
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# 4. Initialize and Train the Model
# We use RandomForest because it's excellent for sensor-based classification
model = RandomForestClassifier(n_estimators=100, random_state=42)
model.fit(X_train, y_train)

# 5. Make Predictions to test accuracy
y_pred = model.predict(X_test)

# 6. Calculate Accuracy
accuracy = accuracy_score(y_test, y_pred)
print(f"\n--- Model Performance ---")
print(f"Accuracy Score: {accuracy * 100:.2f}%")
print("\nClassification Report:")
print(classification_report(y_test, y_pred))

# 7. Save the trained model to a file
joblib.dump(model, 'landslide_model.pkl')
print("\nModel saved as 'landslide_model.pkl'")