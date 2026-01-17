#pragma once

#include "EditorWindow.h"
#include <imgui.h>
#include <string>
#include <vector>
#include <unordered_set>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

enum class TokenType
{
    NONE,
    KEYWORD,
    FUNCTION,
    STRING,
    NUMBER,
    COMMENT,
    OPERATOR,
    IDENTIFIER
};

struct SyntaxError
{
    int lineNumber = 0;
    std::string message;
};

struct TextLine
{
    std::string content;
    bool hasError = false;
    std::string errorMessage;
};

class ScriptEditorWindow : public EditorWindow
{
public:
    ScriptEditorWindow();
    ~ScriptEditorWindow();

    void Draw() override;

    // Open a script for editing
    void OpenScript(const std::string& scriptPath);

    // Save current script
    bool SaveScript();

    // Check if there are unsaved changes
    bool HasUnsavedChanges() const { return hasUnsavedChanges; }

private:
    void DrawMenuBar();
    void DrawTextEditor();
    void DrawColoredReadOnlyView(ImVec2 editorSize, float lineNumberWidth);
    void DrawEditableView(ImVec2 editorSize, float lineNumberWidth);
    void DrawLineNumbers();
    void DrawErrorPanel();
    void DrawStatusBar();

    bool LoadScriptFromFile(const std::string& path);
    void MarkAsModified();

    // Syntax highlighting
    void InitializeLuaKeywords();
    ImVec4 GetColorForToken(TokenType type);
    void RenderTextWithSyntaxHighlighting(const std::string& text, int lineNumber, ImVec2 startPos);
    TokenType IdentifyToken(const std::string& token);
    bool IsLuaKeyword(const std::string& word);
    bool IsLuaFunction(const std::string& word);

    // Error checking
    void CheckSyntaxErrors();
    void ParseTextIntoLines();

    std::string currentScriptPath;
    std::string scriptContent;
    std::string originalContent;

    bool hasUnsavedChanges;

    // Text editor state
    char* textBuffer;
    size_t bufferSize;

    // Editor settings
    bool showLineNumbers;
    float fontSize;
    bool enableSyntaxHighlighting;
    bool autoCheckSyntax;

    // Search/Replace
    char searchText[256];
    char replaceText[256];
    bool showSearchReplace;

    // Syntax highlighting data
    std::unordered_set<std::string> luaKeywords;
    std::unordered_set<std::string> luaBuiltinFunctions;

    // Line tracking
    std::vector<TextLine> lines;
    std::vector<SyntaxError> syntaxErrors;

    // Colors
    ImVec4 colorKeyword;
    ImVec4 colorFunction;
    ImVec4 colorString;
    ImVec4 colorNumber;
    ImVec4 colorComment;
    ImVec4 colorOperator;
    ImVec4 colorIdentifier;
    ImVec4 colorError;
    ImVec4 colorLineNumber;
    ImVec4 colorBackground;

    // Scroll position
    float scrollY;
    int currentLine;
};