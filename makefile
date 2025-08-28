
SUBDIRS =  ex8 ex9

.PHONY: all clean $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

valgrind: $(SUBDIRS)
	@for dir in $(SUBDIRS); do \
		echo "Running valgrind in $$dir..."; \
		$(MAKE) -C $$dir valgrind || echo "No valgrind target in $$dir"; \
	done

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done