for I in `seq 1 $1`; do
    ./deskgen mine -p ./people.json -d ./desk.json
    printf "\r[%7d/%7d]" $((I)) $1
done