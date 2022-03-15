all: sensor cc

sensor.o: sensor.c defines.h
cc.o: cc.c defines.h

.c.o:
	gcc -c $<

sensor: sensor.o
	gcc -o sensor sensor.o

cc: cc.o
	gcc -o cc cc.o

clean:
	rm -f *.exe *.o *.a sensor cc