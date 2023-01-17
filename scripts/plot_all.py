import os
import sys

if not len(sys.argv) == 5:
	print("Usage is ",sys.argv[0]," path_to_csv_files index_first index_last path_to_script")
	exit(0)

file_dir = sys.argv[1]
first = int(sys.argv[2])
nb_files = int(sys.argv[3]) - first
script_dir = sys.argv[4]

for idx in range(nb_files):
	i = idx + first
	input_file = file_dir+("/step_{0}.csv".format(i))
	output_file = file_dir+"/step_{0}{1}{2}.png".format("0" if i < 100 else "", "0" if i < 10 else "", i)
	print("python ../../scripts/plot.py {0} {1}".format(input_file, output_file))
	if not os.system("python {2}/plot.py {0} {1}".format(input_file, output_file, script_dir)) == 0:
		break
