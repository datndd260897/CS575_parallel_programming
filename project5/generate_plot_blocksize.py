import matplotlib.pyplot as plt
from collections import defaultdict
import re

# Input text file path
input_file = "out.txt"

# Dictionary grouping data by Trials:
# Key: Trials, Value: list of (BlockSize, performance)
data_by_trials = defaultdict(list)

with open(input_file, "r") as f:
    for line in f:
        # Skip empty lines
        if not line.strip():
            continue
        try:
            # Use regex to extract required numbers
            # Example line: Trials =       1024, BlockSize =     8, MegaTrials/Second =     7.33, Probability= 9.570%
            numbers = re.findall(r"[-+]?[0-9]*\.?[0-9]+", line)
            # numbers: [Trials, BlockSize, performance, probability]
            if len(numbers) < 3:
                continue
            trials = int(numbers[0])
            block = int(numbers[1])
            performance = float(numbers[2])
            data_by_trials[trials].append((block, performance))
        except Exception as e:
            print("Error processing line:", line, e)

# Create the plot
plt.figure(figsize=(8, 6))
for trials, points in data_by_trials.items():
    # Sort points by BlockSize
    points.sort(key=lambda x: x[0])
    block_values = [pt[0] for pt in points]
    performance_values = [pt[1] for pt in points]
    plt.plot(block_values, performance_values, marker='o', label=f'Trials = {trials}')

plt.xlabel("BlockSize")
plt.ylabel("Performance (MegaTrials/Second)")
plt.title("Performance vs. BLOCKSIZE")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig('performance_vs_blocksize.png')
plt.show()