import matplotlib.pyplot as plt
from collections import defaultdict
import re

# Input text file path
input_file = "out.txt"

# Dictionary grouping data by Array Size (Data Size):
# Key: Array Size, Value: list of (Work Elements, performance)
data_by_array_size = defaultdict(list)

with open(input_file, "r") as f:
    for line in f:
        # Skip empty lines
        if not line.strip():
            continue
        try:
            # Use regex to extract required numbers
            # Example line: Array Size:     4096 , Work Elements:    8 , MegaPointsProcessedPerSecond:      34.81, (    1.5,    3.5,    9.0)
            numbers = re.findall(r"[-+]?[0-9]*\.?[0-9]+", line)
            # numbers: [Array Size, Work Elements, performance, ...]
            if len(numbers) < 3:
                continue
            array_size = int(numbers[0])
            work_elements = int(numbers[1])
            performance = float(numbers[2])
            data_by_array_size[array_size].append((work_elements, performance))
        except Exception as e:
            print("Error processing line:", line, e)

# Create the plot
plt.figure(figsize=(10, 6))
for array_size, points in data_by_array_size.items():
    # Sort points by Work Elements
    points.sort(key=lambda x: x[0])
    work_elements_values = [pt[0] for pt in points]
    performance_values = [pt[1] for pt in points]
    plt.plot(work_elements_values, performance_values, marker='o', label=f'Data Size (float) = {array_size}')

plt.xlabel("Work Elements (Local Size)")
plt.ylabel("Performance (MegaPointsProcessed/s)")
plt.title("Performance vs. Local Size (Work Elements)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig('performance_vs_localsize.png')
plt.show()