#include "Windows.h"
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

std::string& ltrim(std::string& str)
{
    auto it2 = std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
    str.erase(str.begin(), it2);
    return str;
}

std::string& rtrim(std::string& str)
{
    auto it1 = std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
    str.erase(it1.base(), str.end());
    return str;
}

int main()
{
    bool error = false;
    ifstream fxp("effects.fxp");
    ofstream fxs;

    if (fxp.is_open())
    {
        bool foundName = false;
        string line, buffer, effectName;

        //jump header
        while (getline(fxp, line)) if (line.rfind("FX_PROJECT_DATA:", 0) != string::npos) break;

        //loop lines
        while (getline(fxp, line))
        {
            buffer.append(line).append("\n"); //store this line to buffer

            if (foundName)
            {
                if (line.rfind("TXDNAME:", 0) != string::npos) // end
                {
                    fxs << buffer << endl; //send buffer to file
                    fxs.close();
                    buffer.clear();
                    foundName = false;
                    // next effect
                    while (getline(fxp, line)) if (line.rfind("FX_SYSTEM_DATA:", 0) != string::npos) break;
                    buffer.append(line).append("\n");
                }
                else
                {
                    if (line.rfind("TEXTURE", 0) != string::npos)
                    {
                        size_t textureNameStart = line.find(":");
                        string textureFileName = line.substr(textureNameStart + 1);
                        textureFileName = ltrim(rtrim(textureFileName)) + ".png";

                        if (textureFileName.rfind("NULL", 0) == string::npos) //ignore NULL
                        {
                            try { copy_file("textures/" + textureFileName, "out/" + effectName + "/" + textureFileName, copy_options::overwrite_existing); }
                            catch (exception& e) { cout << e.what() << endl; error = true; }
                        }
                    }
                }
            }
            else
            {
                if (line.rfind("NAME:", 0) != string::npos)
                {
                    size_t effectNameStart = line.find(":");
                    effectName = line.substr(effectNameStart + 1);
                    effectName = ltrim(rtrim(effectName));

                    create_directories("out/" + effectName);
                    fxs.open("out/" + effectName + "/" + effectName +".fxs");
                    foundName = true;
                }
            }
        }
        fxp.close();
    }
    cout << "Done" << endl;
    if (error) Sleep(30000);
    return 0;
}