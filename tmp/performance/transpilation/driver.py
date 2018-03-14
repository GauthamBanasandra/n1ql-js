import subprocess
import numpy as np

run_times = []
data_size = '100000'
js_file = './separate/normal_code.js'

for i in range(20):
    run_time = subprocess.check_output(['node', js_file, data_size])
    run_time = float(run_time)
    if run_time > 0:
        print float(run_time)
        run_times.append(float(run_time))

run_times = np.array(run_times)
print '\npercentile:', np.percentile(run_times, 50)
print 'mean:', np.mean(run_times)
print 'median:', np.median(run_times)
