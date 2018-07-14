for f in Symbols/*.ppm; do
	convert "$f" -compress none "${f%.ppm}.jpg"
done
