#include <network-monitor/file-downloader.h>
#include <curl/curl.h>

using namespace NetworkMonitor;


bool DownloadFile(
        const std::string& fileUrl,
        const std::filesystem::path& destination,
        const std::filesystem::path& caCertFile = {}
    )
    {
        CURL* curl {curl_easy_init()};
        // safe nullptr check
        if (curl == nullptr) {
            return false;
        }

        // open the file in our local destination, to write the curl content
        std::FILE* fp {fopen(destination.string().c_str(), "wb")};
        if (fp == nullptr) {
            curl_easy_cleanup(curl);
            return false;
        }

        // Configure curl to create secure connection over TLS - that's why we used the cacert 
        curl_easy_setopt(curl, CURLOPT_URL, fileUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_CAINFO, caCertFile.string().c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);


        // write data to the following fp file, when the request is performed
         curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        

        //perform request- this basically starts the transfer as described in the options
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        //close file
        fclose(fp);

        return res == CURLE_OK;


    }

    nlohmann::json ParseJsonFile(
        const std::filesystem::path& source
    )
    {
        // create a empty parsed JSON object:
        nlohmann::json parsed {};

        try {
            std::ifstream file {source};

            // use overloaded operator >> from nlohmann
            // will accept a ifstream object:
            
            file >> parsed;
        } catch () {
            // will return an empty object
        }

        return parsed
    }