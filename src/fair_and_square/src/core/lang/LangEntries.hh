#ifndef __LangEntries__
#define __LangEntries__

#include <unordered_map>

namespace fas
{
class LangEntries
{
public:

    static LangEntries * GetInstance();

    std::string const &getEntry(std::string const &key_p) const;

    void loadFile(std::string const &filename_p);

private:
    LangEntries() {}
    static LangEntries *_instance {nullptr};

    std::unordered_map<std::string, std::string> _mapString;

};

} // namespace fas


#endif