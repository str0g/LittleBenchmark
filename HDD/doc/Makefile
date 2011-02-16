makedoc:
	make -C refman/latex pdf
	make -C readme
clean:
	@if [ -d refman ]; then rm -r refman; echo "rm -r refman"; fi
	make -C readme clean
