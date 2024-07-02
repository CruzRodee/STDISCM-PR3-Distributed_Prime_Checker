import os
import pandas as pd

# Function to transpose a CSV file
def transpose_csv(input_file, output_file):
    # Read the CSV file
    df = pd.read_csv(input_file)
    
    # Transpose the DataFrame
    df_transposed = df.set_index('Thread Count').T
    
    # Save the transposed DataFrame to a new CSV file
    df_transposed.to_csv(output_file, index=True)

# File paths
input_file = 'test_results_with_slave.csv'
output_file = 'test_results_with_slave_transposed.csv'

# Print current working directory
print(f"Current working directory: {os.getcwd()}")

# List all files in the current directory
print("Files in current directory:")
for file in os.listdir('.'):
    print(file)

# Transpose the CSV file
transpose_csv(input_file, output_file)

print(f"Transposed CSV saved to {output_file}")
