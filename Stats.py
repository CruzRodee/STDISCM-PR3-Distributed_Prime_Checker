import pandas as pd
from scipy import stats

# Load the data
data = pd.read_csv('Results.csv')

# Ensure data is in the correct format
# Each row should be a single observation pair (time with and without slaves)

# Perform the paired t-test
t_statistic, p_value = stats.ttest_rel(data['Time_with_slaves'], data['Time_without_slaves'])

# Print the results
print(f'T-statistic: {t_statistic}, P-value: {p_value}')

# Interpret the results
alpha = 0.05
if p_value < alpha:
    print("Reject the null hypothesis. There is a significant difference in performance.")
else:
    print("Fail to reject the null hypothesis. There is no significant difference in performance.")

# How to run:
# pip install pandas scipy
# python Stats.py