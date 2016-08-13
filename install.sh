#!/bin/bash

PREFIX=/usr/local

for k in "$@"; do
	case "$k" in
		--prefix=*)
			PREFIX=( "${k#*=}" )
			shift
			;;
		*)
			echo Uknown options "$k"
			exit -1
	esac
done;

echo "Installing Maike with PREFIX=$PREFIX"

declare -a rollback

function do_cleanup ()
	{
	for k in "${rollback[@]}"; do
		echo "$k"
		eval "$k"
	done
	}

function uninstall_cmds ()
	{
	echo "PREFIX="'"'"$PREFIX"'"'
	for k in "${rollback[@]}"; do
		echo "$k"
	done
	}

trap '
if [ $? -ne 0 ]; then
	echo "Installation to $PREFIX failed"
	do_cleanup
fi
' 0

trap 'do_cleanup
exit 1' SIGINT SIGTERM 

function transaction ()
	{
	echo "$1"
	eval "$1"
	rollback=("$2" "${rollback[@]}")
	return 0
	}

set -e
transaction 'mkdir -p "$PREFIX"/include/maike' \
	'rmdir "$PREFIX"/include/maike && rmdir "$PREFIX"/include && rmdir "$PREFIX"'
transaction 'cp __targets/*.hpp "$PREFIX"/include/maike' \
	'rm "$PREFIX"/include/maike/*.hpp'
transaction 'mkdir -p "$PREFIX"/lib' \
	'rmdir "$PREFIX"/lib'
transaction 'cp __targets/libmaike.so "$PREFIX/lib"'\
	'rm "$PREFIX"/lib/libmaike.so'
transaction 'cp __targets/libmaikeplug.a "$PREFIX/lib"'\
	'rm "$PREFIX"/lib/libmaikeplug.a'
transaction 'mkdir -p "$PREFIX/lib/maike"' \
	'rmdir "$PREFIX"/lib/maike'
transaction 'cp __targets/target*.so "$PREFIX/lib/maike"' \
	'rm "$PREFIX"/lib/maike/target*.so'
transaction 'mkdir -p "$PREFIX/bin"' \
	'rmdir "$PREFIX"/bin'
transaction 'cp __targets/maike "$PREFIX/bin"' \
	'rm "$PREFIX"/bin/maike'

if [ "$(id -u)" == "0" ]; then
	chmod -R o+r "$PREFIX"/include
	chmod -R o+r "$PREFIX"/bin
	chmod -R o+r "$PREFIX"/lib
	chmod -R o+r "$PREFIX"/include/maike
	chmod -R o+r "$PREFIX"/lib/maike
fi


uninstall_cmds > .uninstall.sh
chmod u+x .uninstall.sh

echo "Maike has now been installed"

if ! [[ "$PATH" == ?(*:)"$PREFIX/bin"?(:*) ]];then
	echo " * Remember to add $PREFIX/bin to the PATH variable before using Maike"
fi

if ! [[ "$CPATH" == ?(*:)"$PREFIX/include"?(:*) ]];then
	echo " * If you wish to use libmaike, or want to develop Maike plugins, \
you may need to add $PREFIX/include to the CPATH variable."
fi

if ! [[ "$LIBRARY_PATH" == ?(*:)"$PREFIX/lib"?(:*) ]];then
	echo "* If you wish to use libmaike, or want to develop Maike plugins, \
you may need to add $PREFIX/lib to the LIBRARY_PATH variable."
fi

if ! [[ "$LD_LIBRARY_PATH" == ?(*:)"$PREFIX/lib"?(:*) ]];then
	echo " * If you wish to use libmaike, or want to develop Maike plugins, \
you may need to add $PREFIX/lib to the LD_LIBRARY_PATH variable."
fi


#echo "If you whish to use Maike later, make sure that "
#	" * the PATH variable $PREFIX/bin. Currently it is set to $PATH"
