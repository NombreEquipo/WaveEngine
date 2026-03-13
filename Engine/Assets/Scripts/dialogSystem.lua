local TYPEWRITER_SPEED = 0.03
local DEFAULT_SEQUENCE = "intro"

public = {
    updateWhenPaused = true
}

local canvas     = nil
local allDialogs = nil

local PORTRAIT_MAP = {
    ["Telemaco"]    = "Portrait_Telemaco",
    ["Atenea"]      = "Portrait_Atenea",
    ["John cartel"] = "Portrait_JohnCartel",
}

local currentPortrait = nil

local state = {
    active          = false,
    currentSequence = nil,
    currentIndex    = 0,
    fullText        = "",
    displayedChars  = 0,
    timer           = 0.0,
    isComplete      = false,
}

local function setPortrait(character)
    if currentPortrait then
        UI.SetElementVisibility(currentPortrait, false)
    end
    local elementName = PORTRAIT_MAP[character]
    if elementName then
        UI.SetElementVisibility(elementName, true)
        currentPortrait = elementName
    end
end

local function loadDialogs()
    if allDialogs then return true end
    local ok, result = pcall(dofile, "../Assets/Scripts/dialogs.lua")
    if not ok or not result then
        Engine.Log("[DialogSystem] ERROR loading dialogs.lua: " .. tostring(result))
        return false
    end
    allDialogs = result
    Engine.Log("[DialogSystem] Dialogs loaded")
    return true
end

local function updateUI()
    UI.SetElementText("DialogText", string.sub(state.fullText, 1, state.displayedChars))
    UI.SetElementVisibility("ContinueIcon", state.isComplete)
end

local function loadDialogEntry(entry)
    UI.SetElementText("CharacterName", entry.character or "")
    setPortrait(entry.character)
    state.fullText       = entry.text or ""
    state.displayedChars = 0
    state.timer          = 0.0
    state.isComplete     = false
    updateUI()
end

local function startSequence(sequenceId)
    if not loadDialogs() then return end
    local seq = allDialogs[sequenceId]
    if not seq then
        Engine.Log("[DialogSystem] Sequence not found: " .. tostring(sequenceId))
        return
    end
    state.active          = true
    state.currentSequence = seq.dialogs
    state.currentIndex    = 1
    Game.Pause()
    UI.SetElementVisibility("DialogBox", true)
    loadDialogEntry(state.currentSequence[1])
    Engine.Log("[DialogSystem] Started: " .. sequenceId)
end

local function closeDialog()
    if currentPortrait then
        UI.SetElementVisibility(currentPortrait, false)
        currentPortrait = nil
    end
    state.active          = false
    state.currentSequence = nil
    state.currentIndex    = 0
    UI.SetElementVisibility("DialogBox", false)
    Game.Resume()
    Engine.Log("[DialogSystem] Closed")
end

local function onAdvancePressed()
    if not state.active then
        startSequence(DEFAULT_SEQUENCE)
        return
    end
    if not state.isComplete then
        state.displayedChars = #state.fullText
        state.isComplete     = true
        updateUI()
        return
    end
    local nextIndex = state.currentIndex + 1
    if nextIndex <= #state.currentSequence then
        state.currentIndex = nextIndex
        loadDialogEntry(state.currentSequence[nextIndex])
    else
        closeDialog()
    end
end

-- Función pública para llamar desde un trigger externo
function TriggerSequence(sequenceId)
    startSequence(sequenceId)
end

function Start(self)
    canvas = self.gameObject:GetComponent("Canvas")
    if not canvas then
        Engine.Log("[DialogSystem] ERROR: No ComponentCanvas found")
        return
    end
    UI.SetElementVisibility("Portrait_Telemaco", false)
    UI.SetElementVisibility("Portrait_Atenea", false)
    UI.SetElementVisibility("Portrait_JohnCartel", false)
    UI.SetElementVisibility("DialogBox", false)
    UI.SetElementVisibility("ContinueIcon", false)
    UI.SetElementText("DialogText", "")
    UI.SetElementText("CharacterName", "")
    Engine.Log("[DialogSystem] Ready")
end

function Update(self, dt)
    if Input.GetKeyDown("P") then
        onAdvancePressed()
    end

    if not state.active or state.isComplete then return end

    state.timer = state.timer + dt
    local charsToShow = math.floor(state.timer / TYPEWRITER_SPEED)
    if charsToShow > state.displayedChars then
        state.displayedChars = math.min(charsToShow, #state.fullText)
        updateUI()
        if state.displayedChars >= #state.fullText then
            state.isComplete = true
            UI.SetElementVisibility("ContinueIcon", true)
        end
    end
end