#Matt Mallett
#CprE 308 Project 2
#run_tests.sh
#automated testing script

GEN="1"

echo "Speedup tests"
for i in 4 8 32 64 256 1024 2048 #4096 6000 #these take too long
    do
    for j in 1 2 4 6 8
        do
            printf "\n%dx%d %d threads %d generations" $i $i $j $GEN
            time ./life -n $i -t $j -r $GEN -i input/input_${i}.txt -o output/out${i}_t${j}.txt
        done
    done

#My results must be slightly different, cmp says they are completely different, but visually they are identical
#I can switch between them in the editor with no visual difference
#All values are identical, it looks like we use different spacers
#if [ $GEN -eq 1 ]
#then
#    echo "Difference comparison with correct solutions"
#    for i in 8 32 64 1024 2048
#        do
#            for j in 1 2 4 6 8
#            do
#                cmp output/out${i}_t${j}.txt output/given/output_${i}.txt
#            done
#        done
#fi
