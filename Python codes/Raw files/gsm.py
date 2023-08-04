import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import make_interp_spline
from matplotlib import rcParams

# Set font to Times New Roman
rcParams['font.family'] = 'Times New Roman'

# Load the data from the CSV file into a pandas DataFrame
data = pd.read_csv('fpm project test files/gsm-data.csv')

# Summary statistics of the data
summary = data.describe()
print(summary)


# Smooth the data using spline interpolation
time_smooth = make_interp_spline(data['Time'], data['Signal strength'])
time_new = np.linspace(data['Time'].min(), data['Time'].max(), 300)  # Increase the number of points for smoothness
signal_smooth = time_smooth(time_new)

# Create a line plot for Signal Strength over Time with smooth continuous lines
plt.figure(figsize=(10, 6))
plt.plot(time_new, signal_smooth, linestyle='-', color='Blue')
plt.xlabel('Time (seconds)', fontsize=18)
plt.ylabel('Signal Strength (dB)', fontsize=18)
plt.title('Signal Strength over Time', fontsize=18)
plt.grid(True)
plt.xticks(fontsize=14)
plt.yticks(fontsize=14)


# Adding horizontal lines for Excellent, Good, and Marginal regions
plt.axhline(y=-70, color='green', linestyle='--', label='Excellent')
plt.axhline(y=-75, color='orange', linestyle='--', label='Good')
plt.axhline(y=-80, color='Red', linestyle='--', label='Marginal')
plt.legend()

plt.show()




# # Create a bar chart for Response count
# response_counts = data['Response'].value_counts()
# plt.figure(figsize=(6, 4))
# plt.bar(response_counts.index, response_counts.values)
# plt.xlabel('Response')
# plt.ylabel('Count')
# plt.title('Response Counts')
# plt.grid(axis='y')
# plt.show()

# Create a bar chart for Quality categories
quality_counts = data['Quality'].value_counts()
plt.figure(figsize=(6, 4))
bars = plt.bar(quality_counts.index, quality_counts.values, zorder=2, color = "Purple")  # Set zorder to 2 for bars to be drawn on top
plt.xlabel('Quality', fontsize=16)
plt.ylabel('Count', fontsize=16)
plt.title('Quality Counts', fontsize=16)
plt.grid(axis='y', linestyle='--', zorder=0)  # Set zorder to 0 for the grid to be drawn behind the bars
plt.xticks(fontsize=14)
plt.yticks(fontsize=14)
plt.ylim(0, 16)

# Add labels above each bar showing the count
for bar in bars:
    height = bar.get_height()
    plt.annotate('{}'.format(height),
            xy=(bar.get_x() + bar.get_width() / 2, height),
            xytext=(0, 3),  # 3 points vertical offset for better alignment
            textcoords="offset points",
            ha='center', va='bottom', fontsize=12)
plt.show()

# Create a scatter plot for Signal Strength vs. Time colored by Quality
plt.figure(figsize=(10, 6))
quality_colors = {'Good': 'green', 'Excellent': 'blue', 'Marginal': 'orange', 'Ok': 'red'}
for quality, color in quality_colors.items():
    subset = data[data['Quality'] == quality]
    plt.scatter(subset['Time'], subset['Signal strength'], color=color, label=quality, s=50)
plt.xlabel('Time (seconds)')
plt.ylabel('Signal Strength (dB)')
plt.title('Signal Strength vs. Time (Colored by Quality)')
plt.legend()
plt.grid(True)
plt.show()
