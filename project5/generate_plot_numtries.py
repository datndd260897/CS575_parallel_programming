import matplotlib.pyplot as plt
from collections import defaultdict
import re

# Input text file path
input_file = "out.txt"

# Dictionary grouping data by BLOCKSIZE:
# Key: BLOCKSIZE, Value: list of (Trials, performance)
data_by_block = defaultdict(list)

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
            data_by_block[block].append((trials, performance))
        except Exception as e:
            print("Error processing line:", line, e)

# Create the plot
plt.figure(figsize=(8, 6))
for block, points in data_by_block.items():
    # Sort points by Trials
    points.sort(key=lambda x: x[0])
    trial_values = [pt[0] for pt in points]
    performance_values = [pt[1] for pt in points]
    plt.plot(trial_values, performance_values, marker='o', label=f'BlockSize = {block}')

plt.xlabel("Number of Trials")
plt.ylabel("Performance (MegaTrials/Second)")
plt.title("Performance vs. NUMTRIALS")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig('performance_vs_numtrials.png')
plt.show()