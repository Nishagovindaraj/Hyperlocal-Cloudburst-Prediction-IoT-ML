import pandas as pd
import random

data = []
for _ in range(500):
    v1 = random.randint(50, 80) #soil
    v2 = random.randint(50, 80) #Rain
    v3 = random.randint(35, 50) #Temp
    v4 = random.randint(55, 85) #Humidity
    
    # Logic: If v1 (Rain) is high and v4 (Moisture) is high, it's a landslide risk
    if v1 > 72 and v4 > 78 and v2 > 60:
        label = 1
    else:
        label = 0
        
    data.append([v1, v2, v3, v4, label])

df = pd.DataFrame(data, columns=['v1', 'v2', 'v3', 'v4', 'label'])
df.to_csv('landslide_data.csv', index=False)
print("Dataset 'landslide_data.csv' created with 500 rows!")