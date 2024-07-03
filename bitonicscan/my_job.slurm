#!/bin/bash
# specify a partition
#SBATCH -p bigbatch
# specify number of nodes
#SBATCH -N 4
# specify number of tasks
#SBATCH --ntasks=8
# specify number of cores (or threads) per process
##SBATCH -c 2
# specify memory pool per node (for all cores on the node)
##SBATCH --mem 10
# specify the wall clock time limit for the job (hh:mm:ss) 5 mins
##SBATCH -t 5:00
# specify the job name
#SBATCH -J PC_mpi_com
# specify the filename to be used for writing output 
# change the path for your own account here
#SBATCH -o /home-mscluster/pberawala/bitonicsort/output/%N.%j.out
# specify the filename for stderr
# change the path for your own account here
#SBATCH -e /home-mscluster/pberawala/bitonicsort/error/%N.%j.err

echo ------------------------------------------------------
echo DATE: $(date)
echo 'Job is running on nodes ' $SLURM_JOB_NODELIST
echo ------------------------------------------------------
echo SLURM: sbatch is running on $SLURM_SUBMIT_HOST
echo SLURM: job ID is $SLURM_JOB_ID
echo SLURM: submit directory is $SLURM_SUBMIT_DIR
echo SLURM: number of nodes allocated is $SLURM_JOB_NUM_NODES
echo SLURM: number of tasks is $SLURM_NTASKS
echo SLURM: job name is $SLURM_JOB_NAME
echo ------------------------------------------------------
cd $SLURM_SUBMIT_DIR
export LD_LIBRARY_PATH=/usr/local/mpich-4.1.1/lib:$LD_LIBRARY_PATH
export PATH=/usr/local/mpich-4.1.1/bin:$PATH
echo =======================================
echo ---------Run Make----------
make
echo ---------Benchmark started----------
exponent=(32768 65536 131072)
nProcs=(1 2 4)

for pe in "${nProcs[@]}"
do
    echo ---------Working on $pe PEs----------
    for exp in "${exponent[@]}"
    do
        echo ${exp} > input.txt  # Clear the file before writing the new value
        echo =======================================
        echo "Testing array size: ${exp}"
        srun ./bitonic_mpi < input.txt
        echo =======================================
    done
    echo -------------------------------------
done
echo ---------Run Make Clean----------
make clean
echo ---------END----------
echo =======================================