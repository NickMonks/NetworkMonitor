
#include <network-monitor/file-downloader.h>

#include <boost/asio.hpp>
#include <boost/test/unit_test.hpp>

#include <filesystem>
#include <fstream>
#include <string>

using NetworkMonitor::DownloadFile;

BOOST_AUTO_TEST_SUITE(network_monitor);

BOOST_AUTO_TEST_CASE(file_downloader)
{
    const std::string fileUrl {
        "https://ltnm.learncppthroughprojects.com/network-layout.json"
    };

    const auto destination {
        std::filesystem::temp_directory_path() / "network-layout.json"
    };

    // Download the file

    bool downloaded {DownloadFile(fileUrl, destination, TEST_CACERT_PEM)};
    BOOST_CHECK(downloaded);
    BOOST_CHECK(std::filesystem::exists(destination));


    {
        const std::string expectedString {"\"stations\":["};
        std::ifstream file {destination};

        std::string line {};
        bool foundExpectedString {false};
        while (std::getline(file, line)) {
            if (line.find(expectedString) != std::string::npos) {
                foundExpectedString = true;
                break;
            }
        }
        BOOST_CHECK(foundExpectedString);
    }

    
}