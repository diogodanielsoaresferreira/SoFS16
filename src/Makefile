subdirs += probing
subdirs += rawdisk
subdirs += mksofs
subdirs += dealers
subdirs += freelists
subdirs += filecluster
subdirs += direntries
subdirs += tools
subdirs += syscalls
subdirs += sofsmount

.PHONY: $(subdirs)

all:	$(subdirs)

$(subdirs):
	@echo -e '\e[34;1;40;1mMaking $@...\e[m'
	@make -C $@

clean:
	@echo -e '\e[31;1;40;1mCleaning...\e[m'
	@for dir in $(subdirs) ; do make -C $$dir clean; done

cleanall:
	@echo -e '\e[31;1;40;1mCleaning all...\e[m'
	@for dir in $(subdirs) ; do make -C $$dir cleanall; done

