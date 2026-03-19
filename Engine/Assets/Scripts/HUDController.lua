-- HUDController.lua

local STAMINA_BAR_MAX_HEIGHT = 56.0
local HEALTH_BAR_MAX_HEIGHT  = 74.0

local function RefreshHealthBar(health)
    local clampedHealth = math.max(0, math.min(100, health))
    UI.SetElementHeight("HealthGrid", (clampedHealth / 100.0) * HEALTH_BAR_MAX_HEIGHT)
end

local function RefreshStaminaBar(stamina)
    local clampedStamina = math.max(0, math.min(100, stamina))
    UI.SetElementHeight("StaminaGrid", (clampedStamina / 100.0) * STAMINA_BAR_MAX_HEIGHT)
end

local function RefreshPotionUI(potions)
    local clampedPotions = math.max(0, potions)
    UI.SetElementText("PotionsNumber", tostring(clampedPotions))
    UI.SetElementVisibility("Potion_Image",  clampedPotions > 0)
    UI.SetElementVisibility("PotionsNumber", clampedPotions > 0)
end

-- Force a full refresh of the HUD
function ForceRefreshHUD()
    if _G.PlayerInstance and _G.PlayerInstance.public then
        local p = _G.PlayerInstance.public
        RefreshHealthBar(p.health)
        RefreshStaminaBar(p.stamina)
    else
        RefreshHealthBar(100)
        RefreshStaminaBar(100)
    end

    local potions = (_G.PotionSystem and _G.PotionSystem.public) and _G.PotionSystem.public.potionCount or 0
    RefreshPotionUI(potions)
end
_G.ForceRefreshHUD = ForceRefreshHUD

function Start(self)
    ForceRefreshHUD()
end

function Update(self, dt)
    if _G.PlayerInstance and _G.PlayerInstance.public then
        local p = _G.PlayerInstance.public
        RefreshHealthBar(p.health)
        RefreshStaminaBar(p.stamina)
    end

    local potions = (_G.PotionSystem and _G.PotionSystem.public) and _G.PotionSystem.public.potionCount or 0
    RefreshPotionUI(potions)
end