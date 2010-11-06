makedoc:
	make -C readme
	make -C refman/latex
clean:
	@if [ -d refman ]; then rm -r refman; echo "rm -r refman"; fi
	make -C readme clean
