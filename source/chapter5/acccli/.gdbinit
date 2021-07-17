set history filename arm.linux.gdb.history
set history save on
set history size 1000
file acccli
target remote 202.115.26.92:9000
break main
continue
