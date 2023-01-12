import matplotlib.pyplot as plt
import csv
import sys

fig, ax = plt.subplots() # note we must use plt.subplots, not plt.subplot

fig.set_size_inches(10.,10.)

ax.set_xlim([-15, 20])
ax.set_ylim([-15, 20])

with open(sys.argv[1], newline='') as f:
	reader = csv.reader(f)
	first = True
	index = 0
	for row in reader:
		if not first:
			x = float(row[0])
			y = float(row[1])
			r = float(row[2])
			color = 'r'
			index = index + 1
			if index % 3 == 0:
				color = 'b'
			elif index % 3 == 1:
				color = 'g'
			circle1 = plt.Circle((x, y), r, color=color)
			ax.add_patch(circle1)
		print(row)
		first = False

plt.grid()
fig.savefig(sys.argv[2], dpi=120)
