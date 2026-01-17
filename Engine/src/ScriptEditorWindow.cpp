#include "ScriptEditorWindow.h"
#include <imgui.h>
#include <fstream>
#include <sstream>
#include <cctype>
#include "Log.h"

ScriptEditorWindow::ScriptEditorWindow()
    : EditorWindow("Script Editor"),
    hasUnsavedChanges(false),
    bufferSize(1024 * 1024),
    showLineNumbers(true),
    fontSize(16.0f),
    showSearchReplace(false),
    enableSyntaxHighlighting(true),
    autoCheckSyntax(true),
    scrollY(0.0f),
    currentLine(1)
{
    textBuffer = new char[bufferSize];
    memset(textBuffer, 0, bufferSize);
    memset(searchText, 0, sizeof(searchText));
    memset(replaceText, 0, sizeof(replaceText));

    InitializeLuaKeywords();

    colorKeyword = ImVec4(0.98f, 0.45f, 0.45f, 1.0f);
    colorFunction = ImVec4(0.4f, 0.85f, 0.94f, 1.0f);
    colorString = ImVec4(0.9f, 0.87f, 0.44f, 1.0f);
    colorNumber = ImVec4(0.68f, 0.51f, 0.82f, 1.0f);
    colorComment = ImVec4(0.45f, 0.52f, 0.45f, 1.0f);
    colorOperator = ImVec4(0.98f, 0.45f, 0.45f, 1.0f);
    colorIdentifier = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
    colorError = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
    colorLineNumber = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    colorBackground = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
}

ScriptEditorWindow::~ScriptEditorWindow()
{
    delete[] textBuffer;
}

void ScriptEditorWindow::InitializeLuaKeywords()
{
    luaKeywords = {
        "and", "break", "do", "else", "elseif", "end", "false",
        "for", "function", "if", "in", "local", "nil", "not",
        "or", "repeat", "return", "then", "true", "until", "while"
    };

    luaBuiltinFunctions = {
        "print", "type", "tonumber", "tostring", "pairs", "ipairs",
        "next", "select", "table", "string", "math",
        "assert", "error", "pcall", "xpcall", "require",
        "setmetatable", "getmetatable",
        "Start", "Update", "Engine", "Input", "Time"
    };
}

void ScriptEditorWindow::Draw()
{
    if (!isOpen) return;

    ImGui::SetNextWindowSizeConstraints(ImVec2(800, 600), ImVec2(FLT_MAX, FLT_MAX));

    if (ImGui::Begin(name.c_str(), &isOpen, ImGuiWindowFlags_MenuBar))
    {
        DrawMenuBar();

        if (showSearchReplace)
        {
            ImGui::Separator();
            ImGui::Text("Search:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);
            ImGui::InputText("##search", searchText, sizeof(searchText));

            ImGui::SameLine();
            ImGui::Text("Replace:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);
            ImGui::InputText("##replace", replaceText, sizeof(replaceText));

            ImGui::SameLine();
            if (ImGui::Button("Replace All"))
            {
                std::string content = textBuffer;
                std::string search = searchText;
                std::string replace = replaceText;

                size_t pos = 0;
                int count = 0;
                while ((pos = content.find(search, pos)) != std::string::npos)
                {
                    content.replace(pos, search.length(), replace);
                    pos += replace.length();
                    count++;
                }

                strncpy_s(textBuffer, bufferSize, content.c_str(), _TRUNCATE);
                MarkAsModified();
                LOG_CONSOLE("[ScriptEditor] Replaced %d occurrences", count);
            }

            ImGui::Separator();
        }

        DrawTextEditor();

        if (!syntaxErrors.empty())
        {
            DrawErrorPanel();
        }

        DrawStatusBar();
    }
    ImGui::End();

    if (!isOpen && hasUnsavedChanges)
    {
        ImGui::OpenPopup("Unsaved Changes##ScriptEditor");
        isOpen = true;
    }

    if (ImGui::BeginPopupModal("Unsaved Changes##ScriptEditor", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("You have unsaved changes!");
        ImGui::Text("Do you want to save before closing?");
        ImGui::Separator();

        if (ImGui::Button("Save", ImVec2(120, 0)))
        {
            SaveScript();
            hasUnsavedChanges = false;
            isOpen = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Discard", ImVec2(120, 0)))
        {
            hasUnsavedChanges = false;
            isOpen = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void ScriptEditorWindow::DrawMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save", "Ctrl+S", false, !currentScriptPath.empty()))
            {
                SaveScript();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Close"))
            {
                isOpen = false;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Find & Replace", "Ctrl+F"))
            {
                showSearchReplace = !showSearchReplace;
            }

            ImGui::Separator();

            ImGui::MenuItem("Line Numbers", nullptr, &showLineNumbers);
            ImGui::MenuItem("Syntax Highlighting", nullptr, &enableSyntaxHighlighting);
            ImGui::MenuItem("Auto Check Syntax", nullptr, &autoCheckSyntax);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::SliderFloat("Font Size", &fontSize, 10.0f, 24.0f, "%.0f");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("Check Syntax Now"))
            {
                CheckSyntaxErrors();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
    {
        if (ImGui::IsKeyPressed(ImGuiKey_S) && !currentScriptPath.empty())
        {
            SaveScript();
        }

        if (ImGui::IsKeyPressed(ImGuiKey_F))
        {
            showSearchReplace = !showSearchReplace;
        }

        if (ImGui::IsKeyPressed(ImGuiKey_H))
        {
            enableSyntaxHighlighting = !enableSyntaxHighlighting;
            LOG_CONSOLE("[ScriptEditor] Syntax highlighting: %s",
                enableSyntaxHighlighting ? "ON" : "OFF");
        }
    }
}

void ScriptEditorWindow::DrawTextEditor()
{
    ParseTextIntoLines();

    float lineNumberWidth = showLineNumbers ? 60.0f : 0.0f;
    ImVec2 editorSize = ImGui::GetContentRegionAvail();
    editorSize.y -= (!syntaxErrors.empty() ? 80.0f : 20.0f);

    if (enableSyntaxHighlighting)
    {
        DrawColoredReadOnlyView(editorSize, lineNumberWidth);
    }
    else
    {
        DrawEditableView(editorSize, lineNumberWidth);
    }
}

void ScriptEditorWindow::DrawColoredReadOnlyView(ImVec2 editorSize, float lineNumberWidth)
{
    ImGui::BeginChild("ColoredEditor", editorSize, true);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 screenPos = ImGui::GetCursorScreenPos();
    float lineHeight = ImGui::GetTextLineHeight();

    // Background
    drawList->AddRectFilled(screenPos,
        ImVec2(screenPos.x + editorSize.x, screenPos.y + editorSize.y),
        ImGui::ColorConvertFloat4ToU32(colorBackground));

    // Line numbers background
    if (showLineNumbers)
    {
        drawList->AddRectFilled(screenPos,
            ImVec2(screenPos.x + lineNumberWidth, screenPos.y + editorSize.y),
            ImGui::ColorConvertFloat4ToU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f)));

        drawList->AddLine(
            ImVec2(screenPos.x + lineNumberWidth, screenPos.y),
            ImVec2(screenPos.x + lineNumberWidth, screenPos.y + editorSize.y),
            ImGui::ColorConvertFloat4ToU32(ImVec4(0.3f, 0.3f, 0.3f, 1.0f)), 2.0f);
    }

    // Render lines
    float scrollY = ImGui::GetScrollY();
    int firstLine = static_cast<int>(scrollY / lineHeight);
    int lastLine = firstLine + static_cast<int>(editorSize.y / lineHeight) + 2;

    for (int i = firstLine; i < lastLine && i < static_cast<int>(lines.size()); ++i)
    {
        float yPos = screenPos.y + (i * lineHeight) - scrollY + 5.0f;

        // Line number
        if (showLineNumbers)
        {
            char lineNum[16];
            snprintf(lineNum, sizeof(lineNum), "%4d", i + 1);
            ImVec4 numColor = lines[i].hasError ? colorError : colorLineNumber;

            drawList->AddText(ImVec2(screenPos.x + 10, yPos),
                ImGui::ColorConvertFloat4ToU32(numColor), lineNum);
        }

        // Colored text
        RenderTextWithSyntaxHighlighting(lines[i].content, i,
            ImVec2(screenPos.x + lineNumberWidth + 10, yPos));
    }

    // Dummy for scrolling
    ImGui::Dummy(ImVec2(1000.0f, lines.size() * lineHeight));

    // Double-click to edit
    if (ImGui::IsWindowHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        enableSyntaxHighlighting = false;
        LOG_CONSOLE("[ScriptEditor] Edit mode");
    }

    ImGui::EndChild();

    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
        "Double-click to edit | Ctrl+H to toggle highlighting");
}

void ScriptEditorWindow::DrawEditableView(ImVec2 editorSize, float lineNumberWidth)
{
    ImGui::BeginChild("EditableEditor", editorSize, true);

    if (showLineNumbers)
    {
        ImGui::BeginChild("LineNums", ImVec2(lineNumberWidth, 0), true, ImGuiWindowFlags_NoScrollbar);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

        for (size_t i = 0; i < lines.size(); ++i)
        {
            ImVec4 numColor = lines[i].hasError ? colorError : colorLineNumber;
            ImGui::TextColored(numColor, "%4zu", i + 1);
        }

        ImGui::PopStyleColor();
        ImGui::EndChild();
        ImGui::SameLine();
    }

    ImGui::BeginChild("TextArea", ImVec2(0, 0), false);

    ImGui::PushStyleColor(ImGuiCol_FrameBg, colorBackground);

    bool textChanged = ImGui::InputTextMultiline(
        "##text",
        textBuffer,
        bufferSize,
        ImVec2(-1, -1),
        ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackAlways,
        [](ImGuiInputTextCallbackData* data) -> int {
            ScriptEditorWindow* editor = (ScriptEditorWindow*)data->UserData;
            editor->MarkAsModified();
            return 0;
        },
        this
    );

    ImGui::PopStyleColor();

    if (textChanged && autoCheckSyntax)
    {
        CheckSyntaxErrors();
    }

    ImGui::EndChild();
    ImGui::EndChild();

    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
        "Ctrl+H to enable syntax highlighting");
}

void ScriptEditorWindow::DrawLineNumbers()
{
}

void ScriptEditorWindow::DrawErrorPanel()
{
    ImGui::Separator();

    if (ImGui::BeginChild("ErrorPanel", ImVec2(0, 60), true))
    {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Syntax Errors:");
        ImGui::Separator();

        for (const auto& error : syntaxErrors)
        {
            ImGui::TextColored(colorError, "Line %d: %s", error.lineNumber, error.message.c_str());
        }
    }
    ImGui::EndChild();
}

void ScriptEditorWindow::DrawStatusBar()
{
    ImGui::Separator();

    std::string status = currentScriptPath.empty() ? "No file loaded" : currentScriptPath;

    if (hasUnsavedChanges)
    {
        status += " *";
    }

    ImGui::Text("%s", status.c_str());

    ImGui::SameLine(ImGui::GetWindowWidth() - 400);

    if (enableSyntaxHighlighting)
    {
        ImGui::TextColored(ImVec4(0.4f, 0.85f, 0.94f, 1.0f), "[VIEW]");
    }
    else
    {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "[EDIT]");
    }

    ImGui::SameLine();

    if (!syntaxErrors.empty())
    {
        ImGui::TextColored(colorError, "%zu error(s)", syntaxErrors.size());
        ImGui::SameLine();
    }
    else if (!currentScriptPath.empty())
    {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "OK");
        ImGui::SameLine();
    }

    ImGui::Text("Lines: %zu", lines.size());
}

void ScriptEditorWindow::RenderTextWithSyntaxHighlighting(const std::string& text, int lineNumber, ImVec2 startPos)
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    std::string currentToken;
    bool inString = false;
    bool inComment = false;
    char stringChar = '\0';
    float currentX = startPos.x;

    auto flushToken = [&]() {
        if (currentToken.empty()) return;

        TokenType type = TokenType::IDENTIFIER;

        if (inComment)
            type = TokenType::COMMENT;
        else if (inString)
            type = TokenType::STRING;
        else
            type = IdentifyToken(currentToken);

        ImVec4 color = GetColorForToken(type);

        drawList->AddText(
            ImVec2(currentX, startPos.y),
            ImGui::ColorConvertFloat4ToU32(color),
            currentToken.c_str()
        );

        currentX += ImGui::CalcTextSize(currentToken.c_str()).x;
        currentToken.clear();
        };

    for (size_t i = 0; i < text.length(); ++i)
    {
        char c = text[i];
        char nextC = (i + 1 < text.length()) ? text[i + 1] : '\0';

        if (!inString && c == '-' && nextC == '-')
        {
            flushToken();
            inComment = true;
            currentToken += c;
            continue;
        }

        if (!inComment && (c == '"' || c == '\''))
        {
            if (!inString)
            {
                flushToken();
                inString = true;
                stringChar = c;
                currentToken += c;
            }
            else if (c == stringChar)
            {
                currentToken += c;
                flushToken();
                inString = false;
                stringChar = '\0';
            }
            else
            {
                currentToken += c;
            }
            continue;
        }

        if (inString || inComment)
        {
            currentToken += c;
            continue;
        }

        if (std::isspace(c) || c == '(' || c == ')' || c == '{' || c == '}' ||
            c == '[' || c == ']' || c == ',' || c == ';' || c == ':' ||
            c == '+' || c == '-' || c == '*' || c == '/' || c == '=' ||
            c == '<' || c == '>' || c == '~' || c == '.')
        {
            flushToken();

            if (!std::isspace(c))
            {
                currentToken += c;
                flushToken();
            }
            else
            {
                currentX += ImGui::CalcTextSize(" ").x;
            }
        }
        else
        {
            currentToken += c;
        }
    }

    flushToken();
}

TokenType ScriptEditorWindow::IdentifyToken(const std::string& token)
{
    if (token.empty()) return TokenType::NONE;

    if (IsLuaKeyword(token)) return TokenType::KEYWORD;
    if (IsLuaFunction(token)) return TokenType::FUNCTION;

    bool isNumber = true;
    bool hasDot = false;
    for (size_t i = 0; i < token.size(); ++i)
    {
        char c = token[i];
        if (i == 0 && (c == '-' || c == '+')) continue;
        if (c == '.' && !hasDot) { hasDot = true; continue; }
        if (!std::isdigit(c)) { isNumber = false; break; }
    }
    if (isNumber && token != "-" && token != "+") return TokenType::NUMBER;

    if (token.length() == 1 && std::ispunct(token[0])) return TokenType::OPERATOR;

    return TokenType::IDENTIFIER;
}

bool ScriptEditorWindow::IsLuaKeyword(const std::string& word)
{
    return luaKeywords.find(word) != luaKeywords.end();
}

bool ScriptEditorWindow::IsLuaFunction(const std::string& word)
{
    return luaBuiltinFunctions.find(word) != luaBuiltinFunctions.end();
}

ImVec4 ScriptEditorWindow::GetColorForToken(TokenType type)
{
    switch (type)
    {
    case TokenType::KEYWORD:    return colorKeyword;
    case TokenType::FUNCTION:   return colorFunction;
    case TokenType::STRING:     return colorString;
    case TokenType::NUMBER:     return colorNumber;
    case TokenType::COMMENT:    return colorComment;
    case TokenType::OPERATOR:   return colorOperator;
    default:                    return colorIdentifier;
    }
}

void ScriptEditorWindow::ParseTextIntoLines()
{
    lines.clear();

    std::stringstream ss(textBuffer);
    std::string line;

    while (std::getline(ss, line))
    {
        TextLine textLine;
        textLine.content = line;
        textLine.hasError = false;
        lines.push_back(textLine);
    }

    if (lines.empty())
    {
        TextLine empty;
        empty.content = "";
        empty.hasError = false;
        lines.push_back(empty);
    }
}

void ScriptEditorWindow::CheckSyntaxErrors()
{
    syntaxErrors.clear();

    for (auto& line : lines)
    {
        line.hasError = false;
        line.errorMessage.clear();
    }

    lua_State* L = luaL_newstate();
    if (!L) return;

    int result = luaL_loadstring(L, textBuffer);

    if (result != LUA_OK)
    {
        const char* error = lua_tostring(L, -1);
        std::string errorMsg = error ? error : "Unknown error";

        size_t colonPos = errorMsg.find("]:");
        if (colonPos != std::string::npos)
        {
            size_t lineStart = colonPos + 2;
            size_t lineEnd = errorMsg.find(":", lineStart);

            if (lineEnd != std::string::npos)
            {
                try
                {
                    std::string lineNumStr = errorMsg.substr(lineStart, lineEnd - lineStart);
                    int lineNum = std::stoi(lineNumStr);
                    std::string message = errorMsg.substr(lineEnd + 2);

                    SyntaxError syntaxError;
                    syntaxError.lineNumber = lineNum;
                    syntaxError.message = message;
                    syntaxErrors.push_back(syntaxError);

                    if (lineNum > 0 && lineNum <= static_cast<int>(lines.size()))
                    {
                        lines[lineNum - 1].hasError = true;
                        lines[lineNum - 1].errorMessage = message;
                    }
                }
                catch (...) {}
            }
        }

        lua_pop(L, 1);
    }

    lua_close(L);
}

void ScriptEditorWindow::OpenScript(const std::string& scriptPath)
{
    if (LoadScriptFromFile(scriptPath))
    {
        currentScriptPath = scriptPath;
        hasUnsavedChanges = false;
        isOpen = true;
        enableSyntaxHighlighting = true;

        ParseTextIntoLines();

        if (autoCheckSyntax)
        {
            CheckSyntaxErrors();
        }

        LOG_CONSOLE("[ScriptEditor] Opened: %s", scriptPath.c_str());
    }
}

bool ScriptEditorWindow::LoadScriptFromFile(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return false;

    std::stringstream buffer;
    buffer << file.rdbuf();
    scriptContent = buffer.str();
    originalContent = scriptContent;
    file.close();

    if (scriptContent.size() >= 3 &&
        (unsigned char)scriptContent[0] == 0xEF &&
        (unsigned char)scriptContent[1] == 0xBB &&
        (unsigned char)scriptContent[2] == 0xBF)
    {
        scriptContent = scriptContent.substr(3);
    }

    strncpy_s(textBuffer, bufferSize, scriptContent.c_str(), _TRUNCATE);
    return true;
}

bool ScriptEditorWindow::SaveScript()
{
    if (currentScriptPath.empty()) return false;

    std::ofstream file(currentScriptPath, std::ios::binary);
    if (!file.is_open()) return false;

    file << textBuffer;
    file.close();

    originalContent = textBuffer;
    hasUnsavedChanges = false;

    LOG_CONSOLE("[ScriptEditor] Saved: %s", currentScriptPath.c_str());

    if (autoCheckSyntax)
    {
        CheckSyntaxErrors();
    }

    return true;
}

void ScriptEditorWindow::MarkAsModified()
{
    if (strcmp(textBuffer, originalContent.c_str()) != 0)
    {
        hasUnsavedChanges = true;
    }
}