lib_asm=$(mktemp)
nasm -f elf64 -g -o $lib_asm src/lib.asm

for src in unit_tests/input/**
do
	in=$(basename $src)
	out="unit_tests/output/$in.out"

	# Complex compilation :)
	code_asm=$(mktemp)
	code_o=$(mktemp)
	executable=$(mktemp)
	trash=$(mktemp)
	./compiler $src $code_asm > trash
	nasm -f elf64 -g -o $code_o $code_asm
	gcc -g -no-pie -o $executable $code_o $lib_asm
	rm $code_asm
	rm $code_o

	curr_out=$(mktemp)
	$executable > $curr_out

	if test -f $out; then
		if cmp -s $curr_out $out; then
			printf "\033[0;32m$in\n"
		else
			printf "\033[0;31m$in\n"
		fi
	else
		cp $curr_out $out
		printf "\033[0;33m$in\n"
	fi

	rm $curr_out
done

rm $lib_asm
printf "\033[0;37m"
