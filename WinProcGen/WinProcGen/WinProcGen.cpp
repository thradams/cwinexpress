#include "stdafx.h"
#include <set>
#include <map>
#include <string>
#include <locale>
#include <fstream>
#include <codecvt>
#include <regex>
#include <iostream>
using namespace std;

void GenerateCommands(std::wofstream& ofs,
    std::set<std::wstring>& functions,
    const std::wstring& className);
struct TemplateItem
{
    std::wstring declaration;
    std::wstring body;
};

std::wstring GetClassName(const std::wstring& str)
{
    auto pos = str.find(L"_");
    if (pos != std::wstring::npos)
    {
        return str.substr(0, pos);
    }
    return L"";
}


std::wstring GetLastFunctionName(const std::wstring& str)
{
    auto pos = str.rfind(L"_");
    if (pos != std::wstring::npos)
    {
        return str.substr(pos + 1, str.size() - pos - 1);
    }
    return L"";
}
std::wstring GetFunctionName(const std::wstring& str)
{
    auto pos = str.find(L"_");
    if (pos != std::wstring::npos)
    {
        return str.substr(pos, str.size() - pos);
    }
    return L"";
}

void Load(const const char* filename,
    std::set<std::wstring>& functions,
    std::map<std::wstring, int>& classes,
    std::map<std::wstring, TemplateItem>& map2)
{
    std::locale ulocale(std::locale(), new std::codecvt_utf8<wchar_t>);
    std::wifstream ifs(filename);
    ifs.imbue(ulocale);

    if (ifs.is_open())
    {
        std::wstring ws;

        std::wregex word_regex(L"[a-zA-Z]+_[_a-zA-Z0-9]+");

        while (std::getline(ifs, ws))
        {
            auto words_begin =
                std::wsregex_iterator(ws.begin(),
                    ws.end(),
                    word_regex);

            auto words_end = std::wsregex_iterator();

            for (auto i = words_begin; i != words_end; ++i)
            {
                auto match = *i;
                auto str = match.str();

                if (ws.find(L"//") != 0)
                {
                    //ignora linha comentada
                    auto className = GetClassName(str);
                    auto funcName = GetFunctionName(str);
                    auto it = map2.find(funcName);




                    if (it != map2.end())
                    {
                        classes[className] = 1;
                        functions.insert(str);
                    }
                    else
                    {
                        //Verificar se � comando de menu
                        if (funcName.find(L"_OnMenuCommand_") == 0)
                        {
                            classes[className] = 1;
                            functions.insert(str);
                            //_OnMenuCommand_Exit
                            //neste caso colocar em uma lista especial
                            //no WM_COMMAND o gerator tem que fazer uma lista
                            //pelo nome IDM_EXIT
                            //o ultimo else chama o oncommand
                        }
                    }
                }

            }
        }
    }
}

std::wstring GetFunctionName2(std::wstring& ws)
{
    std::wstring str;
    std::wregex word_regex(L"\\{class\\}_[#_a-zA-Z0-9]+");
    auto words_begin =
        std::wsregex_iterator(ws.begin(),
            ws.end(),
            word_regex);

    auto words_end = std::wsregex_iterator();

    for (auto i = words_begin; i != words_end; ++i)
    {
        auto match = *i;
        str = match.str();
        break;
    }
    return GetFunctionName(str);
}

void LoadTemplate(const char* filename,
    std::map<std::wstring, TemplateItem>& map)
{
    std::locale ulocale(std::locale(), new std::codecvt_utf8<wchar_t>);
    std::wifstream ifs(filename);
    ifs.imbue(ulocale);

    if (ifs.is_open())
    {
        std::wstring ws;

        while (std::getline(ifs, ws))
        {
            auto funcName = GetFunctionName2(ws);
            TemplateItem item;
            item.declaration = ws;

            std::wstring body;
            while (std::getline(ifs, ws))
            {
                if (ws.empty())
                {
                    break;
                }
                body += ws;
                body += L"\n";
            }

            //ignora linhas em branco
            while (!ws.empty() && std::getline(ifs, ws))
            {
            }

            item.body = body;
            map[funcName] = item;
        }
    }
}


inline void find_replace(std::wstring& in_this_string,
    const std::wstring& find,
    const std::wstring& replace)
{
    std::wstring::size_type pos = 0;
    while (std::wstring::npos != (pos = in_this_string.find(find, pos)))
    {
        in_this_string.replace(pos, find.length(), replace);
        pos += replace.length();
    }
}

void Generate(std::wofstream& ofs,
    std::map<std::wstring, TemplateItem>& map2,
    std::set<std::wstring>& functions,
    std::map<std::wstring, int>& classes,
    bool bForwardDeclarations)
{
    if (bForwardDeclarations)
    {
        ofs << L"/*Forward declarations*/" << endl << endl;
    }

    for (auto& itClasses : classes)
    {
        auto className = itClasses.first;
        bool bIsDlg = functions.find(className + L"_OnInitDialog") != functions.end();

        if (!bForwardDeclarations)
        {
            std::wstring body;
            if (bIsDlg)
            {
                body = map2[L"_#begindlg"].body;
            }
            else
            {
                body = map2[L"_#begin"].body;
            }
            

            find_replace(body, L"{class}", className);
            ofs << body;
        }
        for (auto& functionFullName : functions)
        {
            auto suffix = GetClassName(functionFullName);
            if (suffix == itClasses.first)
            {
                auto functionName = GetFunctionName(functionFullName);
                auto it = map2.find(functionName);
                if (it != map2.end())
                {
                    if (bForwardDeclarations)
                    {
                        auto body = it->second.declaration;
                        find_replace(body, L"{class}", suffix);
                        ofs << body << endl;
                    }
                    else
                    {
                        auto body = it->second.body;
                        find_replace(body, L"{class}", suffix);
                        ofs << body;
                    }
                }
            }
        }
        if (!bForwardDeclarations)
        {
            GenerateCommands(ofs,
                functions,
                className);

            std::wstring body;
            if (bIsDlg)
            {
                body = map2[L"_#enddlg"].body;
            }
            else
            {
                body = map2[L"_#end"].body;
            }
            find_replace(body, L"{class}", className);
            ofs << body;
        }
        else
        {
            ofs << endl;
        }
    }
}

inline void ToUpper(std::wstring& str)
{
    transform(
        str.begin(), str.end(),
        str.begin(),
        towupper); // towlower for lower case
}
void GenerateCommands(std::wofstream& ofs,
    std::set<std::wstring>& functions,
    const std::wstring& className)
{
    bool bIncluded = false;

    int c = 0;

    for (auto& functionFullName : functions)
    {
        if (functionFullName.find(L"_OnMenuCommand_") != std::wstring::npos)
        {
            auto func = GetLastFunctionName(functionFullName);

            auto classNameLocal = GetClassName(functionFullName);
            if (className == classNameLocal)
            {
                ToUpper(func);
                if (!bIncluded)
                {
                    bIncluded = true;
                    ofs << L"    case WM_COMMAND:" << endl;

                }

                if (c > 0)
                    ofs << L"      else";
                ofs << L"        if (LOWORD(wParam) == " << L"IDM_" << func << ")" << endl
                    << L"       " << functionFullName << L"(p);" << endl;
                c++;
            }
            //ofs << L"void " << functionFullName << L"(" << className << L"*p);" << endl;
        }
    }
    //adicionar command se existir
    auto it = functions.find(className + L"_OnCommand");
    if (it != functions.end())
    {
        if (!bIncluded)
        {
            bIncluded = true;
            ofs << L"    case WM_COMMAND:" << endl;

        }

        if (c > 0)
            ofs << L"        else " << endl;
        ofs << L"       " << *it << L"(p, LOWORD(wParam) );" << endl;
    }
    //ofs  <<

    if (bIncluded)
    {
        ofs << L"    break;" << endl << endl;

    }
}

void GenerateCommands(std::wofstream& ofs,
    std::set<std::wstring>& functions,
    bool bForwardDeclarations)
{

    for (auto& functionFullName : functions)
    {
        if (functionFullName.find(L"_OnMenuCommand_") != std::wstring::npos)
        {
            auto className = GetClassName(functionFullName);
            ofs << L"void " << functionFullName << L"(" << className << L"*p);" << endl;
        }
    }
}


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "input.c  output.c";
        return 1;
    }

    const char* filename = argv[1];// "readme.txt";
    const char* templatename = "template.txt";

    if (argc > 3)
    {
        cout << "Template:" << argv[3];
        templatename = argv[3];
    }

    std::map<std::wstring, TemplateItem> map2;
    LoadTemplate(templatename, map2);

    std::set<std::wstring> functions;
    std::map<std::wstring, int> classes;
    Load(filename, functions, classes, map2);

    const char* filenameOut = argv[2];


    std::locale ulocale(std::locale(), new std::codecvt_utf8<wchar_t>);
    std::wofstream ofs(filenameOut);
    ofs.imbue(ulocale);

    if (ofs.is_open())
    {
    }
    ofs << L"/*" << endl;
    ofs << L" DO NOT EDIT THIS FILE" << endl;
    ofs << L"See https://github.com/thradams/cwinexpress" << endl;
    ofs << L"*/" << endl << endl;

    Generate(ofs, map2, functions, classes, true);

    GenerateCommands(ofs,
        functions,
        true);

    Generate(ofs, map2, functions, classes, false);
    return 0;
}
