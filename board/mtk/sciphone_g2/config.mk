#
# Loading U-Boot code at low addreses (begining of RAM)  can cause problem.
# Function get_ram_size() detects size of RAM writing pattern
# between 256MB and 0MB. It takes highest address and divides
# it by 2 on every iteration. It's possible that this function will
# destroy code which is executed at the moment (writing detection pattern).
# U-Boot will probably not occupy more than 2MB, so it's safe to
# load code between 4MB and 8MB (excluding 0x400000 and 0x800000).
# Loading U-Boot at address 0x500000 eliminates problem with get_ram_size()
# function.
#
CONFIG_SYS_TEXT_BASE = 0x500000
