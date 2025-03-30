# Compiler

A compiler written in **C++** using **Flex** and **Bison** for generating parsers and lexers. The author of the grammar, tests, and virtual machine (located in the `labor4/maszyna_wirtualna` directory) is Dr. Maciej Gębala. Detailed requirements for the compiler's operation and information about the grammar and the virtual machine can be found in the `labor4.pdf` file.

Requirements:
 - GCC
 - Flex
 - Bison
 - GNU Make (required to compile the program using the `make` command).

To compile the compiler, run:
`make compiler`

To execute the compiler, use:
`./compiler`

To compile the virtual machine, run:
`make all`

To run the virtual machine operating on long long numbers:
`./maszyna-wirtualna`

To run the virtual machine supporting arbitrary natural numbers:
`./maszyna-wirtualna-cln`

# Kompilator

Kompilator napisany w języku **C++** przy użyciu programów **Flex** i **Bison** służących do generowania parserów i lexerów. Autorem gramatyki, testów i maszyny wirtualnej (umieszczonej w katalogu `labor4/maszyna_wirtualna`) jest dr Maciej Gębala. Szczegółowe wymagania dotyczące działania kompilatora oraz informacje o gramatyce i maszynie wirtualnej znajdują się w pliku `labor4.pdf`.

Wymagania:
 - GCC
 - Flex
 - Bison
 - GNU make (do kompilacji programu za pomocą polecenia `make`)

Kompilacja:
`make compiler`

Uruchomienie kompilatora:
`./compiler`

Kompilacja maszyny wirtualnej:
`make all`

Uruchomienie maszyny wirtaulnej działającej na liczbach typu long long:
`./maszyna-wirtualna`

Uruchomienie maszyny wirtualnej działającej na dowolnych liczbach naturalnych:
`./maszyna-wirtualna-cln`
