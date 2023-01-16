import matplotlib.pyplot as plt
import csv
import sys


text_kwargs = dict(ha='center', va='center', fontsize=12, color='black')

fig, ax = plt.subplots() # note we must use plt.subplots, not plt.subplot

fig.set_size_inches(10.5,5.)

ax.set_xlim([-10, 200])
ax.set_ylim([-0, 100])

with open(sys.argv[1], newline='') as f:
	reader = csv.reader(f)
	first = True
	index = 0
	for row in reader:
		if not first:
			x = float(row[0])
			y = float(row[1])
			r = float(row[2])
			hp = float(row[3])
			color = 'r'
			index = index + 1
			if index % 3 == 0:
				color = 'b'
			elif index % 3 == 1:
				color = 'g'
			circle1 = plt.Circle((x, y), r, color=color)
			ax.add_patch(circle1)
			#plt.text(x, y, str(hp), **text_kwargs)

#		print(row)
		first = False
print("done")
plt.grid()
fig.savefig(sys.argv[2], dpi=120)
