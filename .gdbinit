file "./build/CC1101_Test.elf"

# Connect to texane stlink gdb server
target extended-remote :4242

# Uncomment to enable semihosting
# monitor semihosting enable

# Flash program and load symbols
load
break main

# Run to main (first breakpoint)
continue
