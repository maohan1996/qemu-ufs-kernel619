编译流程见 build.sh

如果使用的是arm64的virt启动，需要用dts文件的话（因为默认编译是没有给出现成的dts）需要启动时候导出dtb，然后反编译出dts再编译成dtb：
1、导出dtb：
    qemu-system-aarch64 \
    -M virt,dumpdtb=virto.dtb \ ====>这一行的dumpdtb是导出dtb的命令
    -cpu cortex-a72 \
    -smp 4 \
    -m 512M \
    -kernel ../linux-6.7/arch/arm64/boot/Image.gz \
    -drive format=raw,file=rootfs_qemu.img \
    -nographic \
    -append "noinitrd root=/dev/vda rw console=ttyAMA0 init=/linuxrc ignore_loglevel" \

    执行之后会直接停止，然后目录中会出现virto.dtb，此时你需要反汇编：
        dtc -I dtb -O dts virto.dtb -o virto.dts
    如果想要在启动时候指定dtb，直接导出的dtb不能直接使用，你还需要把dts再编译成dtb：
        dtc -I dts -O dtb -o virto.dtb virto.dts

    若想支持ufs，需加入如下代码在dts(放到CPU后面就行)：
    ufs40: ufs40@d000000 {
		compatible = "ufshcd-ufs40";
		reg = <0x00 0xd000000 0x00 0x2000>;
		interrupts = <0 186 1>;
	};


2、使用方式：
	./qemu-system-aarch64 \
	-M virt \
	-cpu cortex-a72 \
	-smp 1 \
	-m 512M \
	-kernel /home/maohan/linux/qemu/kernel/arm64_buildout/arch/arm64/boot/Image.gz \
	-dtb virto.dtb \
	-nographic \
	--append "noinitrd root=/dev/vda rw nokaslr console=ttyAMA0 loglevel=8" \
	-drive if=none,file=rootfs.ext3,id=hd0 \
	-device virtio-blk-device,drive=hd0 \


