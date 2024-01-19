r:
	./run -i crash.png -m use-of-uninitialized-value -o reduced libpng/test_pngfix

rc:
	cp reduced reduced_crash
	./run -i reduced_crash -m use-of-uninitialized-value -o reduced libpng/test_pngfix

ro:
	./libpng/test_pngfix < crash.png

rr:
	./libpng/test_pngfix < reduced
