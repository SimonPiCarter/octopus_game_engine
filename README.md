# Octopus Engine

This engine aims at handling RTS on large scale, the goal is to have a fast lock step engine with only logic and no graphical implementation


## Note on commands to produce movie from steps

make && ./game && ./plot.sh

rm out.mp4 && ffmpeg -framerate 30 -pattern_type glob -i 'step/*.png' -c:v libx264 -pix_fmt yuv420p out.mp4  && cp out.mp4 ~/Data/Data/test/out.mp4
