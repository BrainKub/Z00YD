#!/bin/bash
pause()
{
read -s -n 1 -p "Press any key to continue...";
}
build()
{
#Compile kernel with a build script to make things simple
rm -R /home/brainkub/Android/kernel/GITHUB/kernel-3.18.19/out/
mkdir -p out
export CROSS_COMPILE=/home/brainkub/Android/utility/arm-eabi-4.8/bin/arm-eabi-
export USE_CCACHE=1
export ARCH=arm ARCH_MTK_PLATFORM=mt6580
make -C /home/brainkub/Android/kernel/GITHUB/kernel-3.18.19 O=/home/brainkub/Android/kernel/GITHUB/kernel-3.18.19/out ARCH=arm Z00YD_defconfig
#Edit the number according to the number of CPUs you have in your PC:
make -j4 -C /home/brainkub/Android/kernel/GITHUB/kernel-3.18.19 O=/home/brainkub/Android/kernel/GITHUB/kernel-3.18.19/out ARCH=arm | tee build.log
next;
}
pack()
{
cp /home/brainkub/Android/kernel/GITHUB/kernel-3.18.19/out/arch/arm/boot/zImage-dtb /home/brainkub/Android/utility/CarlivImageKitchen64/boot/boot.img-kernel
cd /home/brainkub/Android/utility/CarlivImageKitchen64/
./carliv
}
next()
{
echo "                                                 ";
echo "                                                 ";
echo "---------------------------------------------------";
echo " 1. - Rebuild                                    ";
echo "---------------------------------------------------";
echo "  2. - Pack                        ";
echo "---------------------------------------------------";
printf %s "Type the number, then press ENTER: ";
read renv;
if [ "$renv" == "1" ];
    then
      build;
fi;
if [ "$renv" == "2" ];
    then
      pack;
fi;
}
while :
do
	build;
done
