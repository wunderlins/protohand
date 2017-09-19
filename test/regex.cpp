#include <iostream>
#include <string>
#include <regex> // std::regex
#include <pcrecpp.h> // pcrecpp::RE -- needs "-lpcrecpp -lpcre"
#include <pcrscpp.h> // pcrscpp::replace -- needs "-lpcrscpp -lpcre"

int main() {
    std::regex std_rx (R"del(XYZ\d\d)del");
    pcrecpp::RE pcrecpp_rx (R"del(XYZ\d\d)del");
    pcrscpp::replace pcrscpp_rs(R"del(s/XYZ\d\d/A/g)del");
    std::string str = " XYZ111 d-dxxxxxxx XYZ222 t-nyyyyyyyyy XYZ333 t-r ";

    std::cout << "std::regex way: " << std::regex_replace (str, std_rx, "A") << std::endl
              << "pcrecpp way: ";

    std::string buffer(str);
    pcrecpp_rx.GlobalReplace("A", &buffer);

    std::cout << buffer << std::endl
              << "pcrscpp way: ";

    pcrscpp_rs.replace_store(str);
    std::cout << pcrscpp_rs.replace_result << std::endl;

    return 0;
}