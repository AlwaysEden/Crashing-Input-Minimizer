EXE = delta


jsmn:
	./${EXE} -i OperatingSystem/jsmn/testcases/crash.json -m "AddressSanitizer: heap-buffer-overflow" -o jsmn_reduced OperatingSystem/jsmn/./jsondump

xml:
	./${EXE} -i OperatingSystem/libxml2/testcases/crash.xml -m "SEGV on unknown address" -o xml_reduced "OperatingSystem/libxml2/./xmllint --recover --postvalid -"

png:
	./${EXE} -i OperatingSystem/libpng/crash.png -m "MemorySanitizer: use-of-uninitialized-value" -o png_reduced OperatingSystem/libpng/libpng/./test_pngfix 1>/dev/null &

bal:
	./${EXE} -i OperatingSystem/balance/testcases/fail -o bal_reduced OperatingSystem/balance/./balance

test_xml:
	./${EXE} -i xmlfile -m "SEGV on unknown address" -o xml_reduced "OperatingSystem/libxml2/./xmllint --recover --postvalid -" 1>/dev/null 2>outputfile.txt

test_png:
	./${EXE} -i OperatingSystem/libpng/crash.png -m "MemorySanitizer: use-of-uninitialized-value" -o png_reduced OperatingSystem/libpng/libpng/./test_pngfix 1>/dev/null 2>outputfile.txt

