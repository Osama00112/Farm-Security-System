import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import rcParams

# Set font to Times New Roman
rcParams['font.family'] = 'Times New Roman'

# Read the data into a DataFrame
file_path = "fpm project test files/vibration sensor data.csv"
df = pd.read_csv(file_path)

# Extract data for each calibration level
low_data = df[df["Calibration Level"] == "Low"]["Acceleration (m/s2)"]
medium_data = df[df["Calibration Level"] == "Medium"]["Acceleration (m/s2)"]
high_data = df[df["Calibration Level"] == "High"]["Acceleration (m/s2)"]

# Create the box plot with mean line
plt.boxplot([low_data, medium_data, high_data],
            labels=["Low", "Medium", "High"],
            showmeans=False, meanline=True, meanprops={'linestyle': '--', 'color': 'blue'})

plt.xlabel("Calibration Level", fontsize=12)
plt.ylabel("Acceleration (m/s^2)", fontsize=12)
plt.title("Vibration Sensor Data: Calibration Level vs. Acceleration")
plt.grid(True)

# Display the chart
plt.show()

# Print summary statistics
print("Summary Statistics:")
print("Low Calibration Level:")
print(low_data.describe())
print("\nMedium Calibration Level:")
print(medium_data.describe())
print("\nHigh Calibration Level:")
print(high_data.describe())
