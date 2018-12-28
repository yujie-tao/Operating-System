all: thsh

thsh: thsh.c
	gcc -Wall -Werror thsh.c -o thsh -g

handin: clean
	@if [ `git status --porcelain| wc -l` != 0 ] ; then echo "\n\n\n\n\t\tWARNING: YOU HAVE UNCOMMITTED CHANGES\n\n    Consider committing any pending changes and rerunning make handin.\n\n\n\n"; fi
	git tag -f -a handin -m "Lab 1 Handin"
	git push -f origin --tags handin

update:
	git checkout master
	git pull https://github.com/comp530-f18/lab1.git master

clean:
	rm thsh