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

} // namespace NetworkMonitor

#endif // WEBSOCKET_CLIENT_FILE_DOWNLOADER_H
