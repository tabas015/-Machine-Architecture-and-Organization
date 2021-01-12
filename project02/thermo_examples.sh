#!/bin/bash
#
# Shows examples of running thermo_main on various sensor values


# echo '> make thermo_main' 
make thermo_main > /dev/null
echo

data="0 35 64 $((64*5)) $((64*10)) $((64*10*5)) $((64*10*49)) $((64*10*50)) $((64*10*100))"

for t in $data; do
    printf "==========CELSIUS FOR %d==========\n" "$t"
    echo '> ./thermo_main' $t "C"
    ./thermo_main $t "C"
    echo
    echo
    printf "==========FAHRENHEIT FOR %d==========\n" "$t"
    echo '> ./thermo_main' $t "F"
    ./thermo_main $t "F"
    echo
    echo
done

