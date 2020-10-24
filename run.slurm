#!/bin/bash 
#SBATCH --nodes=1
#SBATCH --tasks-per-node=1
#SBATCH --cpus-per-task=28
#SBATCH -p tornado-k40
#SBATCH -t 10-00:00:00
#SBATCH -J jobname
#SBATCH -o output.out
#SBATCH -e error.out
if [ -f /etc/profile.d/modules-basis.sh ]; 
        then source /etc/profile.d/modules-basis.sh  
fi  
module purge
module load library/opencl/intel
module load compiler/gcc/6
gcc -o hello hello.c -lOpenCL
./hello
