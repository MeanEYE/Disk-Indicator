#!/bin/bash

function show_help {
	echo "Configure DiskIndicator provider support."
	echo -e "Usage: ./configre.sh [options]\n"
	echo "Options:"
	echo -e "\t-h, --help          Show this text"
	echo -e "\t--xorg              Include X.Org provider"
	echo -e "\t--console           Include console provider"
	echo -e "\t--thinkpad          Include ThinkPad provider"
	echo -e "\t--all               Include all providers"
	exit 0
}

add_xorg=false
add_console=false
add_thinkpad=false

# show help when no params are specified
if [ $# = 0 ]; then
	show_help
fi

# process flags
while test $# -gt 0; do
	case "$1" in
		-h|--help)
			show_help
			;;
		--all)
			add_xorg=true
			add_console=true
			add_thinkpad=true
			shift
			;;
		--xorg)
			add_xorg=true
			shift
			;;
		--console)
			add_console=true
			shift
			;;
		--thinkpad)
			add_thinkpad=true
			shift
			;;
		*)
			echo "Came"
			break
			;;
	esac
done

# clean up dependencies file
mkdir -p build
truncate --size=0 build/dependencies.h build/flags build/objects

# include selected providers
if $add_xorg; then
	echo -n "xorg.o " >> build/objects
	echo -n "-lX11 " >> build/flags
	echo "#include \"../src/xorg.h\"" >> build/dependencies.h
fi

if $add_console; then
	echo -n "console.o " >> build/objects
	echo "#include \"../src/console.h\"" >> build/dependencies.h
fi

if $add_thinkpad; then
	echo -n "thinkpad.o " >> build/objects
	echo "#include \"../src/thinkpad.h\"" >> build/dependencies.h
fi
