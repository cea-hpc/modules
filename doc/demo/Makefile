.PHONY: clean

imgs := $(patsubst %.cast,%.svg,$(wildcard */*.cast))

all: $(imgs)

%.svg: %.cast
	svg-term --out $@ <$<

clean:
	rm -f $(imgs)
