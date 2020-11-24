OUT=unicode

$(OUT): Makefile *.cpp *.hpp
	g++ *.cpp -o $(OUT) `pkg-config gtkmm-3.0 --cflags --libs`

.PHONY: clean run

clean:
	-rm $(OUT)

run: $(OUT)
	./$(OUT)
