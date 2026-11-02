obj-m += oslab.o

PWD := $(CURDIR)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) MO=$(PWD)/build modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) MO=$(PWD)/build clean