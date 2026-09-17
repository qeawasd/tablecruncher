#include "csvparser.hh"
#include <iostream>
#include <sstream>
#include <stdexcept>

static void require(bool value, const char *message) {
    if (!value) throw std::runtime_error(message);
}

int main() {
    try {
        CsvDefinition definition;
        CsvDataStorage storage;
        CsvParser parser;
        std::istringstream csv("id,note\r\n00123,\"hello\nworld\"\r\n");
        parser.parseCsvStream(&csv, storage, &definition, 0, true, false);
        require(!parser.hasUnclosedField(), "Valid multiline CSV rejected");
        require(storage.rows() == 2 && storage.columns() == 2, "Wrong CSV dimensions");
        require(storage.get(1, 0) == "00123", "Leading zero lost");
        require(storage.get(1, 1) == "hello\nworld", "Multiline field corrupted");

        CsvParser incomplete;
        CsvDataStorage partial;
        std::istringstream broken("id,note\n1,\"unfinished");
        incomplete.parseCsvStream(&broken, partial, &definition, 0, true, false);
        require(incomplete.hasUnclosedField(), "Incomplete quoted field not detected");

        CsvParser semicolon;
        CsvDataStorage separated;
        definition.delimiter = ';';
        std::istringstream other("id;value\n2;new\n");
        semicolon.parseCsvStream(&other, separated, &definition, 0, true, false);
        require(separated.get(1, 1) == "new", "Explicit delimiter not respected");

        CsvParser empty;
        CsvDataStorage noRows;
        std::istringstream blank("");
        empty.parseCsvStream(&blank, noRows, &definition, 0, true, false);
        require(noRows.rows() == 0, "Empty file not detected");
        std::cout << "PASS: multiline/leading zeros, incomplete quotes, delimiter, empty file\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
