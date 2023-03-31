# Kompilator

Kompilator napisany w języku C++ przy użyciu programów Flex i Bison służących do generowania parserów i lexerów. Autorem gramatyki, testów i maszyny wirtualnej (umieszczonej w katalogu `labor4/maszyna_wirtualna`) jest dr Maciej Gębala.

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
