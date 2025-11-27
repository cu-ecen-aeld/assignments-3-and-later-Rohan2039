#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
# if [ ! -d "${OUTDIR}/linux-stable" ]; then
#     #Clone only if the repository does not exist.
# 	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
# 	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
# fi
# if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
#     cd linux-stable
#     echo "Checking out version ${KERNEL_VERSION}"
#     git checkout ${KERNEL_VERSION}
    
#     # TODO: Add your kernel build steps here

#     make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- mrproper
#     make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- defconfig
#     make -j4 ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- all
#     make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- dtbs

# fi

echo "Adding the Image in outdir"

cp $HOME/Image ${OUTDIR}

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
mkdir -p rootfs
cd rootfs
mkdir -p bin dev etc home/conf lib lib64 proc sbin sys tmp usr var
mkdir -p usr/bin usr/lib usr/sbin
mkdir -p var/log 

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
else
    cd busybox
fi

# TODO: Make and install busybox
make distclean
make defconfig
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
make CONFIG_PREFIX="${OUTDIR}/rootfs" ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install

cd ${OUTDIR}
echo "Library dependencies"
program_interpreter=$(${CROSS_COMPILE}readelf -a $HOME/busybox | grep "program interpreter" | awk -F'/' '{print $3}' | tr -d ']')
Shared_library=$(${CROSS_COMPILE}readelf -a $HOME/busybox | grep "Shared library" | awk -F': ' '{print $2}' | tr -d '[]')
Shared_library_1=$(printf "%s" "$Shared_library" | sed -n '1p')
Shared_library_2=$(printf "%s" "$Shared_library" | sed -n '2p')
Shared_library_3=$(printf "%s" "$Shared_library" | sed -n '3p')

# TODO: Add library dependencies to rootfs
cp $(find /usr/local/arm-cross-compiler/install/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu -name ${program_interpreter}) ${OUTDIR}/rootfs/lib
cp $(find /usr/local/arm-cross-compiler/install/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu -name ${Shared_library_1}) ${OUTDIR}/rootfs/lib64
cp $(find /usr/local/arm-cross-compiler/install/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu -name ${Shared_library_2}) ${OUTDIR}/rootfs/lib64
cp $(find /usr/local/arm-cross-compiler/install/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu -name ${Shared_library_3}) ${OUTDIR}/rootfs/lib64


# TODO: Make device nodes
cd ${OUTDIR}/rootfs

if [ ! -e "${OUTDIR}/rootfs/dev/null" ] || [ ! -e "${OUTDIR}/rootfs/dev/console" ]; then
    sudo mknod -m 666 /tmp/aeld/rootfs/dev/null c 1 3
    #sudo mknod -m 666 ${OUTDIR}/tmp/aeld/rootfs/dev/null c 1 3
    sudo mknod -m 600 /tmp/aeld/rootfs/dev/console c 5 1
    #sudo mknod -m 600 ${OUTDIR}/rootfs/dev/console c 5 1
fi
# # TODO: Clean and build the writer utility
cd $HOME/CRZ/Learnings//LinuxAssign/assignments-3-and-later-Rohan2039/finder-app
rm -f writer *.o
${CROSS_COMPILE}gcc -c writer.c -o writer.o
${CROSS_COMPILE}gcc writer.o -o writer

# # TODO: Copy the finder related scripts and executables to the /home directory
# # on the target rootfs
cd  $HOME/CRZ/Learnings//LinuxAssign/assignments-3-and-later-Rohan2039/
cp finder-app/finder.sh ${OUTDIR}/rootfs/home
cp conf/username.txt ${OUTDIR}/rootfs/home/conf
cp conf/assignment.txt ${OUTDIR}/rootfs/home/conf
cp finder-app/finder-test.sh ${OUTDIR}/rootfs/home
cp finder-app/autorun-qemu.sh ${OUTDIR}/rootfs/home
cp finder-app/writer ${OUTDIR}/rootfs/home
cd ${OUTDIR}/rootfs

# TODO: Chown the root directory
sudo chown -R root:root *
# TODO: Create initramfs.cpio.gz
find .| cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
cd ${OUTDIR}
gzip -f initramfs.cpio
