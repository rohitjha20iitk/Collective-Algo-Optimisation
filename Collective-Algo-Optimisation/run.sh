echo "*****Running Assignment 2*****"          

make

# P = 4 : (1,4) (2,2) (4,1) ----> (2,2) 
# P = 16 : (1,16) (4,4) ---> (4,4)


b=4
for execution in 1 2 3 4 5 6 7 8 9 10
do
    echo $execution
    for P in 4 16
    do    
        for ppn in 1 8 
        do
	      if [ $P == $b ]
        then
            g=$((P/2))
        else
            g=$((P/4))
        fi
        python script.py $g $g $ppn
        for D in 16 256 2048
          do 
           	total=$((P * ppn))
           	mpirun -np $total -f hostfile ./code $g $g $ppn $D >> data_$P*$ppn.txt
          done
        done
    done
done

echo "Plotting data"
python3 plot.py

echo "*****Completed Assignment 2*****"
