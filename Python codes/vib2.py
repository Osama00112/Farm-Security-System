import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import rcParams

# Set font to Times New Roman
rcParams['font.family'] = 'Times New Roman'

# Read the data into a DataFrame
file_path = "fpm project test files/vibration sensor data.csv"
df = pd.read_csv(file_path)

# Define a color mapping dictionary for calibration levels
calibration_colors = {
    "Low": 0,
    "Medium": 0.5,
    "High": 1
}

# Convert the "Calibration Level" values to numeric codes
df["Calibration Code"] = df["Calibration Level"].map(calibration_colors)

# Create the scatter plot with color mapping
plt.scatter(df["Time(sec)"], df["Acceleration (m/s2)"], c=df["Calibration Code"], cmap="viridis")
plt.xlabel("Time (sec)", fontsize = 12)
plt.ylabel("Acceleration (m/s^2)", fontsize = 12)
plt.title("Vibration Sensor Data: Time vs. Acceleration (Colored by Calibration Level)")
plt.colorbar(label="Calibration Level")
plt.grid(True)

# Display the chart
plt.show()
