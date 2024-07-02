import subprocess
import re
import numpy as np
import pandas as pd

# Function to run the client program and capture the output time
def run_client(start, end, threads):
    command = f"./client"
    process = subprocess.Popen(command, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    
    # Provide input to the client program
    input_str = f"{start}\n{end}\n{threads}\n"
    stdout, stderr = process.communicate(input=input_str.encode())
    
    if process.returncode != 0:
        print(f"Error running client: {stderr.decode('utf-8')}")
        return None
    
    # Extract the time taken from the output
    output = stdout.decode('utf-8')
    print(output)  # Print the output for debugging
    match = re.search(r'Time taken: ([\d\.]+) milliseconds', output)
    if match:
        return float(match.group(1))
    else:
        return None

# Configuration for the tests
start = 1
end = int(1e8)
threads = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
runs = 5

# Data structure to store results
results_with_slave = np.zeros((len(threads), runs))

# Run tests with slave
for i, thread_count in enumerate(threads):
    for run in range(runs):
        runtime = run_client(start, end, thread_count)
        if runtime:
            results_with_slave[i][run] = runtime

# Calculate averages
average_with_slave = np.mean(results_with_slave, axis=1)

# Create a DataFrame to store the results
df = pd.DataFrame({
    'Thread Count': threads,
    'Run #1': results_with_slave[:, 0],
    'Run #2': results_with_slave[:, 1],
    'Run #3': results_with_slave[:, 2],
    'Run #4': results_with_slave[:, 3],
    'Run #5': results_with_slave[:, 4],
    'Average': average_with_slave,
})

# Save the results to a CSV file
df.to_csv('test_results_with_3slaves.csv', index=False)

print("Test results saved to test_results_with_3slaves.csv")
