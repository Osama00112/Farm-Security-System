import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import rcParams

# Set font to Times New Roman
rcParams['font.family'] = 'Times New Roman'

# Read the Excel file into a DataFrame
file_path = "fpm project test files/laser data.xlsx"
df = pd.read_excel(file_path, header=None, names=["Time", "Detection"])

# Fill missing data with previous values
df["Detection"].ffill(inplace=True)

# Extract the data from the DataFrame
time = df["Time"]
detection = df["Detection"]

# Create the bar chart
plt.fill_between(time, detection, where=detection == 1, color='navy', alpha=0.3)

# Set y-axis limits
plt.ylim([0, 2])

# Set x-axis limits
plt.xlim([162, 180])

# Add "Ideal Condition" text and horizontal marker with vertical terminal lines
plt.text(165.9, 1.16, "Ideal", ha='left', va='center', fontsize=10)
plt.text(165.7, 1.04, "Condition", ha='left', va='center', fontsize=10)
plt.plot([165.1, 168.1], [1.1, 1.1], color='black', lw=1.5)
plt.plot([165.1, 165.1], [1.05, 1.15], color='black', lw=1.5)
plt.plot([168.1, 168.1], [1.05, 1.15], color='black', lw=1.5)


# Add "Detected Condition" text and horizontal marker with vertical terminal lines
plt.text(174, 1.16, "Detected", ha='left', va='center', fontsize=10)
plt.text(174, 1.04, "Condition", ha='left', va='center', fontsize=10)
plt.plot([173.5, 176.16], [1.1, 1.1], color='black', lw=1.5)
plt.plot([173.5, 173.5], [1.05, 1.15], color='black', lw=1.5)
plt.plot([176.16, 176.16], [1.05, 1.15], color='black', lw=1.5)

# Customize the chart
plt.xlabel("TIME (SEC)")
plt.ylabel("DETECTION (TRUE/FALSE)")
plt.title("Detection Chart")

# Add gridlines
plt.grid(True, linestyle='--', alpha=0.5)

# Add a border around the figure
fig = plt.gcf()
fig.set_edgecolor('black')
fig.set_linewidth(1.0)

# Display the chart
plt.show()
