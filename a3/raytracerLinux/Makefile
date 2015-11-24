CC = g++
CFLAGS = -ggdb -O2
LIBS = -lm

raytracer:  raytracer.o util.o light_source.o scene_object.o bmp_io.o
	$(CC) $(CFLAGS) -o raytracer \
	raytracer.o util.o light_source.o scene_object.o bmp_io.o $(LIBS)

clean:
	-rm -f core *.o
	-rm raytracer
	-rm view1.bmp
	-rm view2.bmp
	



