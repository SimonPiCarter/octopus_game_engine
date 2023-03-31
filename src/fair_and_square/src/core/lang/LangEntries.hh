#ifndef __LangEntries__
#define __LangEntries__

#include <unordered_map>

namespace fas
{
class LangEntries
{
public:

    /// @brief singleton getter
    static LangEntries * GetInstance();

    /// @brief get the entry
    std::string const &getEntry(std::string const &key_p) const;

    /// @brief Load entries from a file
    void loadFromFile(std::string const &filename_p);

private:
    LangEntries();
    static LangEntries *_instance;

    std::unordered_map<std::string, std::string> _mapEntries;

};

} // namespace fas


#endif
