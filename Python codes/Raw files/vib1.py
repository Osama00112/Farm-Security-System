import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import rcParams

# Set font to Times New Roman
rcParams['font.family'] = 'Times New Roman'

# Read the data into a DataFrame
file_path = "fpm project test files/vibration sensor data.csv"
df = pd.read_csv(file_path)

# Create the line plot
plt.plot(df["Time(sec)"], df["Acceleration (m/s2)"])
plt.xlabel("Time (sec)", fontsize = 12)
plt.ylabel("Acceleration (m/s^2)", fontsize = 12)
plt.title("Vibration Sensor Data: Time vs. Acceleration")
plt.grid(True)

# Display the chart
plt.show()
