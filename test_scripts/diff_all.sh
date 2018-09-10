#!/bin/bash

# File that outputs the differences between our superblock and the professor superblock

# Name of Created Disk
name="zzz"
# Name of differences file
final_name="all_differences.txt"
# Initial number of blocks of created disk
blocks=1000

j=0
clusters=2
inodes=0

> "$final_name"

while [ "$blocks" -lt 200000 ]
do

	for (( clusters = 1; clusters <= 8; clusters++ ))
	do
		j=0
		
		while [ "$j" -lt 8 ]
		do

			case "$j" in
				7) inodes=$(($blocks/8))
				;;
				1) inodes=$((($blocks/4)+1))
				;;
				2) inodes=$((($blocks/10)-1))
				;;
				3) inodes=$((($blocks/6)))
				;;
				4) inodes=$((($blocks/16)+1))
				;;
				5) inodes=127
				;;
				6) inodes=598
				;;
				*) inodes=209
				;;
			esac
			echo "Blocks: $blocks"
			echo "inodes: $inodes"
			echo "Clusters: $clusters"
			create_disk=$(../bin/createDisk $name $blocks)

			# Get Professor Superblock
			prof_output=$(../bin/mksofs_bin -z -c $clusters -i $inodes $name)
			content_prof=$(../bin/showblock -a 0-$(($blocks-1)) $name)
			echo "" > "prof_Outp"

			# Write content on file
			printf '%s\n' "$content_prof" | while IFS= read -r line
			do
			   echo "$line" >> "prof_Outp"
			done

			# Get Group Superblock
			group_output=$(../bin/mksofs -z -c $clusters -i $inodes $name)
			content_group=$(../bin/showblock -a 0-$(($blocks-1)) $name)
			echo "" > "group_Outp"

			# Write content on file
			printf '%s\n' "$content_group" | while IFS= read -r line
			do
			   echo "$line" >> "group_Outp"
			done

			# Print differences on file
			if diff "prof_Outp" "group_Outp" >/dev/null ; then
				#echo "Blocks: $blocks" >> "$final_name"
				#echo "The Same!" >> "$final_name"
				echo ""
			else
				echo "Blocks: $blocks" >> "$final_name"
				echo "inodes: $inodes" >> "$final_name"
				echo "Clusters: $clusters" >> "$final_name"
				diff "prof_Outp" "group_Outp" >> "$final_name"
			fi
			j=$(($j+1))
		done
	done

	blocks=$(($blocks+1))
done
