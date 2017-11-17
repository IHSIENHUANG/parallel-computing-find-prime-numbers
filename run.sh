#!/bin/bash

#make all

jobid=$(qsub part1_node1.job| cut -d '.' -f 1)

jobid=$(qsub -W depend=afterok:${jobid} part1_node2.job | cut -d '.' -f 1 )
jobid=$(qsub -W depend=afterok:${jobid} part1_node4.job | cut -d '.' -f 1 )
jobid=$(qsub -W depend=afterok:${jobid} part1_node8.job | cut -d '.' -f 1 )
jobid=$(qsub -W depend=afterok:${jobid} part2_node1.job | cut -d '.' -f 1 )
jobid=$(qsub -W depend=afterok:${jobid} part2_node2.job | cut -d '.' -f 1 )
jobid=$(qsub -W depend=afterok:${jobid} part2_node4.job | cut -d '.' -f 1 )
jobid=$(qsub -W depend=afterok:${jobid} part2_node8.job | cut -d '.' -f 1 )
jobid=$(qsub -W depend=afterok:${jobid} part3_node1.job | cut -d '.' -f 1 )
jobid=$(qsub -W depend=afterok:${jobid} part3_node2.job | cut -d '.' -f 1 )
jobid=$(qsub -W depend=afterok:${jobid} part3_node4.job | cut -d '.' -f 1 )
qsub -W depend=afterok:${jobid} part3_node8.job | cut -d '.' -f 1



