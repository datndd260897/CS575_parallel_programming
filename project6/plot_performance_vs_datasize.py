import matplotlib.pyplot as plt
from collections import defaultdict
import re

# Input text file path
input_file = "out.txt"

# Dictionary grouping data by Work Elements (local size):
# Key: Work Elements, Value: list of (Array Size, performance)
data_by_work_elements = defaultdict(list)

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
            data_by_work_elements[work_elements].append((array_size, performance))
        except Exception as e:
            print("Error processing line:", line, e)

# Create the plot
plt.figure(figsize=(10, 6))
for work_elements, points in data_by_work_elements.items():
    # Sort points by Array Size
    points.sort(key=lambda x: x[0])
    array_size_values = [pt[0] for pt in points]
    performance_values = [pt[1] for pt in points]
    plt.plot(array_size_values, performance_values, marker='o', label=f'Work Elements = {work_elements}')

plt.xlabel("Data Size (float)")
plt.ylabel("Performance (MegaPointsProcessed/s)")
plt.title("Performance vs. Data Size")
plt.legend()
plt.grid(True)
plt.xscale('log')  # Log scale for better visualization of array sizes
plt.tight_layout()
plt.savefig('performance_vs_datasize.png')
plt.show()