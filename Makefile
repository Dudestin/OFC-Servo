sum_project:mem.o main.o tty.o gpio.o str_util.o motor.o reset.o clock.o curs_dialog.o curs_yesno.o tui.o
	g++ -o sum_product mem.o main.o tty.o gpio.o str_util.o motor.o reset.o	clock.o curs_dialog.o curs_yesno.o tui.o -lncurses -pthread -O2
clean:
	rm -f *.o *~ sum_project
