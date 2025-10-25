#! /bin/bash

runFlag=false
guiFlag=false
cleanFlag=false

if [ $# -eq 0 ]; then
    echo "Just compiling."
fi

for var in "$@"
do
	if [ "$var" = "-r" ];
	then
	    echo "Running after Compiling."
		runFlag=true

	elif [ "$var" = "-g" ];
	then
	    echo "Gui active."
		guiFlag=true

	elif [ "$var" = "-c" ];
	then
		echo "Clean applied."
		cleanFlag=true
	fi;
done

if [ "$cleanFlag" = true ]
then
	rm -rf cmake-build-cpp
	rm myProject
fi

mkdir -p cmake-build-cpp
cd cmake-build-cpp || exit


if [ "$guiFlag" = true ]
then
	cmake .. -DCMAKE_BUILD_TYPE=Release -DGUI_MF=ON
else
	cmake .. -DCMAKE_BUILD_TYPE=Debug
fi

make -j "$(nproc)"
cd ..  || exit

if [ "$runFlag" = true ]
then
	./myProject
fi
