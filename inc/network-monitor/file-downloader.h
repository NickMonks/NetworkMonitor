#ifndef WEBSOCKET_CLIENT_FILE_DOWNLOADER_H
#define WEBSOCKET_CLIENT_FILE_DOWNLOADER_H

#include <filesystem>
#include <string>

namespace NetworkMonitor {

    /*! \brief Download a file from a remote HTTPS URL
     *  
     *  \param destination 
     * 
     *  \param cacertFile
     * 
     */

    bool DownloadFile(
        const std::string& fileUrl,
        const std::filesystem::path& destination,
        const std::filesystem::path& caCertFile = {}
    );

    /*! \brief Parse a local file into a JSON object.
    *
    *  \param source The path to the JSON file to load and parse.
    */
    nlohmann::json ParseJsonFile(
        const std::filesystem::path& source
    );

} // namespace NetworkMonitor

#endif // WEBSOCKET_CLIENT_FILE_DOWNLOADER_H
