#ifndef stdout_redir_h
#define stdout_redir_h

#include <filesystem>
#include <string>

void redirect_stdout() {
    auto path = std::filesystem::path(__FILE__);
    path.remove_filename();
    path = path / "..";
    path /= "out.ppm";
    std::cout << "Redirecting stdout to file:\n" << path << std::endl;
    freopen(path.c_str(), "w", stdout);
}

#endif /* stdout_redir_h */
